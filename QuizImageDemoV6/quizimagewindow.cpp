/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quizimagewindow.h"
#include "cuberenderer.h"
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
    RenderControl(QWindow *w) : m_window(w) { }
    QWindow *renderWindow(QPoint *offset) override;

private:
    QWindow *m_window;
};

QWindow *RenderControl::renderWindow(QPoint *offset)
{
    if (offset)
        *offset = QPoint(0, 0);
    return m_window;
}

WindowSingleThreaded::WindowSingleThreaded()
    : m_rootItem{nullptr, nullptr},
      m_textureId{0,0},
      m_quickInitialized(false),
      m_quickReady(false),
      m_dpr(0)
{
    setSurfaceType(QSurface::OpenGLSurface);

    QSurfaceFormat format;
    // Qt Quick may need a depth and stencil buffer. Always make sure these are available.
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    setFormat(format);

    // create QOpenGLContext
    m_context = new QOpenGLContext;
    m_context->setFormat(format);
    m_context->create();

    m_offscreenSurface = new QOffscreenSurface;
    // Pass m_context->format(), not format. Format does not specify and color buffer
    // sizes, while the context, that has just been created, reports a format that has
    // these values filled in. Pass this to the offscreen surface to make sure it will be
    // compatible with the context's configuration.
    // (use current QOpenGLContext format for QOffscreenSurface)
    m_offscreenSurface->setFormat(m_context->format());
    m_offscreenSurface->create();

    // create opnegl render class for QOffscreenSurface
    m_cubeRenderer = new CubeRenderer(m_offscreenSurface);

    m_renderControl[0] = new RenderControl(this);
    m_renderControl[1] = new RenderControl(this);

    // Create a QQuickWindow that is associated with out render control. Note that this
    // window never gets created or shown, meaning that it will never get an underlying
    // native (platform) window.
    m_quickWindow[0] = new QQuickWindow(m_renderControl[0]);
    m_quickWindow[1] = new QQuickWindow(m_renderControl[1]);

    // Create a QML engine.
    m_qmlEngine[0] = new QQmlEngine;
    if (!m_qmlEngine[0]->incubationController())
        m_qmlEngine[0]->setIncubationController(m_quickWindow[0]->incubationController());
    m_qmlEngine[1] = new QQmlEngine;
    if (!m_qmlEngine[1]->incubationController())
        m_qmlEngine[1]->setIncubationController(m_quickWindow[1]->incubationController());

    // When Quick says there is a need to render, we will not render immediately. Instead,
    // a timer with a small interval is used to get better performance.
    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(5);
    connect(&m_updateTimer, &QTimer::timeout, this, &WindowSingleThreaded::render);

    // Now hook up the signals. For simplicy we don't differentiate between
    // renderRequested (only render is needed, no sync) and sceneChanged (polish and sync
    // is needed too).
    connect(m_quickWindow[0], &QQuickWindow::sceneGraphInitialized, this, &WindowSingleThreaded::createTexture);
    connect(m_quickWindow[0], &QQuickWindow::sceneGraphInvalidated, this, &WindowSingleThreaded::destroyTexture);
    connect(m_renderControl[0], &QQuickRenderControl::renderRequested, this, &WindowSingleThreaded::requestUpdate);
    connect(m_renderControl[0], &QQuickRenderControl::sceneChanged, this, &WindowSingleThreaded::requestUpdate);
    connect(m_quickWindow[1], &QQuickWindow::sceneGraphInitialized, this, &WindowSingleThreaded::createTexture);
    connect(m_quickWindow[1], &QQuickWindow::sceneGraphInvalidated, this, &WindowSingleThreaded::destroyTexture);
    connect(m_renderControl[1], &QQuickRenderControl::renderRequested, this, &WindowSingleThreaded::requestUpdate);
    connect(m_renderControl[1], &QQuickRenderControl::sceneChanged, this, &WindowSingleThreaded::requestUpdate);

    // Just recreating the texture on resize is not sufficient, when moving between screens
    // with different devicePixelRatio the QWindow size may remain the same but the texture
    // dimension is to change regardless.
    connect(this, &QWindow::screenChanged, this, &WindowSingleThreaded::handleScreenChange);
}

WindowSingleThreaded::~WindowSingleThreaded()
{
    // Make sure the context is current while doing cleanup. Note that we use the
    // offscreen surface here because passing 'this' at this point is not safe: the
    // underlying platform window may already be destroyed. To avoid all the trouble, use
    // another surface that is valid for sure.
    // make current QOpenGLContext as QOffscreeenSurface
    m_context->makeCurrent(m_offscreenSurface);

    delete m_qmlComponent[0];
    delete m_qmlEngine[0];
    delete m_quickWindow[0];
    delete m_renderControl[0];
    delete m_qmlComponent[1];
    delete m_qmlEngine[1];
    delete m_quickWindow[1];
    delete m_renderControl[1];

    if (m_textureId[0])
    {
        // delete texture inside current QOpenGLContext for QOffscreeenSurface
        m_context->functions()->glDeleteTextures(1, &m_textureId[0]);
    }
    if (m_textureId[1])
    {
        // delete texture inside current QOpenGLContext for QOffscreeenSurface
        m_context->functions()->glDeleteTextures(1, &m_textureId[1]);
    }

    m_context->doneCurrent();

    // remove opengl render
    delete m_cubeRenderer;

    delete m_offscreenSurface;
    delete m_context;
}

/*
 * here is the example of how to create several QQuickWindows and
 * how to setup textures for each of them
*/
void WindowSingleThreaded::createTexture()
{
    // The scene graph has been initialized. It is now time to create an texture and associate
    // it with the QQuickWindow.
    m_dpr = devicePixelRatio();
    m_textureSize = size() * m_dpr;
    // get opengl functions from the QOpenGLContext
    // we have already setup QOpenGLContext as QOffscreenSurface
    QOpenGLFunctions *f = m_context->functions();

    f->glGenTextures(1, &m_textureId[0]);
    f->glBindTexture(GL_TEXTURE_2D, m_textureId[0]);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureSize.width(), m_textureSize.height(), 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_quickWindow[0]->setRenderTarget(QQuickRenderTarget::fromOpenGLTexture(m_textureId[0], m_textureSize));

    // TODO: fix texture size
    f->glGenTextures(1, &m_textureId[1]);
    f->glBindTexture(GL_TEXTURE_2D, m_textureId[1]);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureSize.width(), m_textureSize.height(), 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_quickWindow[1]->setRenderTarget(QQuickRenderTarget::fromOpenGLTexture(m_textureId[1], m_textureSize));
}

void WindowSingleThreaded::destroyTexture()
{
    // maybe not setuped QOffscreenSurface
    m_context->functions()->glDeleteTextures(1, &m_textureId[0]);
    m_textureId[0] = 0;
    m_context->functions()->glDeleteTextures(1, &m_textureId[1]);
    m_textureId[1] = 0;
}

void WindowSingleThreaded::render()
{
    // setup rendering into the texture
    if (!m_context->makeCurrent(m_offscreenSurface))
        return;

    // Polish, synchronize and render the next frame (into our texture).  In this example
    // everything happens on the same thread and therefore all three steps are performed
    // in succession from here. In a threaded setup the render() call would happen on a
    // separate thread.
    m_renderControl[0]->beginFrame();
    m_renderControl[0]->polishItems();
    m_renderControl[0]->sync();
    m_renderControl[0]->render();
    m_renderControl[0]->endFrame();
    m_renderControl[1]->beginFrame();
    m_renderControl[1]->polishItems();
    m_renderControl[1]->sync();
    m_renderControl[1]->render();
    m_renderControl[1]->endFrame();

    QOpenGLFramebufferObject::bindDefault();
    // flush QOpenGLContext
    m_context->functions()->glFlush();

    m_quickReady = true;

    // Get something onto the screen.
    // run rendering for this QWindow and for m_context
    // with texture to use in drawings
    // (render to the current QWindow)
    m_cubeRenderer->render(this, m_context, m_quickReady ? m_textureId[0] : 0, m_quickReady ? m_textureId[1] : 0);
}

void WindowSingleThreaded::requestUpdate()
{
    if (!m_updateTimer.isActive())
        m_updateTimer.start();
}

void WindowSingleThreaded::run()
{
    disconnect(m_qmlComponent[0], &QQmlComponent::statusChanged, this, &WindowSingleThreaded::run);
    disconnect(m_qmlComponent[1], &QQmlComponent::statusChanged, this, &WindowSingleThreaded::run);

    if (m_qmlComponent[0]->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent[0]->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }
    if (m_qmlComponent[1]->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent[1]->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }

    QObject *rootObject0 = m_qmlComponent[0]->create();
    if (m_qmlComponent[0]->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent[0]->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }
    QObject *rootObject1 = m_qmlComponent[1]->create();
    if (m_qmlComponent[1]->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent[1]->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }

    m_rootItem[0] = qobject_cast<QQuickItem *>(rootObject0);
    if (!m_rootItem[0]) {
        qWarning("run: Not a QQuickItem");
        delete rootObject0;
        return;
    }
    m_rootItem[1] = qobject_cast<QQuickItem *>(rootObject1);
    if (!m_rootItem[1]) {
        qWarning("run: Not a QQuickItem");
        delete rootObject1;
        return;
    }

    // The root item is ready. Associate it with the window.
    m_rootItem[0]->setParentItem(m_quickWindow[0]->contentItem());
    m_rootItem[1]->setParentItem(m_quickWindow[1]->contentItem());

    // Update item and rendering related geometries.
    updateSizes();

    // Ensure key events are received by the root Rectangle.
    m_rootItem[0]->forceActiveFocus();
    m_rootItem[1]->forceActiveFocus();

    // Initialize the render control and our OpenGL resources.
    // setup QOpenGLContext into the QOffscreenSurface
    m_context->makeCurrent(m_offscreenSurface);
    // setup graphic device into the QQuickWindow
    m_quickWindow[0]->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(m_context));
    m_renderControl[0]->initialize();
    m_quickWindow[1]->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(m_context));
    m_renderControl[1]->initialize();
    m_quickInitialized = true;
}

void WindowSingleThreaded::updateSizes()
{
    // Behave like SizeRootObjectToView.
    m_rootItem[0]->setWidth(width());
    m_rootItem[0]->setHeight(height());
    m_rootItem[1]->setWidth(width());
    m_rootItem[1]->setHeight(height());

    m_quickWindow[0]->setGeometry(0, 0, width(), height());
    m_quickWindow[1]->setGeometry(0, 0, width(), height());

    // resize render screen size
    // work with drawing matrix
    m_cubeRenderer->resize(width(), height());
}

// TODO: use separate versions
void WindowSingleThreaded::startQuick(const QString &filename)
{
    m_qmlComponent[0] = new QQmlComponent(m_qmlEngine[0], QUrl(filename));
    m_qmlComponent[1] = new QQmlComponent(m_qmlEngine[1], QUrl(filename));
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
}

void WindowSingleThreaded::exposeEvent(QExposeEvent *)
{
    if (isExposed()) {
        if (!m_quickInitialized) {
            // run rendering
            // for this QWindow and m_context QOpenGLContext
            // (possibly this mostly for initialization)
            m_cubeRenderer->render(this, m_context, m_quickReady ? m_textureId[0] : 0, m_quickReady ? m_textureId[1] : 0);
            startQuick(QStringLiteral("qrc:/rendercontrol/demo.qml"));
        }
    }
}

void WindowSingleThreaded::resizeTexture()
{
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
}

void WindowSingleThreaded::resizeEvent(QResizeEvent *)
{
    // If this is a resize after the scene is up and running, recreate the texture and the
    // Quick item and scene.
    if (m_textureId[0] && m_textureId[0] && m_textureSize != size() * devicePixelRatio())
        resizeTexture();
}

void WindowSingleThreaded::handleScreenChange()
{
    if (m_dpr != devicePixelRatio())
        resizeTexture();
}

void WindowSingleThreaded::mousePressEvent(QMouseEvent *e)
{
    // Use the constructor taking position and globalPosition. That puts position into the
    // event's position and scenePosition, and globalPosition into the event's globalPosition. This way
    // the scenePosition in e is ignored and is replaced by position. This is necessary
    // because QQuickWindow thinks of itself as a top-level window always.
    QMouseEvent mappedEvent(e->type(), e->position(), e->globalPosition(), e->button(), e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(m_quickWindow[0], &mappedEvent);
    QCoreApplication::sendEvent(m_quickWindow[1], &mappedEvent);
}

void WindowSingleThreaded::mouseReleaseEvent(QMouseEvent *e)
{
    QMouseEvent mappedEvent(e->type(), e->position(), e->globalPosition(), e->button(), e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(m_quickWindow[0], &mappedEvent);
    QCoreApplication::sendEvent(m_quickWindow[1], &mappedEvent);
}

void WindowSingleThreaded::keyPressEvent(QKeyEvent *e)
{
    QCoreApplication::sendEvent(m_quickWindow[0], e);
    QCoreApplication::sendEvent(m_quickWindow[1], e);
}

void WindowSingleThreaded::keyReleaseEvent(QKeyEvent *e)
{
    QCoreApplication::sendEvent(m_quickWindow[0], e);
    QCoreApplication::sendEvent(m_quickWindow[1], e);
}
