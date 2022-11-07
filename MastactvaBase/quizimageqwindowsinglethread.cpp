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


bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::prepareContext(QOpenGLContext *context, QOffscreenSurface *offscreenSurface)
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

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::postContext(QOpenGLContext *context)
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

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::run(
        //QuizImageQWindowSingleThread* qwindow,
        QOpenGLContext *context,
        QOffscreenSurface *offscreenSurface,
        const QSize &windowSize
        )
{
    //disconnect(m_qmlComponent, &QQmlComponent::statusChanged, qwindow, &QuizImageQWindowSingleThread::run);

    if (m_qmlComponent->isError())
    {
        const QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
        {
            qWarning() << error.url() << error.line() << error;
        }
        return false;
    }

    QObject *rootObject = m_qmlComponent->create();
    if (m_qmlComponent->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
        {
            qWarning() << error.url() << error.line() << error;
        }
        return false;
    }

    m_rootItem = qobject_cast<QQuickItem *>(rootObject);
    if (!m_rootItem)
    {
        qWarning("run: Not a QQuickItem");
        delete rootObject;
        return false;
    }

    // The root item is ready. Associate it with the window.
    m_rootItem->setParentItem(m_quickWindow->contentItem());

    // Update item and rendering related geometries.
    updateSizes(windowSize);

    // Ensure key events are received by the root Rectangle.
    m_rootItem->forceActiveFocus();

    // Initialize the render control and our OpenGL resources.
    // setup QOpenGLContext into the QOffscreenSurface
    context->makeCurrent(offscreenSurface);
    // setup graphic device into the QQuickWindow
    m_quickWindow->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(context));
    m_renderControl->initialize();

    return true;
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::updateSizes(
        const QSize &windowSize
        )
{
    // Behave like SizeRootObjectToView.
    m_rootItem->setWidth(windowSize.width());
    m_rootItem->setHeight(windowSize.height());

    m_quickWindow->setGeometry(0, 0, windowSize.width(), windowSize.height());
}

QQmlComponent* QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::getQmlComponent()
{
    return m_qmlComponent.get();
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::render(QOpenGLContext *context)
{
    if (!context ||
        !m_renderControl
        )
    {
        return false;
    }

    // Polish, synchronize and render the next frame (into our texture).  In this example
    // everything happens on the same thread and therefore all three steps are performed
    // in succession from here. In a threaded setup the render() call would happen on a
    // separate thread.
    m_renderControl->beginFrame();
    m_renderControl->polishItems();
    m_renderControl->sync();
    m_renderControl->render();
    m_renderControl->endFrame();

    QOpenGLFramebufferObject::bindDefault();
    // flush QOpenGLContext
    context->functions()->glFlush();

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

    m_defaultRender = std::make_unique<DefaultTextureRender>(m_offscreenSurface.get());

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
    QuizImageQMLDrawingSurface::prepareContext(m_context.get(), m_offscreenSurface.get());
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.free(m_context.get());
    }
    QuizImageQMLDrawingSurface::postContext(m_context.get());

    //m_defaultRenderer.reset();
    m_drawingSurfaces.clear();
    m_defaultRender.reset();
    m_offscreenSurface.reset();
    m_context.reset();
}

void QuizImageQWindowSingleThread::exposeEvent(QExposeEvent *e)
{
    // TODO: add implementation
    Q_UNUSED(e);
    /*
    if (isExposed()) {
        if (!m_quickInitialized) {
            // run rendering
            // for this QWindow and m_context QOpenGLContext
            // (possibly this mostly for initialization)
            m_defaultRenderer->render(this, m_context, m_quickReady ? m_textureId[0] : 0, m_quickReady ? m_textureId[1] : 0);
            startQuick(QStringLiteral("qrc:/rendercontrol/demo.qml"), QStringLiteral("qrc:/rendercontrol/demo2.qml"));
        }
    }
    */
}

void QuizImageQWindowSingleThread::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    /*
    // If this is a resize after the scene is up and running, recreate the texture and the
    // Quick item and scene.
    if (m_textureId[0] && m_textureId[0] && m_textureSize != size() * devicePixelRatio())
        resizeTexture();
    */
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
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        disconnect(surface.getQmlComponent(), &QQmlComponent::statusChanged, this, &QuizImageQWindowSingleThread::run);
    }
    m_quickInitialized = true;
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        m_quickInitialized &= surface.run(m_context.get(), m_offscreenSurface.get(), QSize{width(), height()});
    }
}

void QuizImageQWindowSingleThread::createTexture()
{
    m_dpr = devicePixelRatio();
    m_textureSize = size() * m_dpr;

    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.createTexture(m_context.get(), m_textureSize);
    }
}

void QuizImageQWindowSingleThread::destroyTexture()
{
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.deleteTexture(m_context.get());
    }
}

void QuizImageQWindowSingleThread::render()
{
    m_quickReady = true;
    QuizImageQMLDrawingSurface::prepareContext(m_context.get(), m_offscreenSurface.get());
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        m_quickReady &= surface.render(m_context.get());
    }
    m_defaultRender->render(this, m_context.get(), 0); // TODO: setup textures
}

void QuizImageQWindowSingleThread::requestUpdate()
{
    if (m_updateTimer && !m_updateTimer->isActive())
    {
        m_updateTimer->start();
    }
}

void QuizImageQWindowSingleThread::handleScreenChange()
{
    /*
    if (m_dpr != devicePixelRatio())
        resizeTexture();
    */
}

void QuizImageQWindowSingleThread::startQuick(const QString &filename)
{
    Q_UNUSED(filename);
    /*
    m_qmlComponent[0] = new QQmlComponent(m_qmlEngine[0], QUrl(filename1));
    m_qmlComponent[1] = new QQmlComponent(m_qmlEngine[1], QUrl(filename2));
    bool startRun = false;
    if (m_qmlComponent[0]->isLoading())
        connect(m_qmlComponent[0], &QQmlComponent::statusChanged, this, &WindowSingleThreaded::run);
    else
        startRun = true;
    if (m_qmlComponent[1]->isLoading())
        connect(m_qmlComponent[1], &QQmlComponent::statusChanged, this, &WindowSingleThreaded::run);
    else
        startRun = true;
    if(startRun)
        run();
    */
}

void QuizImageQWindowSingleThread::updateSizes()
{
    /*
    // Behave like SizeRootObjectToView.
    m_rootItem[0]->setWidth(width());
    m_rootItem[0]->setHeight(height());
    m_rootItem[1]->setWidth(width());
    m_rootItem[1]->setHeight(height());

    m_quickWindow[0]->setGeometry(0, 0, width(), height());
    m_quickWindow[1]->setGeometry(0, 0, width(), height());

    // resize render screen size
    // work with drawing matrix
    m_defaultRenderer->resize(width(), height());
    */
}

void QuizImageQWindowSingleThread::resizeTexture()
{
    /*
    if (m_rootItem[0] && m_rootItem[1] && m_context->makeCurrent(m_offscreenSurface)) {
        m_context->functions()->glDeleteTextures(1, &m_textureId[0]);
        m_context->functions()->glDeleteTextures(1, &m_textureId[1]);
        m_textureId[0] = 0;
        m_textureId[1] = 0;
        createTexture();
        m_context->doneCurrent();
        updateSizes();
        render();
    }
    */
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
