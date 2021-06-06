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

std::unique_ptr<ObjectArtefactData> ObjectArtefactData::copy() const
{
    std::unique_ptr<ObjectArtefactData> result = std::make_unique<ObjectArtefactData>();
    result->m_id = m_id;
    result->m_effectId = m_effectId;
    result->m_artefactId = m_artefactId;
    result->m_stepIndex = m_stepIndex;
    Q_ASSERT(result->m_artefactData.operator bool());
    if(m_artefactData.operator bool())
    {
        for(const ArtefactData * elem_ : *m_artefactData)
        {
            if(nullptr == elem_)
            {
                continue;
            }
            result->m_artefactData->push_back(elem_->copy().release());
        }
    }
    return result;
}
