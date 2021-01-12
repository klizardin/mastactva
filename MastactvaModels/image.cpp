#include "image.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"


Image::Image(ImageModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_objectModelInfo = this;
    m_imageModel = parent_;
    //qDebug() << "Image::Image()" << getObjectName();
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
    if(nullptr != sf)
    {
        sf->add(getFilename(), hash(), g_imagesRelPath);
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

ImageSource Image::filename() const
{
    return m_filename;
}

void Image::setFilename(const ImageSource &filename_)
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
    if(nullptr == m_imagePoints)
    {
        const_cast<Image *>(this)->m_imagePoints = const_cast<Image *>(this)->createImagePoints();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ImagePointModel *>(m_imagePoints)));
}

void Image::setImagePoints(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_imagePoints)
    {
        delete m_imagePoints;
        m_imagePoints = nullptr;
        emit imagePointsChanged();
    }
}

QVariant Image::imageDescription() const
{
    if(nullptr == m_imageDescriptionModel)
    {
        const_cast<Image *>(this)->m_imageDescriptionModel = const_cast<Image *>(this)->createImageDescriptionModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ImageDescriptionModel *>(m_imageDescriptionModel)));
}

void Image::setImageDescription(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_imageDescriptionModel)
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
    m->addLayoutExtraFieldsImpl("QuizUserModel", QVariant());
    m->setLayoutRefImpl("image", m_imageModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imageModel->getQMLLayoutName() + QString("_Image_") + QVariant::fromValue(m_appId).toString() + QString("_ImagePointModel_"));
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
    m->setLayoutQMLName(m_imageModel->getQMLLayoutName() + QString("_Image_") + QVariant::fromValue(m_appId).toString() + QString("_ImageDescriptionModel_"));
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
    init(this);
    setAutoCreateChildrenModelsOnSelectImpl(true);
}
