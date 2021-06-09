#include "object_tests.h"
#include "drawingdata_effectobjects.h"


void MapFileSource::add(const QString &filename_, const QString &text_)
{
    m_texts.insert(filename_, text_);
}

void MapFileSource::add(const QString &filename_, const QImage &image_)
{
    m_images.insert(filename_, image_);
}

QString MapFileSource::getText(const FileSource &filename_) const
{
    return m_texts.value(filename_, QString());
}

QImage MapFileSource::getImage(const FileSource &filename_) const
{
    return m_images.value(filename_, QImage());
}

namespace drawing_objects
{

void BaseTest::initialize(drawing_data::QuizImageObjects &data_) const
{
    std::shared_ptr<MapFileSource> filesource = std::make_shared<MapFileSource>();
    ::EffectObjectsData effectObjectData;
    ::DrawingDataEffectObjects drawingDataEffectObjects(std::move(effectObjectData));
    drawingDataEffectObjects.init(filesource);
    drawingDataEffectObjects.initialize(data_);
}

}
