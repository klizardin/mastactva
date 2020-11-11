#include "imagepoint.h"
#include <math.h>
#include "utils.h"


ImagePointToNextImage::ImagePointToNextImage(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
}

int ImagePointToNextImage::id() const
{
    return m_id;
}

void ImagePointToNextImage::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImagePointToNextImage::imagePointId() const
{
    return m_imagePointId;
}

void ImagePointToNextImage::setImagePointId(const int &imagePointId_)
{
    m_imagePointId = imagePointId_;

    emit imagePointIdChanged();
}

QString ImagePointToNextImage::nextImage() const
{
    return getNextImage();
}

void ImagePointToNextImage::setNextImage(const QString &nextImage_)
{
    setNextImage(ImageSource(nextImage_));
}

ImageSource ImagePointToNextImage::getNextImage() const
{
    return m_nextImage;
}

void ImagePointToNextImage::setNextImage(const ImageSource &nextImage_)
{
    m_nextImage = nextImage_;
}

ImagePointToNextImageModel::ImagePointToNextImageModel(QObject *parent_ /*= nullptr*/)
    :ListModelBaseOfData<ImagePointToNextImage, ImagePointToNextImageModel>(parent_)
{
    init(this);
}

void ImagePointToNextImageModel::listLoaded(const QJsonDocument &reply_)
{
    base::listLoaded(reply_);
    m_loaded = true;
}

void ImagePointToNextImageModel::clearLoaded()
{
    m_loaded = false;
}

bool ImagePointToNextImageModel::loaded() const
{
    return m_loaded;
}


ImagePoint::ImagePoint(ImagePointModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_imagePointModel = parent_;
}

int ImagePoint::id() const
{
    return m_id;
}

void ImagePoint::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImagePoint::imageId() const
{
    return m_imageId;
}

void ImagePoint::setImageId(const int &imageId_)
{
    m_imageId = imageId_;

    emit imageIdChanged();
}

qreal ImagePoint::xCoord() const
{
    return m_x;
}

void ImagePoint::setXCoord(const qreal &xCoord_)
{
    m_x = xCoord_;

    emit xCoordChanged();
}

qreal ImagePoint::yCoord() const
{
    return m_y;
}

void ImagePoint::setYCoord(const qreal &yCoord_)
{
    m_y = yCoord_;

    emit yCoordChanged();
}

qreal ImagePoint::weight() const
{
    return m_weight;
}

void ImagePoint::setWeight(const qreal &weight_)
{
    m_weight = weight_;

    emit weightChanged();
}

QDateTime ImagePoint::created() const
{
    return m_created;
}

void ImagePoint::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QVariant ImagePoint::nextImage() const
{
    if(nullptr == m_imagePointToNextImage)
    {
        const_cast<ImagePoint *>(this)->m_imagePointToNextImage = const_cast<ImagePoint *>(this)->createImagePointToNextImage();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ImagePointToNextImageModel *>(m_imagePointToNextImage)));
}

void ImagePoint::setNextImage(const QVariant &obj_)
{
    if(obj_.isNull())
    {
        delete m_imagePointToNextImage;
        m_imagePointToNextImage = nullptr;

        emit nextImageChanged();
    }
}

ImagePointToNextImageModel *ImagePoint::getNextImage() const
{
    nextImage();
    return m_imagePointToNextImage;
}

qreal ImagePoint::distanceCoef(qreal x_, qreal y_) const
{
    return sqrt(sqr(m_x - x_) + sqr(m_y - y_)) * m_weight;
}

ImagePointToNextImageModel *ImagePoint::createImagePointToNextImage()
{
    ImagePointToNextImageModel *m = new ImagePointToNextImageModel(this);
    m->initResponse();
    //m->addLayoutExtraGetFieldsImpl("QuizUserModel", QVariant());
    m->setLayoutRefImpl("image_point", m_imagePointModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image_point");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(QString("ImagePointToNextImageModel") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->clearLoaded();
    m->loadList();
    return m;
}

ImagePointModel::ImagePointModel(QObject *parent_ /*= nullptr*/)
    :ListModelBaseOfData<ImagePoint, ImagePointModel>(parent_)
{
    init(this);
}

void ImagePointModel::listLoaded(const QJsonDocument &reply_)
{
    base::listLoaded(reply_);
    for(ImagePoint *ip: m_data)
    {
        if(nullptr == ip) { continue; }
        ip->nextImage();
    }
}

QVariant ImagePointModel::nextImageByCoords(qreal x_, qreal y_)
{
    bool loaded = true;
    ImagePoint *minIp = nullptr;
    qreal mind = 0.0;
    for(ImagePoint *ip: m_data)
    {
        if(nullptr == ip) { continue; }
        ImagePointToNextImageModel *m = ip->getNextImage();
        if(nullptr == m) { continue; }
        qreal d = ip->distanceCoef(x_, y_);
        if(nullptr == minIp || d < mind)
        {
            mind = d;
            minIp = ip;
        }
        loaded &= m->loaded();
    }
    if(!loaded || nullptr == minIp) { return QVariant(); }
    minIp->getNextImage()->setCurrentIndex(0);
    return minIp->getNextImage()->getCurrentItem();
}

