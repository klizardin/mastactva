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
#include <list>
#include <vector>
#include <string>
#include <memory>


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


// TODO: add interface for new drawing surface
class QuizImageQWindowSingleThread : public QWindow
{
    Q_OBJECT

protected:
    class QuizImageQMLDrawingSurface
    {
    public:
        QuizImageQMLDrawingSurface() = default;
        ~QuizImageQMLDrawingSurface() = default;
        bool create(QuizImageQWindowSingleThread *qwindow);
        static bool preFree(QOpenGLContext *context, QOffscreenSurface *offscreenSurface);
        bool free(QOpenGLContext *context);
        static bool postFree(QOpenGLContext *context);

    private:
        std::unique_ptr<QQuickRenderControl> m_renderControl;
        std::unique_ptr<QQuickWindow> m_quickWindow;
        std::unique_ptr<QQmlEngine> m_qmlEngine;
        std::unique_ptr<QQmlComponent> m_qmlComponent;
        QQuickItem * m_rootItem = nullptr;
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
    void connectDrawingSurface(QQuickRenderControl * renderControl, QQuickWindow * quickWindow);
    bool createSurface();

private:
    std::unique_ptr<QOpenGLContext> m_context;

    // https://doc.qt.io/qt-6/qoffscreensurface.html
    std::unique_ptr<QOffscreenSurface> m_offscreenSurface;

    std::list<QuizImageQMLDrawingSurface> m_drawingSurfaces;

    QSize m_textureSize = QSize{};
    bool m_quickInitialized = false;
    bool m_quickReady = false;
    std::unique_ptr<QTimer> m_updateTimer;
    qreal m_dpr = 1.0;
};

#endif // QUIZIMAGEQWINDOWSINGLETHREAD_H
