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
#include <QQmlApplicationEngine>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include "defaulttexturerender.h"
#include "../MastactvaBase/iquizimageqwindow.h"


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
class QuizImageQWindowSingleThread : public QWindow, public IQuizImageQWindowOperations
{
    Q_OBJECT

protected:
    class QuizImageQMLDrawingSurface
    {
    public:
        QuizImageQMLDrawingSurface(int renderingWindowsId_);
        QuizImageQMLDrawingSurface(QuizImageQMLDrawingSurface &&surface_) = default;
        ~QuizImageQMLDrawingSurface() = default;

        QuizImageQMLDrawingSurface & operator=(QuizImageQMLDrawingSurface &&surface_) = default;

        bool create(QuizImageQWindowSingleThread *qwindow);
        static bool prepareContext(QOpenGLContext *context, QOffscreenSurface *offscreenSurface);
        bool free(QOpenGLContext *context);
        static bool postContext(QOpenGLContext *context);
        void mousePressEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *e);
        void keyPressEvent(QKeyEvent *e);
        void keyReleaseEvent(QKeyEvent *e);
        bool createTexture(QOpenGLContext *context, const QSize &textureSize);
        bool deleteTexture(QOpenGLContext *context);
        bool run(
                //QuizImageQWindowSingleThread* qwindow,
                QOpenGLContext *context,
                QOffscreenSurface *offscreenSurface,
                const QSize &windowSize
                );
        void updateSizes(const QSize &windowSize);
        QQmlComponent* getQmlComponent();
        bool render(QOpenGLContext *context);
        bool hasTexture() const;
        bool startQuick(const QString &filename);
        void setWindowSize(const QSize &windowSize);
        uint getTexture() const;
        const QString &getTextureName() const;
        void setTextureName(const QString &texture);
        int getRenderingWindowsId() const;
        bool isDefaultTexture() const;

    private:
        std::unique_ptr<QQuickRenderControl> m_renderControl;
        std::unique_ptr<QQuickWindow> m_quickWindow;
        std::unique_ptr<QQmlApplicationEngine> m_qmlEngine;
        std::unique_ptr<QQmlComponent> m_qmlComponent;
        QQuickItem * m_rootItem = nullptr;
        uint m_textureId = 0;
        QString m_textureName;
        int m_renderingWindowsId = 0;
    };

public:
    QuizImageQWindowSingleThread(const QString & qmlFileName);
    ~QuizImageQWindowSingleThread() override;

    void setTextures(const TextureNames & textures_) override;
    int count() const override;
    QString at(int index) const override;
    bool isDefaultTexture(int index) const override;
    QString getCurrentTextureName() const override;

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
    std::vector<uint> getTextures() const;

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
    std::unique_ptr<DefaultTextureRender> m_defaultRenderer;
    QString m_qmlFileName;
    QString m_currentTextureName;
};


#endif // QUIZIMAGEQWINDOWSINGLETHREAD_H
