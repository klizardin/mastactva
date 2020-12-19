#include "quizimage.h"
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
#include <QSGRenderNode>
#include "openglquizimage.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"


QuizImage::QuizImage(QQuickItem *parent_ /*= nullptr*/)
    : QQuickItem(parent_)
{
    setFlag(ItemHasContents);
}

void QuizImage::updateState()
{
    update();
}

QString QuizImage::fromImage() const
{
    return m_fromImage;
}

void QuizImage::setFromImage(const QString &fromImage_)
{
    if(fromImage_ == m_fromImage) { return; }

    m_fromImage = fromImage_;

    delete m_image;
    m_image = nullptr;

    // update image size
    m_image = new QImage(m_fromImage);
    const QSize imageOrigSize = m_image->size();
    const qreal imageRate = (qreal)std::max(1,imageOrigSize.width())/(qreal)std::max(1,imageOrigSize.height());
    const qreal rectRate = std::max(1.0, width())/std::max(1.0, height());
    if(rectRate > imageRate)
    {
        m_imageSize = QSize(height()*imageRate, height());
    }
    else
    {
        m_imageSize = QSize(width(), width()/imageRate);
    }

    updateState();
    emit fromImageChanged();
    emit paintedWidthChanged();
    emit paintedHeightChanged();
}

QString QuizImage::toImage() const
{
    return m_toImage;
}

void QuizImage::setToImage(const QString &toImage_)
{
    if(toImage_ == m_toImage) { return; }

    m_toImage = toImage_;

    updateState();
    emit toImageChanged();
}

QString QuizImage::fragmentShaderFilename()
{
    return m_fragmentShaderFilename;
}

void QuizImage::setFragmentShaderFilename(const QString &fragmentShaderFilename_)
{
    if(fragmentShaderFilename_ == m_fragmentShaderFilename)
    {
        QUrl url = QUrl::fromLocalFile(fragmentShaderFilename_);
        const QString newHash = calculateFileURLHash(url.toString());
        if(newHash == m_fragmentShaderFilenameHash) { return; }
        m_fragmentShaderFilenameHash = newHash;
    }

    m_fragmentShaderFilename = fragmentShaderFilename_;

    updateState();
    emit fragmentShaderFilenameChanged();
}

QString QuizImage::vertexShaderFilename()
{
    return m_vertexShaderFilename;
}

void QuizImage::setVertexShaderFilename(const QString &vertexShaderFilename_)
{
    if(vertexShaderFilename_ == m_vertexShaderFilename)
    {
        QUrl url = QUrl::fromLocalFile(vertexShaderFilename_);
        const QString newHash = calculateFileURLHash(url.toString());
        if(newHash == m_vertexShaderFilenameHash) { return; }
        m_vertexShaderFilenameHash = newHash;
    }

    m_vertexShaderFilename = vertexShaderFilename_;

    updateState();
    emit vertexShaderFilenameChanged();
}

qreal QuizImage::t() const
{
    return m_t;
}

void QuizImage::setT(qreal t_)
{
    m_t = t_;

    updateState();
    emit tChanged();
}

int QuizImage::paintedWidth() const
{
    return std::max(1, m_imageSize.width());
}

int QuizImage::paintedHeight() const
{
    return std::max(1, m_imageSize.height());
}

QSGNode *QuizImage::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
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
        if (!n)
            n = new OpenGlQuizImage(this);
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
