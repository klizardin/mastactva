#include "drawingdata_effect.h"
#include <map>
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
    if(!m_effectObjectsData.operator bool()
            || !m_filesources.operator bool())
    {
        return;
    }
    using SortedEffectObjects = std::multimap<int, const EffectObjectsData *>;
    SortedEffectObjects sortedEffectObjects;
    for(const EffectObjectsData *effectObject_ : *m_effectObjectsData)
    {
        if(!effectObject_)
        {
            continue;
        }
        sortedEffectObjects.insert({effectObject_->m_stepIndex, effectObject_});
    }
    for(const SortedEffectObjects::value_type &v_ : sortedEffectObjects)
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addObjects(data_, m_filesources.get());
    }
}
