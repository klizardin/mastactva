#include "effectobjects_data.h"


EffectObjectsData::EffectObjectsData()
{
    m_objectInfoData = std::make_shared<QVector<ObjectInfoData *>>();
    m_objectArtefactData = std::make_shared<QVector<ObjectArtefactData *>>();
}

EffectObjectsData::EffectObjectsData(EffectObjectsData &&data_)
{
    operator=(std::move(data_));
}

EffectObjectsData & EffectObjectsData::operator = (EffectObjectsData &&data_)
{
    m_id = std::move(data_.m_id);
    m_effectId = std::move(data_.m_effectId);
    m_objectInfoId = std::move(data_.m_objectInfoId);
    m_stepIndex = std::move(data_.m_stepIndex);
    m_objectInfoData = std::move(data_.m_objectInfoData);
    m_objectArtefactData = std::move(data_.m_objectArtefactData);
    return *this;
}

