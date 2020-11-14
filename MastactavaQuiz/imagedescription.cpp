#include "imagedescription.h"

ImageDescription::ImageDescription(QObject *parent_ /* = nullptr*/)
    : QObject(parent_)
{
}

int ImageDescription::id() const
{
    return m_id;
}

void ImageDescription::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImageDescription::image() const
{
    return m_imageId;
}

void ImageDescription::setImage(const int &imageId_)
{
    m_imageId = imageId_;

    emit imageChanged();
}

QDateTime ImageDescription::fromDate() const
{
    return m_fromDate;
}

void ImageDescription::setFromDate(const QDateTime &fromDate_)
{
    m_fromDate = fromDate_;

    emit fromDateChanged();
}

QString ImageDescription::description() const
{
    return m_descriptionText;
}

void ImageDescription::setDescription(const QString &descriptionText_)
{
    m_descriptionText = descriptionText_;

    emit descriptionChanged();
}

ImageDescriptionModel::ImageDescriptionModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
