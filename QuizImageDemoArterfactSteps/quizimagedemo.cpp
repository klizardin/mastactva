#include "quizimagedemo.h"
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtCore/QDebug>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include "../MastactvaBase/openglquizimage.h"


QuizImageDemo::QuizImageDemo(QQuickItem *parent_ /*= nullptr*/)
    : QQuickItem(parent_)
{
    setFlag(ItemHasContents);
}

QSGNode *QuizImageDemo::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGRenderNode *n = static_cast<QSGRenderNode *>(node);

    QSGRendererInterface *ri = window()->rendererInterface();
    if (!ri)
        return nullptr;

    switch (ri->graphicsApi()) {
    case QSGRendererInterface::OpenGL:
        Q_FALLTHROUGH();
    case QSGRendererInterface::OpenGLRhi:
#if QT_CONFIG(opengl)
        if (nullptr == n) { n = new OpenGlQuizImage(); }
        static_cast<OpenGlQuizImage *>(n)->sync(this);
#endif
        break;

    case QSGRendererInterface::MetalRhi:
// Restore when QTBUG-78580 is done and the .pro is updated accordingly
//#ifdef Q_OS_DARWIN
        break;

    case QSGRendererInterface::Direct3D12: // ### Qt 6: remove
        break;

    case QSGRendererInterface::Software:
        break;

    default:
        break;
    }

    if (!n)
        qWarning("QSGRendererInterface reports unknown graphics API %d", ri->graphicsApi());

    return n;
}

// This item does not support being moved between windows. If that is desired,
// itemChange() should be reimplemented as well.

void QuizImageDemo::updateState()
{
    update();
}

qreal QuizImageDemo::t() const
{
    return m_t;
}

void QuizImageDemo::setT(const qreal &t_)
{
    m_t = t_;

    updateState();
    emit tChanged();
}

QString QuizImageDemo::fromImage() const
{
    return m_fromImage;
}

void QuizImageDemo::setFromImage(const QString &fromImage_)
{
    m_fromImage = fromImage_;

    updateState();
    emit fromImageChanged();
}

QString QuizImageDemo::toImage() const
{
    return m_toImage;
}

void QuizImageDemo::setToImage(const QString &toImage_)
{
    m_toImage = toImage_;

    updateState();
    emit toImageChanged();
}

QString QuizImageDemo::project() const
{
    return m_project;
}

void QuizImageDemo::setProject(const QString &project_)
{
    m_project = project_;

    emit projectChanged();
}
