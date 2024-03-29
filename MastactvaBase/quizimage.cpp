/*
    Copyright 2021

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

#include "quizimage.h"
//#include <QtGui/QOpenGLFramebufferObject>
//#include <QtGui/QOpenGLContext>
//#include <QtGui/QOpenGLFunctions>
//#include <QtGui/QOpenGLTexture>
//#include <QtGui/QOpenGLShaderProgram>
//#include <QtGui/QOpenGLBuffer>
//#include <QtGui/QOpenGLShader>
//#include <QtCore/QDebug>
//#include <QQuickWindow>
//#include <QSGRendererInterface>
//#include <QSGRenderNode>
//#include "../MastactvaBase/openglquizimage.h"
//#include "../MastactvaBase/utils.h"
#include <math.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include <QRandomGenerator>
#include <QTime>
#include <QVector>
#include "../MastactvaBase/quizimageopengldrawingdata.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


/*QuizImage::QuizImage(QQuickItem *parent_)
    : QQuickItem(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "QuizImage::QuizImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    setFlag(ItemHasContents);
}

void QuizImage::updateState()
{
    update();
}

void QuizImage::updateEffects()
{
    m_updateEffects = true;
    formImageData();
    updateState();
}

void QuizImage::swapImages()
{
#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::swapImages()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    std::swap(m_fromImageUrl, m_toImageUrl);
    m_t = 1.0 - m_t;

    delete m_image;
    m_image = nullptr;

    updateStateIfDataIsReady();

    emit fromImageChanged();
    emit toImageChanged();
    emit tChanged();
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

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::setFromImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    const QString imageUrl = fromImageInfo_.at(0).toString();
    const QString imageHash = fromImageInfo_.at(1).toString();

    if(imageUrl == m_fromImageUrl || imageHash.isEmpty()) { return; }

    m_fromImageUrl = imageUrl;

    //qDebug() << "setFromImage() " << m_fromImageUrl;
    delete m_image;
    m_image = nullptr;

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::connect(
                    sf, SIGNAL(downloaded(const QString &)), this,
                    SLOT(imageDownloadedSlot(const QString &)));
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

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::setToImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    const QString imageUrl = toImageInfo_.at(0).toString();
    const QString imageHash = toImageInfo_.at(1).toString();

    if(imageUrl == m_toImageUrl || imageHash.isEmpty()) { return; }

    m_toImageUrl = imageUrl;
    //qDebug() << "setToImage() " << m_toImageUrl;

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::connect(
                    sf, SIGNAL(downloaded(const QString &)),
                    this, SLOT(imageDownloadedSlot(const QString &)));
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
#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::setEffect()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    QObject *obj = qvariant_cast<QObject *>(effect_);
    Effect *effect = qobject_cast<Effect *>(obj);
    if(m_effect == effect) { return; }

    m_effect = effect;

    if(nullptr != m_effect)
    {
        m_effectLoading = true;
        QObject::connect(
                    m_effect, SIGNAL(childrenLoaded()),
                    this, SLOT(effectChildrenLoadedSlot()));
        if(m_effect->isChildrenLoaded())
        {
            addShadersToWaitDownload();
        }
    }
    else
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
    //qDebug() << "QuizImage::setT()" << QThread::currentThread() << QThread::currentThreadId();

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
#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::setArgumentSet()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    QObject *obj = qvariant_cast<QObject *>(argumentSet_);
    EffectArgSet *argumentSet = qobject_cast<EffectArgSet *>(obj);
    if(m_argumentSet == argumentSet) { return; }

    m_argumentSet = argumentSet;

    if(nullptr != m_argumentSet)
    {
        QObject::connect(
                    m_argumentSet, SIGNAL(childrenLoaded()),
                    this, SLOT(effectChildrenLoadedSlot()));
        if(m_argumentSet->isChildrenLoaded())
        {
            updateStateIfDataIsReady();
        }
    }
    else
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

QString QuizImage::shadersBuildLog()
{
    return m_renderBuildLog;
}

void QuizImage::setShadersBuildLog(const QString &log_)
{
    m_lastLog = log_;
    m_renderBuildLog = m_lastLog;

    emit shadersBuildLogChanged();
}

void QuizImage::renderBuildError(const QString &log_)
{
    if(log_ == m_lastLog) { return; }

    m_lastLog = log_;
    m_renderBuildLog += m_lastLog;

    emit shadersBuildLogChanged();
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
        for(const QString &artefactUrl : qAsConst(m_artefactsUrls))
        {
            if(!sf->isUrlDownloaded(artefactUrl)) { return false; }
        }
    }
    return true;
}

bool QuizImage::isImageDataUpdated() const
{
    return m_imageDataUpdated;
}

void QuizImage::retryData()
{
    m_imageDataUpdated = false;
    m_effectUpdated = false;
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

bool QuizImage::isEffectUpdated() const
{
    return m_effectUpdated;
}

OpenGLDrawingImageData *QuizImage::getData()
{
    return m_data.getDrawingData();
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
#if defined(TRACE_THREADS_QUIZIMAGE)
        qDebug() << "QuizImage::updatePaintNode()" << QThread::currentThread() << QThread::currentThreadId();
#endif
        if (!n)
        {
            n = new OpenGlQuizImage();
        }
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
    //qDebug() << "imageDownloadedSlot() url = " << url_;
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

    // free resource for waiting for resource loaded
    m_artefactsUrls.clear();
    if(nullptr != m_argumentSet)
    {
        QObject::disconnect(
                    m_argumentSet, SIGNAL(childrenLoaded()),
                    this, SLOT(effectChildrenLoadedSlot()));
    }
    if(nullptr != m_effect)
    {
        QObject::disconnect(
                    m_effect, SIGNAL(childrenLoaded()),
                    this, SLOT(effectChildrenLoadedSlot()));
    }
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::disconnect(
                    sf, SIGNAL(downloaded(const QString &)),
                    this, SLOT(imageDownloadedSlot(const QString &)));
    }

    // update image size
    if(nullptr == m_image && nullptr != sf)
    {
        const QString fileUrl = sf->get(m_fromImageUrl);
        QUrl url(fileUrl);
        m_image = new QImage(url.path());
        const QSize imageOrigSize = m_image->size();
        const qreal imageRate = (qreal)std::max(1,imageOrigSize.width())
                / (qreal)std::max(1,imageOrigSize.height())
                ;
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

    formImageData();
    updateState();
}

void QuizImage::addShadersToWaitDownload()
{
    m_effectLoading = false;

    m_artefactsUrls.clear();
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();

    EffectObjectsModel *effectObjectsModel = m_effect->getEffectObjects();
    Q_ASSERT(nullptr != effectObjectsModel && effectObjectsModel->isListLoaded());

    for(int i = 0; i < effectObjectsModel->sizeImpl(); i++ )
    {
        EffectObjects *effectObjects = effectObjectsModel->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effectObjects);

        ObjectArtefactModel *effectObjectArtefactModel = effectObjects->getObjectArtefacts();
        Q_ASSERT(nullptr != effectObjectArtefactModel && effectObjectArtefactModel->isListLoadedImpl());

        for(int j = 0; j < effectObjectArtefactModel->sizeImpl(); j++)
        {
            ObjectArtefact *effectObjectArtefact = effectObjectArtefactModel->dataItemAtImpl(j);
            Q_ASSERT(nullptr != effectObjectArtefact);

            ArtefactModel * artefactModel = effectObjectArtefact->getArtefact();
            Q_ASSERT(nullptr != artefactModel && artefactModel->isListLoadedImpl());

            for(int k = 0; k < artefactModel->sizeImpl(); k++)
            {
                Artefact *artefact = artefactModel->dataItemAtImpl(k);
                Q_ASSERT(nullptr != artefact);
                m_artefactsUrls.push_back(artefact->filename());
                if(nullptr != sf)
                {
                    sf->add(artefact->filename(), artefact->hash(), g_artefactsRelPath);
                }
            }
        }
    }
    updateStateIfDataIsReady();
}

void QuizImage::formImageData()
{
    // all data
    m_imageDataUpdated = true;
    m_data.setFromImageUrl(fromImageLocalUrl());
    m_data.setToImageUrl(toImageLocalUrl());

    const Effect *effect = getEffect();
    const EffectArgSet *argumentSet = getArgumentSet();
    const bool updateEffectRequired = needToUpdateEffects();
    m_data.setEffect(effect);
    m_data.setArgumentSet(argumentSet);
    if(updateEffectRequired || m_data.isEffectChanged())
    {
        m_effectUpdated = true;
        m_data.prepareDrawingData();
    }
}*/


namespace std
{
    void swap(QuizImage::Image &i1_, QuizImage::Image &i2_) noexcept
    {
        std::swap(i1_.name, i2_.name);
        std::swap(i1_.hash, i2_.hash);
        std::swap(i1_.loaded, i2_.loaded);
    }
}


QuizImage::QuizImage()
{
    initDefaultDrawingData();

    const QString hash = calculateFileURLHash(g_noImageName);
    m_fromImage = { g_noImageName, hash, QString(), true };
    m_toImage = { g_noImageName, hash, QString(), true };
}

QQuickFramebufferObject::Renderer *QuizImage::createRenderer() const
{
    return new QuizImageFboRenderer<QuizImage>();
}

void QuizImage::updateState()
{
    update();
}

void QuizImage::updateEffects()
{
    // TODO: implement
}

void QuizImage::swapImages()
{
#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::swapImages()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    std::swap(m_fromImage, m_toImage);
    m_t = 1.0 - m_t;

    updateStateIfDataIsReady();
}

qreal QuizImage::t() const
{
    return m_t;
}

void QuizImage::setT(const qreal &t_)
{
    m_t = t_;

    updateState();
    emit tChanged();
}

QVariantList QuizImage::fromImage() const
{
    QVariantList res;
    res.push_back(QVariant::fromValue(m_fromImage.name));
    res.push_back(QVariant::fromValue(m_fromImage.hash));
    res.push_back(QVariant::fromValue(m_fromImage.serverFilesNamespace));
    return res;
}

void QuizImage::setFromImage(const QVariantList &fromImage_)
{
    if(fromImage_.size() < 3) { return; }

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::setFromImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    const QString imageUrl = fromImage_.at(0).toString();
    const QString imageHash = fromImage_.at(1).toString();
    const QString imageNamespace = fromImage_.at(2).toString();

    if(imageUrl == m_fromImage.name || imageHash.isEmpty()) { return; }

    m_fromImage = { imageUrl, imageHash, imageNamespace, false };

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(sf)
    {
        QObject::connect(
                    sf, SIGNAL(downloaded(const QString &)), this,
                    SLOT(imageDownloadedSlot(const QString &)));
        sf->add(m_fromImage.name, m_fromImage.hash, g_imagesRelPath, imageNamespace);
    }

    emit fromImageChanged();
}

QVariantList QuizImage::toImage() const
{
    QVariantList res;
    res.push_back(QVariant::fromValue(m_toImage.name));
    res.push_back(QVariant::fromValue(m_toImage.hash));
    res.push_back(QVariant::fromValue(m_toImage.serverFilesNamespace));
    return res;
}

void QuizImage::setToImage(const QVariantList &toImage_)
{
    if(toImage_.size() < 3) { return; }

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "QuizImage::setFromImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    const QString imageUrl = toImage_.at(0).toString();
    const QString imageHash = toImage_.at(1).toString();
    const QString serverFilesNamespace = toImage_.at(2).toString();

    if(imageUrl == m_toImage.name || imageHash.isEmpty()) { return; }

    m_toImage = { imageUrl, imageHash, serverFilesNamespace, false };

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    if(sf)
    {
        QObject::connect(
                    sf, SIGNAL(downloaded(const QString &)), this,
                    SLOT(imageDownloadedSlot(const QString &)));
        sf->add(m_toImage.name, m_toImage.hash, g_imagesRelPath, serverFilesNamespace);
    }

    emit toImageChanged();
}

QString QuizImage::log() const
{
    return m_compilerLog;
}

void QuizImage::setLog(const QString &log_)
{
    m_compilerLog = log_;

    emit logChanged();
}

QVariant QuizImage::effect() const
{
    return m_effect;
}

void QuizImage::setEffect(const QVariant &effect_)
{
    m_effect = effect_;

    emit effectChanged();
}

QVariant QuizImage::argumentSet() const
{
    return m_argumentSet;
}

void QuizImage::setArgumentSet(const QVariant &argumentSet_)
{
    m_argumentSet = argumentSet_;

    emit argumentSetChanged();
}

int QuizImage::paintedWidth() const
{
    return m_paintedSize.width();
}

int QuizImage::paintedHeight() const
{
    return m_paintedSize.height();
}

void QuizImage::freeProject()
{
    m_drawingData.reset(nullptr);
    m_drawingOldData.reset(nullptr);
}

bool QuizImage::isImageDataUpdated() const
{
    return m_drawingData.operator bool();
}

std::unique_ptr<drawing_data::QuizImageObjects> QuizImage::getData()
{
    return std::move(m_drawingData);
}

void QuizImage::setDataToFree(std::unique_ptr<drawing_data::QuizImageObjects> &&old_)
{
    m_drawingOldData = std::move(old_);
}

void QuizImage::renderBuildError(const QString &compilerLog_)
{
    setLog(compilerLog_);
}

void QuizImage::initDefaultDrawingData()
{
    std::unique_ptr<drawing_data::QuizImageObjects> data(new drawing_data::QuizImageObjects());
    drawing_data::DefaultQuizImageObject defaultData;
    defaultData.initialize(*data.get());
    m_drawingData = std::move(data);
}

void QuizImage::imageDownloadedSlot(const QString &url_)
{
    m_fromImage.loaded = url_ == m_fromImage.name ? true : m_fromImage.loaded;
    m_toImage.loaded = url_ == m_toImage.name ? true : m_toImage.loaded;

    updateStateIfDataIsReady();
}

void QuizImage::updateStateIfDataIsReady()
{
    if(m_fromImage.loaded && m_toImage.loaded)
    {
        updateState();
    }
}

bool QuizImage::isFromImageReady() const
{
    return m_fromImage.loaded;
}

bool QuizImage::isToImageReady() const
{
    return m_toImage.loaded;
}

const QString &QuizImage::getFromImageUrl() const
{
    return m_fromImage.name;
}

const QString &QuizImage::getToImageUrl() const
{
    return m_toImage.name;
}
