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

QVariantList QuizImage::fromImage() const
{
    QVariantList res;
    res.push_back(QVariant::fromValue(m_fromImageUrl));
    return res;
}

void QuizImage::setFromImage(const QVariantList &fromImageInfo_)
{
    if(fromImageInfo_.size() < 2) { return; }
    const QString imageUrl = fromImageInfo_.at(0).toString();
    const QString imageHash = fromImageInfo_.at(1).toString();

    if(imageUrl == m_fromImageUrl) { return; }

    m_fromImageUrl = imageUrl;

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::connect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(imageDownloadedSlot(const QString &)));
        sf->add(imageUrl, imageHash, g_imagesCachePath);
    }
    emit fromImageChanged();
}

QVariantList QuizImage::toImage() const
{
    QVariantList res;
    res.push_back(QVariant::fromValue(m_toImageUrl));
    return res;
}

void QuizImage::setToImage(const QVariantList &toImageInfo_)
{
    if(toImageInfo_.size() < 2) { return; }
    const QString imageUrl = toImageInfo_.at(0).toString();
    const QString imageHash = toImageInfo_.at(1).toString();

    if(imageUrl == m_fromImageUrl) { return; }

    m_toImageUrl = imageUrl;

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::connect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(imageDownloadedSlot(const QString &)));
        sf->add(imageUrl, imageHash, g_imagesCachePath);
    }

    emit toImageChanged();
}

QVariant QuizImage::effect() const
{
    return QVariant::fromValue(static_cast<QObject *>(const_cast<Effect *>(m_effect)));
}

void QuizImage::setEffect(const QVariant &effect_)
{
    QObject *obj = qvariant_cast<QObject *>(effect_);
    Effect *effect = qobject_cast<Effect *>(obj);
    if(m_effect == effect) { return; }

    m_effect = effect;
    QObject::connect(m_effect, SIGNAL(childrenLoaded()), this, SLOT(effectChildrenLoadedSlot()));
    if(m_effect->isChildrenLoaded())
    {
        updateStateIfDataIsReady();
    }

    emit effectChanged();
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

bool QuizImage::areAllDataAvailable() const
{
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        if(!sf->isUrlDownloaded(m_fromImageUrl) || !sf->isUrlDownloaded(m_toImageUrl))
        {
            return false;
        }
    }
    if(nullptr != m_effect)
    {
        if(!m_effect->isChildrenLoaded())
        {
            return false;
        }
    }
    return true;
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

void QuizImage::imageDownloadedSlot(const QString &url_)
{
    Q_UNUSED(url_);
    updateStateIfDataIsReady();
}

void QuizImage::effectChildrenLoadedSlot()
{
    updateStateIfDataIsReady();
}

void QuizImage::updateStateIfDataIsReady()
{
    const bool canUpdate = areAllDataAvailable();
    if(!canUpdate) { return; }

    if(nullptr != m_effect)
    {
        QObject::disconnect(m_effect, SIGNAL(childrenLoaded()), this, SLOT(effectChildrenLoadedSlot()));
    }
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::disconnect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(imageDownloadedSlot(const QString &)));
    }
    updateState();
}
