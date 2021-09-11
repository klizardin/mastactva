#include "effectargset_data.h"
#include "../MastactvaBase/data_utils.h"


EffectArgSetData::EffectArgSetData()
{
    m_effectArgValuesData = ::data_object::utils::createDataVector(
                static_cast<const EffectArgValueData *>(nullptr)
                );
}

EffectArgSetData::EffectArgSetData(
        int id_,
        int effectId_,
        int easingTypeId_,
        const QString &description_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_effectId(effectId_),
      m_easingTypeId(easingTypeId_),
      m_description(description_),
      m_created(created_)
{
    m_effectArgValuesData = ::data_object::utils::createDataVector(
                static_cast<const EffectArgValueData *>(nullptr)
                );
}

EffectArgSetData::EffectArgSetData(EffectArgSetData &&data_)
{
    operator=(std::move(data_));
}

EffectArgSetData &EffectArgSetData::operator=(EffectArgSetData &&data_)
{
    m_id = std::move(data_.m_id);
    m_effectId = std::move(data_.m_effectId);
    m_easingTypeId = std::move(data_.m_easingTypeId);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_effectArgValuesData = std::move(data_.m_effectArgValuesData);

    return *this;
}

std::unique_ptr<EffectArgSetData> EffectArgSetData::getDataCopy() const
{
    std::unique_ptr<EffectArgSetData> result = std::make_unique<EffectArgSetData>();
    result->m_id = m_id;
    result->m_effectId = m_effectId;
    result->m_easingTypeId = m_easingTypeId;
    result->m_description = m_description;
    result->m_created = m_created;
    ::data_object::utils::copyDataVector(m_effectArgValuesData.get(), result->m_effectArgValuesData.get());
    return result;
}
