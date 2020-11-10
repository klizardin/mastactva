#include "image.h"

Image::Image(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{

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

ImageModel::ImageModel(QObject *parent_ /*= nullptr*/)
    : ListModelBaseOfData<Image, ImageModel>(parent_)
{
    init(this);
}
