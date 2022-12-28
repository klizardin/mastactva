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

#ifndef WINDOW_SINGLETHREADED_H
#define WINDOW_SINGLETHREADED_H

#include <QWindow>
#include <QMatrix4x4>
#include <QTimer>
#include <QQuickRenderControl>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickItem>

QT_BEGIN_NAMESPACE
class QOpenGLContext;
class QOpenGLTexture;
class QOffscreenSurface;
QT_END_NAMESPACE

class DefaultRenderer;

class WindowSingleThreaded : public QWindow
{
    Q_OBJECT

public:
    struct QMLTextureRenderer
    {
        std::unique_ptr<QQuickRenderControl> m_renderControl;
        QQuickWindow *m_quickWindow = nullptr;
        QQmlEngine *m_qmlEngine = nullptr;
        QQmlComponent *m_qmlComponent = nullptr;
        QQuickItem *m_rootItem = nullptr;
        uint m_textureId = 0;
    };

public:
    WindowSingleThreaded();
    ~WindowSingleThreaded();

protected:
    void exposeEvent(QExposeEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

private slots:
    void run();

    void createTexture();
    void destroyTexture();
    void render();
    void requestUpdate();
    void handleScreenChange();

private:
    void startQuick(const QString &filename1, const QString &filename2);
    void updateSizes();
    void resizeTexture();

    QOpenGLContext *m_context;

    // https://doc.qt.io/qt-6/qoffscreensurface.html
    QOffscreenSurface *m_offscreenSurface;

    QMLTextureRenderer m_qmlTextureRrenderer[2];
    QSize m_textureSize;
    bool m_quickInitialized;
    bool m_quickReady;
    QTimer m_updateTimer;

    // TODO: several steps of CubeRenderer ?
    DefaultRenderer *m_cubeRenderer;
    qreal m_dpr;
};

#endif
