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

void QuizImage::updateEffects()
{
    m_updateEffects = true;
    updateState();
}

void QuizImage::swapImages()
{
    std::swap(m_fromImageUrl, m_toImageUrl);

    delete m_image;
    m_image = nullptr;

    updateStateIfDataIsReady();

    emit fromImageChanged();
    emit toImageChanged();
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

    if(imageUrl == m_fromImageUrl || imageHash.isEmpty()) { return; }

    m_fromImageUrl = imageUrl;
    delete m_image;
    m_image = nullptr;

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::connect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(imageDownloadedSlot(const QString &)));
        sf->add(imageUrl, imageHash, g_imagesRelPath);
    }
    emit fromImageChanged();
}

QString QuizImage::fromImageLocalUrl() const
{
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        return sf->get(m_fromImageUrl);
    }
    return QString();
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

    if(imageUrl == m_toImageUrl || imageHash.isEmpty()) { return; }

    m_toImageUrl = imageUrl;

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::connect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(imageDownloadedSlot(const QString &)));
        sf->add(imageUrl, imageHash, g_imagesRelPath);
    }

    emit toImageChanged();
}

QString QuizImage::toImageLocalUrl() const
{
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        return sf->get(m_toImageUrl);
    }
    return QString();
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

    m_effectLoading = true;
    m_effect = effect;

    QObject::connect(m_effect, SIGNAL(childrenLoaded()), this, SLOT(effectChildrenLoadedSlot()));
    if(m_effect->isChildrenLoaded())
    {
        addShadersToWaitDownload();
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

QVariant QuizImage::argumentSet() const
{
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgSet *>(m_argumentSet)));
}

void QuizImage::setArgumentSet(const QVariant &argumentSet_)
{
    QObject *obj = qvariant_cast<QObject *>(argumentSet_);
    EffectArgSet *argumentSet = qobject_cast<EffectArgSet *>(obj);
    if(m_argumentSet == argumentSet) { return; }

    m_argumentSet = argumentSet;

    QObject::connect(m_argumentSet, SIGNAL(childrenLoaded()), this, SLOT(effectChildrenLoadedSlot()));
    if(m_argumentSet->isChildrenLoaded())
    {
        updateStateIfDataIsReady();
    }

    emit argumentSetChanged();
}

int QuizImage::paintedWidth() const
{
    return std::max(1, m_imageSize.width());
}

int QuizImage::paintedHeight() const
{
    return std::max(1, m_imageSize.height());
}

bool QuizImage::areAllDataAvailable()
{
    if(nullptr != m_argumentSet)
    {
        if(!m_argumentSet->isChildrenLoaded()) { return false; }
    }
    if(nullptr != m_effect)
    {
        if(!m_effect->isChildrenLoaded()) { return false; }
        else
        {
            if(m_effectLoading)
            {
                addShadersToWaitDownload();
                return false;
            }
        }
    }
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        if(!sf->isUrlDownloaded(m_fromImageUrl)
                || !sf->isUrlDownloaded(m_toImageUrl)
                ) { return false; }
        for(const QString &shaderUrl : m_shadersUrls)
        {
            if(!sf->isUrlDownloaded(shaderUrl)) { return false; }
        }
    }
    return true;
}

Effect *QuizImage::getEffect() const
{
    return m_effect;
}

bool QuizImage::needToUpdateEffects()
{
    bool res = m_updateEffects;
    m_updateEffects = false;
    return res;
}

EffectArgSet *QuizImage::getArgumentSet() const
{
    return m_argumentSet;
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

    m_shadersUrls.clear();
    if(nullptr != m_argumentSet)
    {
        QObject::disconnect(m_argumentSet, SIGNAL(childrenLoaded()), this, SLOT(effectChildrenLoadedSlot()));
    }
    if(nullptr != m_effect)
    {
        QObject::disconnect(m_effect, SIGNAL(childrenLoaded()), this, SLOT(effectChildrenLoadedSlot()));
    }
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::disconnect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(imageDownloadedSlot(const QString &)));
    }
    if(nullptr == m_image && nullptr != sf)
    {
        const QString fileUrl = sf->get(m_fromImageUrl);
        QUrl url(fileUrl);
        m_image = new QImage(url.path());
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

        emit paintedWidthChanged();
        emit paintedHeightChanged();
    }
    updateState();
}

void QuizImage::addShadersToWaitDownload()
{
    m_effectLoading = false;

    m_shadersUrls.clear();
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();

    EffectShaderModel *shaders = m_effect->getEffectShaders();
    Q_ASSERT(nullptr != shaders && shaders->isListLoaded());
    for(int i = 0; i < shaders->sizeImpl(); i++)
    {
        EffectShader *effect_shader = shaders->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effect_shader);
        ShaderModel *shaderModel = effect_shader->getShader();
        Q_ASSERT(nullptr != shaderModel && shaderModel->isListLoaded() && shaderModel->sizeImpl() > 0);
        Shader *shader = shaderModel->dataItemAtImpl(0);
        m_shadersUrls.push_back(shader->filename());
        if(nullptr != sf)
        {
            sf->add(shader->filename(), shader->hash(), g_shadersRelPath);
        }
    }
    updateStateIfDataIsReady();
}
