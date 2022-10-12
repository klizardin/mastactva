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

#ifndef QUIZIMAGEQWINDOWSINGLETHREAD_H
#define QUIZIMAGEQWINDOWSINGLETHREAD_H

#include <QWindow>
#include <QObject>
#include <QWidget>
#include <QMatrix4x4>
#include <QTimer>


QT_BEGIN_NAMESPACE
class QOpenGLContext;
class QOpenGLTexture;
class QOffscreenSurface;
class QQuickRenderControl;
class QQuickWindow;
class QQmlEngine;
class QQmlComponent;
class QQuickItem;
QT_END_NAMESPACE


class QuizImageQWindowSingleThread : public QWindow
{
    Q_OBJECT

protected:
    struct QuizImageQMLDrawingSurface
    {
        std::unique_ptr<QQuickRenderControl> m_renderControl;
        std::unique_ptr<QQuickWindow> m_quickWindow;
        std::unique_ptr<QQmlEngine> m_qmlEngine;
        std::unique_ptr<QQmlComponent> m_qmlComponent;
        std::unique_ptr<QQuickItem> m_rootItem;
        uint m_textureId = -1;
    };

public:
    QuizImageQWindowSingleThread();
    ~QuizImageQWindowSingleThread() override;

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
    void startQuick(const QString &filename);
    void updateSizes();
    void resizeTexture();

private:
    std::unique_ptr<QOpenGLContext> m_context;

    // https://doc.qt.io/qt-6/qoffscreensurface.html
    std::unique_ptr<QOffscreenSurface> m_offscreenSurface;

    std::vector<QuizImageQMLDrawingSurface> m_drawingSurfaces;

    QSize m_textureSize = QSize{};
    bool m_quickInitialized = false;
    bool m_quickReady = false;
    std::unique_ptr<QTimer> m_updateTimer;
    qreal m_dpr = 1.0;
};

#endif // QUIZIMAGEQWINDOWSINGLETHREAD_H
