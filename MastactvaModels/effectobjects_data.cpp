#include "effectobjects_data.h"
#include <map>
#include "../MastactvaModels/objectartefact_data.h"
#include "../MastactvaBase/data_utils.h"


EffectObjectsData::EffectObjectsData()
{
    m_objectInfoData = data_object::utils::createDataVector(static_cast<const ObjectInfoData *>(nullptr));
    m_objectArtefactData = data_object::utils::createDataVector(static_cast<const ObjectArtefactData *>(nullptr));
}

EffectObjectsData::EffectObjectsData(
        int id_,
        int effectId_,
        int objectInfoId_,
        int stepIndex_
        )
    : m_id(id_),
      m_effectId(effectId_),
      m_objectInfoId(objectInfoId_),
      m_stepIndex(stepIndex_)
{
    m_objectInfoData = data_object::utils::createDataVector(static_cast<const ObjectInfoData *>(nullptr));
    m_objectArtefactData = data_object::utils::createDataVector(static_cast<const ObjectArtefactData *>(nullptr));
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

std::unique_ptr<EffectObjectsData> EffectObjectsData::getDataCopy() const
{
    std::unique_ptr<EffectObjectsData> result = std::make_unique<EffectObjectsData>();
    result->m_id = m_id;
    result->m_effectId = m_effectId;
    result->m_objectInfoId = m_objectInfoId;
    result->m_stepIndex = m_stepIndex;
    data_object::utils::copyDataVector(m_objectInfoData.get(), result->m_objectInfoData.get());
    data_object::utils::copyDataVector(m_objectArtefactData.get(), result->m_objectArtefactData.get());
    return result;
}
