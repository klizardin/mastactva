#include "image.h"

Image::Image(ImageModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_imageModel = parent_;
    m_thisListModelInfo = this;
}

bool Image::isImageLoaded() const
{
    Q_ASSERT(m_imageModel->autoCreateChildrenModels());
    return isListLoadedImpl();
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
    if(obj_.isNull())
    {
        delete m_imagePoints;
        m_imagePoints = nullptr;
        emit imagePointsChanged();
    }
}

ImagePointModel *Image::createImagePoints()
{
    ImagePointModel *m = new ImagePointModel(this);
    m->initResponse();
    m->addLayoutExtraGetFieldsImpl("QuizUserModel", QVariant());
    m->setLayoutRefImpl("image", m_imageModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imageModel->getQMLLayoutName() + QString("_ImagePointModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


ImageModel::ImageModel(QObject *parent_ /*= nullptr*/)
    : ListModelBaseOfData<Image, ImageModel>(parent_)
{
    init(this);
}
