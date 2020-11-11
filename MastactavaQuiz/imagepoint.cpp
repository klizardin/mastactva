#include "imagepoint.h"

ImagePoint::ImagePoint(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
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

ImagePointModel::ImagePointModel(QObject *parent_ /*= nullptr*/)
    :ListModelBaseOfData<ImagePoint, ImagePointModel>(parent_)
{
    init(this);
}

QVariant ImagePointModel::nextImageByCoords(qreal x_, qreal y_)
{
    return QVariant();
}
