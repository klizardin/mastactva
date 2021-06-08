#include "artefact_data.h"


ArtefactData::ArtefactData()
{
    m_artefactArgData = std::shared_ptr<QVector<ArtefactArgData *>>(
            new QVector<ArtefactArgData *>(),
            [](QVector<ArtefactArgData *> *ptr_)->void
    {
        if(nullptr == ptr_)
        {
            return;
        }
        for(ArtefactArgData *& p_: *ptr_)
        {
            delete p_;
            p_ = nullptr;
        }
        ptr_->clear();
    });
}

ArtefactData::ArtefactData(ArtefactData &&data_)
{
    operator = (std::move(data_));
}

ArtefactData &ArtefactData::operator = (ArtefactData &&data_)
{
    m_id = std::move(data_.m_id);
    m_name = std::move(data_.m_name);
    m_filename = std::move(data_.m_filename);
    m_hash = std::move(data_.m_hash);
    m_typeId = std::move(data_.m_typeId);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_artefactArgData = std::move(data_.m_artefactArgData);

    return *this;
}

std::unique_ptr<ArtefactData> ArtefactData::copy() const
{
    std::unique_ptr<ArtefactData> result = std::make_unique<ArtefactData>();
    result->m_id = m_id;
    result->m_name = m_name;
    result->m_filename = m_filename;
    result->m_hash = m_hash;
    result->m_typeId = m_typeId;
    result->m_description = m_description;
    result->m_created = m_created;
    Q_ASSERT(result->m_artefactArgData.operator bool());
    if(m_artefactArgData.operator bool())
    {
        for(const ArtefactArgData *elem_: *m_artefactArgData)
        {
            if(nullptr == elem_)
            {
                continue;
            }
            result->m_artefactArgData->push_back(elem_->copy().release());
        }
    }
    return result;
}
