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

#include "image.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


Image::Image(ImageModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Image::Image()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "Image::Image()" << this;
#endif

    m_objectModelInfo = this;
    m_imageModel = parent_;
    //qDebug() << "Image::Image()" << getObjectName();
}

Image::~Image()
{
    if(m_imagePointsModel)
    {
        m_imagePointsModel->deleteLater();
    }
    m_imagePointsModel = nullptr;
    if(m_imageDescriptionModel)
    {
        m_imageDescriptionModel->deleteLater();
    }
    m_imageDescriptionModel = nullptr;
}

bool Image::isImageLoaded() const
{
    return IListModelInfoObjectImpl::isListLoadedImpl();
}

void Image::loadChildren()
{
    //qDebug() << "Image::loadChildren()" << getObjectName();
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    m_imageModel->autoLoadDataItemImpl(this);
}

void Image::downloadImage()
{
    //qDebug() << "Image::downloadImage() " << getObjectName() << "file =" << getFilename();
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    QObject::connect(sf, SIGNAL(downloaded(const QString &)),
                     this, SLOT(imageDownloaded(const QString &)));
    if(sf)
    {
        //IListModelInfoObjectImpl::startLoadChildModel();
        sf->add(getFilename(), hash(), g_imagesRelPath);
    }
}

void Image::imageDownloaded(const QString &url_)
{
    if(url_ != getFilename()) { return; }

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    QObject::disconnect(sf, SIGNAL(downloaded(const QString &)),
                        this, SLOT(imageDownloaded(const QString &)));

    if(sf)
    {
        //IListModelInfoObjectImpl::endLoadChildModel();
        if(sf->isUrlDownloaded(getFilename()))
        {
            setLocalImageSource(sf->get(url_));
        }
    }
}

int Image::id() const
{
    return m_id;
}

void Image::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

FileSource Image::filename() const
{
    return m_filename;
}

void Image::setFilename(const FileSource &filename_)
{
    m_filename = filename_;

    emit filenameChanged();
}

QString Image::getFilename() const
{
    return filename();
}

void Image::setFilenameStr(const QString &filename_)
{
    setFilename(filename_);
}

QString Image::localImageSource() const
{
    return m_localImageUrl;
}

void Image::setLocalImageSource(const QString &url_)
{
    m_localImageUrl = url_;

    emit localImageSourceChanged();
}

QString Image::hash() const
{
    return m_hash;
}

void Image::setHash(const QString &hash_)
{
    m_hash = hash_;

    emit hashChanged();
}

int Image::gallery() const
{
    return m_gallery;
}

void Image::setGallery(const int &gallery_)
{
    m_gallery = gallery_;

    emit galleryChanged();
}

bool Image::top() const
{
    return m_top;
}

void Image::setTop(const bool &top_)
{
    m_top = top_;

    emit topChanged();
}

QDateTime Image::created() const
{
    return m_created;
}

void Image::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QVariant Image::imagePoints() const
{
    if(!m_imagePointsModel)
    {
        const_cast<Image *>(this)->m_imagePointsModel = const_cast<Image *>(this)
                ->createImagePoints();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ImagePointModel *>(
                                       m_imagePointsModel)
                                   )
                               );
}

void Image::setImagePoints(const QVariant &obj_)
{
    if(obj_.isNull() && m_imagePointsModel)
    {
        delete m_imagePointsModel;
        m_imagePointsModel = nullptr;
        emit imagePointsChanged();
    }
}

QVariant Image::imageDescription() const
{
    if(!m_imageDescriptionModel)
    {
        const_cast<Image *>(this)->m_imageDescriptionModel = const_cast<Image *>(this)
                ->createImageDescriptionModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ImageDescriptionModel *>(
                                       m_imageDescriptionModel)
                                   )
                               );
}

void Image::setImageDescription(const QVariant &obj_)
{
    if(obj_.isNull() && m_imageDescriptionModel)
    {
        delete m_imageDescriptionModel;
        m_imageDescriptionModel = nullptr;
        emit imageDescriptionChanged();
    }
}

ImagePointModel *Image::createImagePoints()
{
    //qDebug() << "Image::createImagePoints()" << getObjectName();
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    ImagePointModel *m = new ImagePointModel(this);
    m->initResponse();
#if !defined(LOCALDATAAPICACHE)
    m->addLayoutExtraFieldsImpl("QuizUserModel", QVariant());
#endif
    m->setLayoutRefImpl("image", m_imageModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imageModel->getQMLLayoutName() + QString("_Image_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ImagePointModel_"));
    m->registerListModel();
    m->setParentListModelInfo(this);
    m->setImageModel(m_imageModel);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

ImageDescriptionModel *Image::createImageDescriptionModel()
{
    //qDebug() << "Image::createImageDescriptionModel()" << getObjectName();
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    ImageDescriptionModel *m = new ImageDescriptionModel(this);
    m->initResponse();
    m->setLayoutRefImpl("image", m_imageModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imageModel->getQMLLayoutName() + QString("_Image_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ImageDescriptionModel_"));
    m->registerListModel();
    m->setParentListModelInfo(this);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

void Image::startLoadChildModel()
{
    //qDebug() << "Image::startLoadChildModel() " << getObjectName();
    IListModelInfoObjectImpl::startLoadChildModel();
}

void Image::endLoadChildModel()
{
    //qDebug() << "Image::endLoadChildModel() " << getObjectName();
    IListModelInfoObjectImpl::endLoadChildModel();
}

void Image::loadChildrenVF()
{
    //qDebug() << "Image::loadChildrenVF() " << getObjectName() << "file ="  << getFilename();
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void Image::objectLoadedVF()
{
    //qDebug() << "Image::objectLoadedVF() " << getObjectName() << "file ="  << getFilename();
    IListModelInfoObjectImpl::trace();
    downloadImage();
    IListModelInfoObjectImpl::objectLoadedVF();
}

void Image::listLoadedVF()
{
    //qDebug() << "Image::listLoadedVF() " << getObjectName() << "file =" << getFilename();
    IListModelInfoObjectImpl::trace();
    emit imageLoaded();
}

QString Image::getObjectName() const
{
    return m_imageModel->getQMLLayoutName() + QString("_Image_") + QVariant::fromValue(m_appId).toString();
}

ImageModel::ImageModel(QObject *parent_ /*= nullptr*/)
    : ListModelBaseOfData<Image, ImageModel>(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImageModel::ImageModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImageModel::ImageModel()" << this;
#endif

    init(this);
    setAutoCreateChildrenModelsOnSelectImpl(true);
}
