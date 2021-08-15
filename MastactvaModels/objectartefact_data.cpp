#include "objectartefact_data.h"
#include "../MastactvaBase/data_utils.h"


ObjectArtefactData::ObjectArtefactData()
{
    m_artefactData = data_object::utils::createDataVector(static_cast<const ArtefactData *>(nullptr));
}

ObjectArtefactData::ObjectArtefactData(
        int id_,
        int effectId_,
        int artefactId_,
        int stepIndex_,
        ArtefactData *artefact_
        )
    : m_id(id_),
     m_effectId(effectId_),
     m_artefactId(artefactId_),
     m_stepIndex(stepIndex_)
{
    m_artefactData = data_object::utils::createDataVector(static_cast<const ArtefactData *>(nullptr));
    if(m_artefactData.operator bool())
    {
        m_artefactData->push_back(artefact_);
    }
}

ObjectArtefactData::ObjectArtefactData(ObjectArtefactData &&data_)
{
    operator = (std::move(data_));
}

ObjectArtefactData &ObjectArtefactData::operator = (ObjectArtefactData &&data_)
{
    m_id = std::move(data_.m_id);
    m_effectId = std::move(data_.m_effectId);
    m_artefactId = std::move(data_.m_artefactId);
    m_stepIndex = std::move(data_.m_stepIndex);
    m_artefactData = std::move(data_.m_artefactData);

    return *this;
}

std::unique_ptr<ObjectArtefactData> ObjectArtefactData::getDataCopy() const
{
    std::unique_ptr<ObjectArtefactData> result = std::make_unique<ObjectArtefactData>();
    result->m_id = m_id;
    result->m_effectId = m_effectId;
    result->m_artefactId = m_artefactId;
    result->m_stepIndex = m_stepIndex;
    data_object::utils::copyDataVector(m_artefactData.get(), result->m_artefactData.get());
    return result;
}
