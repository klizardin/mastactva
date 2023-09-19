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
#include "../MastactvaBase/quizimage.h"
#include "../MastactvaBase/names.h"


bool TextureNames::isDefaultTexcture(const QString &textureName_)
{
    return QString(g_renderTextureDefault) == textureName_
            || QString(g_renderTextureDefaultSynonim) == textureName_
            ;
}

TextureNames::TextureNames()
{
    addDefaultTextureName();
}

TextureNames::TextureNames(const std::vector<QString> &textureNames_)
    :std::vector<QString>(textureNames_)
{
    addDefaultTextureName();
}

bool TextureNames::hasDefaultTextureName() const
{
    return !empty() &&
            (std::find(std::begin(*this), std::end(*this), g_renderTextureDefault) != std::end(*this)
                || std::find(std::begin(*this), std::end(*this), g_renderTextureDefaultSynonim) != std::end(*this)
            )
            ;
}

void TextureNames::addDefaultTextureName()
{
    if(!hasDefaultTextureName())
    {
        push_back(g_renderTextureDefault);
    }
}


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


QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::QuizImageQMLDrawingSurface(int renderingWindowsId_)
    :m_renderingWindowsId(renderingWindowsId_)
{
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
    m_qmlEngine = std::make_unique<QQmlApplicationEngine>();
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
    if(m_textureCreated)
    {
        return true;
    }

    f->glGenTextures(1, &m_textureId);
    f->glBindTexture(GL_TEXTURE_2D, m_textureId);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.width(), textureSize.height(), 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_quickWindow->setRenderTarget(QQuickRenderTarget::fromOpenGLTexture(m_textureId, textureSize));
    m_textureCreated = true;

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
    m_textureCreated = false;

    return true;
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::run(
        //QuizImageQWindowSingleThread* qwindow,
        QOpenGLContext *context,
        QOffscreenSurface *offscreenSurface,
        const QSize &windowSize,
        bool runTestByTest
        )
{
    //disconnect(m_qmlComponent, &QQmlComponent::statusChanged, qwindow, &QuizImageQWindowSingleThread::run);
    m_quickInitialized = false;

    if (m_qmlComponent->isError())
    {
        const QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
        {
            qWarning() << error.url() << error.line() << error;
        }
        return;
    }

    // TODO: can we here set QQuickItem properties
    // https://stackoverflow.com/questions/26704621/how-to-create-a-qqmlcomponent-from-c-at-runtime
    // https://stackoverflow.com/questions/33611529/set-property-of-qml-object-using-c
    QObject *rootObject = m_qmlComponent->create();
    if (m_qmlComponent->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
        {
            qWarning() << error.url() << error.line() << error;
        }
        return;
    }

    m_rootItem = qobject_cast<QQuickItem *>(rootObject);
    if (!m_rootItem)
    {
        qWarning("run: Not a QQuickItem");
        delete rootObject;
        return;
    }

    // set renderingTextureName property of the QuizImage quick item
    QObject * quizImageObject = m_rootItem->findChild<QObject *>(QStringLiteral("quizImage"));
    QQuickItem *quizImageQuickItem = qobject_cast<QQuickItem*>(quizImageObject);
    if(!quizImageQuickItem)
    {
        qWarning("run: Cannot find the quizImage QQuickItem");
        delete rootObject;
        return;
    }
    quizImageQuickItem->setProperty("renderingTextureName", QVariant::fromValue(m_textureName));
    const int renderingWindowsId = getRenderingWindowsId();
    quizImageQuickItem->setProperty("renderingWindowsId", QVariant::fromValue(renderingWindowsId));
    quizImageQuickItem->setProperty("doRunTestsStepByStep", QVariant::fromValue(runTestByTest));

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

    QMetaObject::invokeMethod(quizImageQuickItem, "initDefaultDrawingData");

    m_quickInitialized = true;
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

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::hasTexture() const
{
    return m_textureId != 0;
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::startQuick(const QString &filename)
{
    if(m_qmlComponent)
    {
        return !m_qmlComponent->isLoading();
    }
    m_qmlEngine->addImportPath("qrc:/Mastactva");
    // TODO: is it possible to parametrise filename?
    m_qmlComponent = std::make_unique<QQmlComponent>(m_qmlEngine.get(), QUrl(filename));
    return !m_qmlComponent->isLoading();
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::setWindowSize(const QSize &windowSize)
{
    if(!m_rootItem || !m_quickWindow)
    {
        return;
    }

    m_rootItem->setWidth(windowSize.width());
    m_rootItem->setHeight(windowSize.height());
    m_quickWindow->setGeometry(0, 0, windowSize.width(), windowSize.height());
}

uint QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::getTexture() const
{
    return m_textureId;
}

const QString &QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::getTextureName() const
{
    return m_textureName;
}

void QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::setTextureName(const QString &texture)
{
    const int renderingWindowsId = getRenderingWindowsId();
    m_textureName = texture;

    if(!m_rootItem)
    {
        return;
    }

    QObject * quizImageObject = m_rootItem->findChild<QObject *>(QStringLiteral("renderer"));
    QQuickItem *quizImageQuickItem = qobject_cast<QQuickItem*>(quizImageObject);
    if(quizImageQuickItem)
    {
        quizImageQuickItem->setProperty("renderingWindowsId", QVariant::fromValue(renderingWindowsId));
        quizImageQuickItem->setProperty("renderingTextureName", QVariant::fromValue(m_textureName));
    }
    QMetaObject::invokeMethod(quizImageQuickItem, "initDefaultDrawingData");
}

int QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::getRenderingWindowsId() const
{
    return m_renderingWindowsId;
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::isDefaultTexture() const
{
    return TextureNames::isDefaultTexcture(getTextureName());
}

bool QuizImageQWindowSingleThread::QuizImageQMLDrawingSurface::isQuickInitialized() const
{
    return m_quickInitialized;
}


// TODO: add implementation
// just simple possible implementation
QuizImageQWindowSingleThread::QuizImageQWindowSingleThread(const QString & qmlFileName, bool runMultipleTests)
    : m_qmlFileName(qmlFileName),
      m_runTestByTest(runMultipleTests)
{
    IQuizImageQWindow::addQuizImageWindows(this);
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

    m_defaultRenderer = std::make_unique<DefaultTextureRender>(m_offscreenSurface.get());

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
    IQuizImageQWindow::removeQuizImageWindows(this);
    m_updateTimer.reset();
    QuizImageQMLDrawingSurface::prepareContext(m_context.get(), m_offscreenSurface.get());
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        surface.free(m_context.get());
    }
    QuizImageQMLDrawingSurface::postContext(m_context.get());

    //m_defaultRenderer.reset();
    m_drawingSurfaces.clear();
    m_defaultRenderer.reset();
    m_offscreenSurface.reset();
    m_context.reset();
}

bool QuizImageQWindowSingleThread::setTextures(const TextureNames & textures_)
{
    m_activeOffscreenSurafaces = std::min((int)textures_.size(), (int)m_drawingSurfaces.size());
    auto it = m_drawingSurfaces.begin();
    for(int i = 0; i < m_activeOffscreenSurafaces && it != m_drawingSurfaces.end(); i++, ++it)
    {
        it->setTextureName(textures_[i]);
    }
    return textures_.size() <= m_drawingSurfaces.size();
}

int QuizImageQWindowSingleThread::count() const
{
    return m_activeOffscreenSurafaces; // m_drawingSurfaces.size();
}

QString QuizImageQWindowSingleThread::at(int index) const
{
    auto it = std::begin(m_drawingSurfaces);
    std::advance(it, std::max(index, m_activeOffscreenSurafaces));
    return it != std::end(m_drawingSurfaces) ? it->getTextureName() : g_renderTextureDefault;
}

bool QuizImageQWindowSingleThread::isDefaultTexture(int index) const
{
    auto it = std::begin(m_drawingSurfaces);
    std::advance(it, std::max(index, m_activeOffscreenSurafaces));
    return it != std::end(m_drawingSurfaces) ? TextureNames::isDefaultTexcture(it->getTextureName()) : true;
}

QString QuizImageQWindowSingleThread::getCurrentTextureName() const
{
    return m_currentTextureName;
}

void QuizImageQWindowSingleThread::exposeEvent(QExposeEvent *e)
{
    Q_UNUSED(e);
    if (isExposed())
    {
        const bool quickInitialized = std::all_of(
                    std::begin(m_drawingSurfaces), std::end(m_drawingSurfaces),
                    [](const QuizImageQMLDrawingSurface &surface)->bool
        {
            return surface.isQuickInitialized();
        });
        if (!quickInitialized)
        {
            // run rendering
            // for this QWindow and m_context QOpenGLContext
            // (possibly this mostly for initialization)
            m_defaultRenderer->render(this, m_context.get() , getTextures());
            startQuick(m_qmlFileName);
        }
    }
}

void QuizImageQWindowSingleThread::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);

    // If this is a resize after the scene is up and running, recreate the texture and the
    // Quick item and scene.
    bool hasTexture = false;
    const auto newTextureSize = size() * devicePixelRatio();
    auto it = std::cbegin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::cend(m_drawingSurfaces); ++i, ++it)
    {
        hasTexture |= it->hasTexture();
    }
    if (hasTexture && newTextureSize != m_textureSize)
    {
        resizeTexture();
    }
}

void QuizImageQWindowSingleThread::mousePressEvent(QMouseEvent *e)
{
    // Use the constructor taking position and globalPosition. That puts position into the
    // event's position and scenePosition, and globalPosition into the event's globalPosition. This way
    // the scenePosition in e is ignored and is replaced by position. This is necessary
    // because QQuickWindow thinks of itself as a top-level window always.
    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        it->mousePressEvent(e);
    }
}

void QuizImageQWindowSingleThread::mouseReleaseEvent(QMouseEvent *e)
{
    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        it->mouseReleaseEvent(e);
    }
}

void QuizImageQWindowSingleThread::keyPressEvent(QKeyEvent *e)
{
    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        it->keyPressEvent(e);
    }
}

void QuizImageQWindowSingleThread::keyReleaseEvent(QKeyEvent *e)
{
    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        it->keyReleaseEvent(e);
    }
}

void QuizImageQWindowSingleThread::run()
{
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        disconnect(surface.getQmlComponent(), &QQmlComponent::statusChanged, this, &QuizImageQWindowSingleThread::run);
    }
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        if(!surface.isQuickInitialized())
        {
            surface.run(m_context.get(), m_offscreenSurface.get(), QSize{width(), height()}, m_runTestByTest);
        }
    }
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        if(!surface.isQuickInitialized())
        {
            connect(surface.getQmlComponent(), &QQmlComponent::statusChanged, this, &QuizImageQWindowSingleThread::run);
        }
    }
}

void QuizImageQWindowSingleThread::createTexture()
{
    m_dpr = devicePixelRatio();
    m_textureSize = size() * m_dpr;

    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        it->createTexture(m_context.get(), m_textureSize);
    }
}

void QuizImageQWindowSingleThread::destroyTexture()
{
    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        it->deleteTexture(m_context.get());
    }
}

void QuizImageQWindowSingleThread::render()
{
    m_quickReady = true;
    QuizImageQMLDrawingSurface::prepareContext(m_context.get(), m_offscreenSurface.get());
    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        m_currentTextureName = it->getTextureName();
        m_quickReady &= it->render(m_context.get());
    }
    m_defaultRenderer->render(this, m_context.get(), getTextures());
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
    if (m_dpr != devicePixelRatio())
    {
        resizeTexture();
    }
}

void QuizImageQWindowSingleThread::startQuick(const QString &filename)
{
    bool startRun = false;
    for(QuizImageQMLDrawingSurface &surface : m_drawingSurfaces)
    {
        if(!surface.startQuick(filename))
        {
            connect(surface.getQmlComponent(), &QQmlComponent::statusChanged, this, &QuizImageQWindowSingleThread::run);
        }
        else
        {
            startRun |= true;
        }
    }

    if(startRun)
    {
        run();
    }
}

void QuizImageQWindowSingleThread::updateSizes()
{
    auto it = std::begin(m_drawingSurfaces);
    for(int i = 0; i < m_activeOffscreenSurafaces && it != std::end(m_drawingSurfaces); ++i, ++it)
    {
        it->setWindowSize(QSize(width(), height()));
    }
    m_defaultRenderer->resize(width(), height());
}

void QuizImageQWindowSingleThread::resizeTexture()
{
    QuizImageQMLDrawingSurface::prepareContext(m_context.get(), m_offscreenSurface.get());
    destroyTexture();
    createTexture();
    QuizImageQMLDrawingSurface::postContext(m_context.get());
    updateSizes();
    render();
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
    for(int i = 0; i < g_maxDrawingSurfaceCount; ++i )
    {
        m_drawingSurfaces.push_back(QuizImageQMLDrawingSurface{i});
        if(!m_drawingSurfaces.back().create(this))
        {
            return false;
        }
    }
    setTextures(TextureNames{});
    return true;
}

std::vector<uint> QuizImageQWindowSingleThread::getTextures() const
{
    std::vector<uint> result;
    result.reserve(m_drawingSurfaces.size());
    auto it = m_drawingSurfaces.cbegin();
    for(int i = 0
        ; it != m_drawingSurfaces.cend() && i < m_activeOffscreenSurafaces
        ; ++it
        )
    {
        result.push_back(it->getTexture());
    }
    return result;
}


