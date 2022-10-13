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


// TODO: add implementation
// just simple possible implementation
QuizImageQWindowSingleThread::QuizImageQWindowSingleThread()
{

}

QuizImageQWindowSingleThread::~QuizImageQWindowSingleThread()
{
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
    Q_UNUSED(e);
}

void QuizImageQWindowSingleThread::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
}

void QuizImageQWindowSingleThread::keyPressEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
}

void QuizImageQWindowSingleThread::keyReleaseEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
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
