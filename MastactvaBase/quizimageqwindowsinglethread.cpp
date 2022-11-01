/*
    Copyright 2022

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "quizimageqwindowsinglethread.h"
#include "defaulttexturerender.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOffscreenSurface>
#include <QScreen>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQuickRenderControl>
#include <QCoreApplication>
#include <QQuickRenderTarget>
#include <QQuickGraphicsDevice>


class RenderControl : public QQuickRenderControl
{
public:
    RenderControl(QWindow *w) : m_window(w)
    {
    }

    QWindow *renderWindow(QPoint *offset) override;

private:
    QWindow *m_window = nullptr;
};


QWindow *RenderControl::renderWindow(QPoint *offset)
{
    if (offset)
    {
        *offset = QPoint(0, 0);
    }
    return m_window;
}


bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::preFree(QOpenGLContext *context, QOffscreenSurface *offscreenSurface)
{
    if(!context || !offscreenSurface)
    {
        return false;
    }
    context->makeCurrent(offscreenSurface);
    return true;
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::free(QOpenGLContext *context)
{
    m_qmlComponent.reset();
    m_qmlEngine.reset();
    m_quickWindow.reset();
    m_renderControl.reset();
    m_rootItem = nullptr;

    if(!deleteTexture(context))
    {
        return false;
    }

    return true;
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::postFree(QOpenGLContext *context)
{
    if(!context)
    {
        return false;
    }
    context->doneCurrent();
    return true;
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    QMouseEvent mappedEvent(e->type(), e->position(), e->globalPosition(), e->button(), e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(m_quickWindow.get(), &mappedEvent);
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::mouseReleaseEvent(QMouseEvent *e)
{
    QMouseEvent mappedEvent(e->type(), e->position(), e->globalPosition(), e->button(), e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(m_quickWindow.get(), &mappedEvent);
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::keyPressEvent(QKeyEvent *e)
{
    QCoreApplication::sendEvent(m_quickWindow.get(), e);
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::keyReleaseEvent(QKeyEvent *e)
{
    QCoreApplication::sendEvent(m_quickWindow.get(), e);
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::create(QuizImageQWindowSingleThread *qwindow)
{
    if(!qwindow)
    {
        return false;
    }

    m_renderControl = std::make_unique<RenderControl>(qwindow);

    // Create a QQuickWindow that is associated with out render control. Note that this
    // window never gets created or shown, meaning that it will never get an underlying
    // native (platform) window.
    m_quickWindow = std::make_unique<QQuickWindow>(m_renderControl.get());

    // Create a QML engine.
    m_qmlEngine = std::make_unique<QQmlEngine>();
    if (!m_qmlEngine->incubationController())
        m_qmlEngine->setIncubationController(m_quickWindow->incubationController());

    // Now hook up the signals. For simplicy we don't differentiate between
    // renderRequested (only render is needed, no sync) and sceneChanged (polish and sync
    // is needed too).
    qwindow->connectDrawingSurface(m_renderControl.get(), m_quickWindow.get());

    return true;
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::createTexture(QOpenGLContext *context, const QSize &textureSize)
{
    if(!context || !m_quickWindow)
    {
        return false;
    }

    QOpenGLFunctions *f = context->functions();
    if(!f)
    {
        return false;
    }

    f->glGenTextures(1, &m_textureId);
    f->glBindTexture(GL_TEXTURE_2D, m_textureId);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.width(), textureSize.height(), 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_quickWindow->setRenderTarget(QQuickRenderTarget::fromOpenGLTexture(m_textureId, textureSize));

    return true;
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::deleteTexture(QOpenGLContext *context)
{
    if(!context || !m_textureId || !context->functions())
    {
        return false;
    }

    context->functions()->glDeleteTextures(1, &m_textureId);
    m_textureId = 0;

    return true;
}


// TODO: add implementation
// just simple possible implementation
QuizImageQWindowSingleThread::QuizImageQWindowSingleThread()
{
    setSurfaceType(QSurface::OpenGLSurface);

    QSurfaceFormat format;
    // Qt Quick may need a depth and stencil buffer. Always make sure these are available.
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    setFormat(format);

    // create QOpenGLContext
    m_context = std::make_unique<QOpenGLContext>();
    m_context->setFormat(format);
    m_context->create();

    m_offscreenSurface = std::make_unique<QOffscreenSurface>();

    // Pass m_context->format(), not format. Format does not specify and color buffer
    // sizes, while the context, that has just been created, reports a format that has
    // these values filled in. Pass this to the offscreen surface to make sure it will be
    // compatible with the context's configuration.
    // (use current QOpenGLContext format for QOffscreenSurface)
    m_offscreenSurface->setFormat(m_context->format());
    m_offscreenSurface->create();

    // create opnegl render class for QOffscreenSurface
    //m_defaultRenderer = new DefaultRenderer(m_offscreenSurface);

    if(!createSurface())
    {
        return;
    }

    // When Quick says there is a need to render, we will not render immediately. Instead,
    // a timer with a small interval is used to get better performance.
    m_updateTimer = std::make_unique<QTimer>();
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(5);
    connect(m_updateTimer.get(), &QTimer::timeout, this, &QuizImageQWindowSingleThread::render);
}

QuizImageQWindowSingleThread::~QuizImageQWindowSingleThread()
{
    m_updateTimer.reset();
    QuizImageQMLDrawingSurface::preFree(m_context.get(), m_offscreenSurface.get());
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.free(m_context.get());
    }
    QuizImageQMLDrawingSurface::postFree(m_context.get());

    //m_defaultRenderer.reset();
    m_drawingSurfaces.clear();
    m_offscreenSurface.reset();
    m_context.reset();
}

void QuizImageQWindowSingleThread::exposeEvent(QExposeEvent *e)
{
    Q_UNUSED(e);
}

void QuizImageQWindowSingleThread::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
}

void QuizImageQWindowSingleThread::mousePressEvent(QMouseEvent *e)
{
    // Use the constructor taking position and globalPosition. That puts position into the
    // event's position and scenePosition, and globalPosition into the event's globalPosition. This way
    // the scenePosition in e is ignored and is replaced by position. This is necessary
    // because QQuickWindow thinks of itself as a top-level window always.
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.mousePressEvent(e);
    }
}

void QuizImageQWindowSingleThread::mouseReleaseEvent(QMouseEvent *e)
{
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.mouseReleaseEvent(e);
    }
}

void QuizImageQWindowSingleThread::keyPressEvent(QKeyEvent *e)
{
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.keyPressEvent(e);
    }
}

void QuizImageQWindowSingleThread::keyReleaseEvent(QKeyEvent *e)
{
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.keyReleaseEvent(e);
    }
}

void QuizImageQWindowSingleThread::run()
{
}

void QuizImageQWindowSingleThread::createTexture()
{
}

void QuizImageQWindowSingleThread::destroyTexture()
{
}

void QuizImageQWindowSingleThread::render()
{
}

void QuizImageQWindowSingleThread::requestUpdate()
{
}

void QuizImageQWindowSingleThread::handleScreenChange()
{
}

void QuizImageQWindowSingleThread::startQuick(const QString &filename)
{
    Q_UNUSED(filename);
}

void QuizImageQWindowSingleThread::updateSizes()
{
}

void QuizImageQWindowSingleThread::resizeTexture()
{
}

void QuizImageQWindowSingleThread::connectDrawingSurface(QQuickRenderControl * renderControl, QQuickWindow * quickWindow)
{
    connect(quickWindow, &QQuickWindow::sceneGraphInitialized, this, &QuizImageQWindowSingleThread::createTexture);
    connect(quickWindow, &QQuickWindow::sceneGraphInvalidated, this, &QuizImageQWindowSingleThread::destroyTexture);
    connect(renderControl, &QQuickRenderControl::renderRequested, this, &QuizImageQWindowSingleThread::requestUpdate);
    connect(renderControl, &QQuickRenderControl::sceneChanged, this, &QuizImageQWindowSingleThread::requestUpdate);
}

bool QuizImageQWindowSingleThread::createSurface()
{
    m_drawingSurfaces.emplace_back();
    if(m_drawingSurfaces.back().create(this))
    {
        return true;
    }

    if(!m_drawingSurfaces.empty())
    {
        auto itback = std::end(m_drawingSurfaces);
        --itback;
        m_drawingSurfaces.erase(itback);
    }
    return false;
}
