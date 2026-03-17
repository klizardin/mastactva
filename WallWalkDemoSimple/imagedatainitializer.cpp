#include "imagedatainitializer.h"


bool MapFileSource::containsText(const QString &filename_) const
{
    return m_texts.contains(filename_);
}

bool MapFileSource::containsImage(const QString &filename_) const
{
    return m_images.contains(filename_);
}

bool MapFileSource::load(const QString &filename_)
{
    if(containsImage(filename_))
    {
        return true;
    }
    QImage image;
    if(image.load(filename_))
    {
        addImage(filename_, image);
        return true;
    }
    return false;
}

void MapFileSource::addText(const QString &filename_, const QString &text_)
{
    m_texts.insert(filename_, text_);
}

void MapFileSource::addImage(const QString &filename_, const QImage &image_)
{
    m_images.insert(filename_, image_);
}

QString MapFileSource::getText(const FileSource &filename_) const
{
    return m_texts.value(filename_, QString{});
}

QImage MapFileSource::getImage(const FileSource &filename_) const
{
    return m_images.value(filename_, QImage{});
}


ImageDataInitializer::ImageDataInitializer()
{

}
