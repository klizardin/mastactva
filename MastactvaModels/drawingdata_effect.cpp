#include "drawingdata_effect.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_effectobjects.h"


DrawingDataEffect::DrawingDataEffect(EffectData &&data_)
    : EffectData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectObjectsData, static_cast<DrawingDataEffectObjects *>(nullptr));
}

EffectData * DrawingDataEffect::getData()
{
    return this;
}

void DrawingDataEffect::init(std::shared_ptr<drawingdata::IFileSource> filesources_)
{
    m_filesources = std::move(filesources_);
}

void DrawingDataEffect::initialize(drawing_data::QuizImageObjects &data_) const
{
    Q_UNUSED(data_);
}
