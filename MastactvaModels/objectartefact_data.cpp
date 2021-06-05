#include "objectartefact_data.h"


ObjectArtefactData::ObjectArtefactData()
{
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
