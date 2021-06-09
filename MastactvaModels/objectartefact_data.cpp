#include "objectartefact_data.h"


inline
std::shared_ptr<QVector<ArtefactData *>> createArtefactData()
{
    return std::shared_ptr<QVector<ArtefactData *>>(
                new QVector<ArtefactData *>(),
                [](QVector<ArtefactData *> *ptr_)->void
        {
            if(nullptr == ptr_)
            {
                return;
            }
            for(ArtefactData *& p_: *ptr_)
            {
                delete p_;
                p_ = nullptr;
            }
            ptr_->clear();
            delete ptr_;
        });
}


ObjectArtefactData::ObjectArtefactData()
{
    m_artefactData = createArtefactData();
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
    m_artefactData = createArtefactData();
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
