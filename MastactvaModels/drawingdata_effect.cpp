#include "drawingdata_effect.h"
#include <map>
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_effectobjects.h"


DrawingDataEffect::DrawingDataEffect(EffectData &&data_)
    : EffectData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectObjectsData, static_cast<DrawingDataEffectObjects *>(nullptr));
}

void DrawingDataEffect::init(std::shared_ptr<drawingdata::IFileSource> filesources_)
{
    m_details.filesource = std::move(filesources_);
}

void DrawingDataEffect::initialize(drawing_data::QuizImageObjects &data_) const
{
    if(!m_effectObjectsData.operator bool()
            || !m_details.filesource.operator bool())
    {
        return;
    }
    const_cast<DrawingDataEffect *>(this)->m_details.clear();
    using SortedEffectObjects = std::multimap<int, const DrawingDataEffectObjects *>;
    SortedEffectObjects sortedMainEffectObjects;
    SortedEffectObjects sortedEffectObjects;
    for(const EffectObjectsData *effectObject_ : *m_effectObjectsData)
    {
        const DrawingDataEffectObjects *effectObject = dynamic_cast<const DrawingDataEffectObjects *>(effectObject_);
        if(!effectObject)
        {
            continue;
        }
        if(effectObject->isMain())
        {
            sortedMainEffectObjects.insert({effectObject->getStepIndex(), effectObject});
        }
        else
        {
            sortedEffectObjects.insert({effectObject->getStepIndex(), effectObject});
        }
    }
    for(const SortedEffectObjects::value_type &v_ : sortedMainEffectObjects)
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addObjects(data_, m_details);
    }
    for(const SortedEffectObjects::value_type &v_ : sortedEffectObjects)
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addObjects(data_, m_details);
    }
}
