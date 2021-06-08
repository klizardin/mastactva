#include "effectobjects_data.h"


EffectObjectsData::EffectObjectsData()
{
    m_objectInfoData = std::shared_ptr<QVector<ObjectInfoData *>>(
            new QVector<ObjectInfoData *>(),
            [](QVector<ObjectInfoData *> *ptr_)->void
    {
        if(nullptr == ptr_)
        {
            return;
        }
        for(ObjectInfoData *& p_: *ptr_)
        {
            delete p_;
            p_ = nullptr;
        }
        ptr_->clear();
    });
    m_objectArtefactData = std::shared_ptr<QVector<ObjectArtefactData *>>(
            new QVector<ObjectArtefactData *>(),
            [](QVector<ObjectArtefactData *> *ptr_)->void
    {
        if(nullptr == ptr_)
        {
            return;
        }
        for(ObjectArtefactData *& p_: *ptr_)
        {
            delete p_;
            p_ = nullptr;
        }
        ptr_->clear();
    });
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

std::unique_ptr<EffectObjectsData> EffectObjectsData::copy() const
{
    std::unique_ptr<EffectObjectsData> result = std::make_unique<EffectObjectsData>();
    result->m_id = m_id;
    result->m_effectId = m_effectId;
    result->m_objectInfoId = m_objectInfoId;
    result->m_stepIndex = m_stepIndex;
    Q_ASSERT(result->m_objectInfoData.operator bool());
    Q_ASSERT(result->m_objectArtefactData.operator bool());
    if(m_objectInfoData.operator bool())
    {
        for(const ObjectInfoData *elem_ : *m_objectInfoData)
        {
            if(nullptr == elem_)
            {
                continue;
            }
            result->m_objectInfoData->push_back(elem_->copy().release());
        }
    }
    if(m_objectArtefactData.operator bool())
    {
        for(const ObjectArtefactData *elem_ : *m_objectArtefactData)
        {
            if(nullptr == elem_)
            {
                continue;
            }
            result->m_objectArtefactData->push_back(elem_->copy().release());
        }
    }
    return result;
}
