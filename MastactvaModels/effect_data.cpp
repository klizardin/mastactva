#include "effect_data.h"
#include "../MastactvaBase/data_utils.h"


EffectData::EffectData()
{
    m_effectObjectsData = ::data_object::utils::createDataVector(static_cast<const EffectObjectsData *>(nullptr));
}

EffectData::EffectData(
        int id_,
        const QString &name_,
        const QString &description_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_name(name_),
      m_description(description_),
      m_created(created_)
{
    m_effectObjectsData = ::data_object::utils::createDataVector(static_cast<const EffectObjectsData *>(nullptr));
}

EffectData::EffectData(EffectData &&data_)
{
    operator = (std::move(data_));
}

EffectData &EffectData::operator = (EffectData &&data_)
{
    m_id = std::move(data_.m_id);
    m_name = std::move(data_.m_name);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_effectObjectsData = std::move(data_.m_effectObjectsData);
    return *this;
}

std::unique_ptr<EffectData> EffectData::getDataCopy() const
{
    std::unique_ptr<EffectData> result = std::make_unique<EffectData>();
    result->m_id = m_id;
    result->m_name = m_name;
    result->m_description = m_description;
    result->m_created = m_created;
    ::data_object::utils::copyDataVector(m_effectObjectsData.get(), result->m_effectObjectsData.get());
    return result;
}


