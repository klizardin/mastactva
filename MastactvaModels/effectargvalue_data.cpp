#include "effectargvalue_data.h"
#include "../MastactvaBase/data_utils.h"


EffectArgValueData::EffectArgValueData()
{
    m_effectArgsData = ::data_object::utils::createDataVector(static_cast<const EffectArgData *>(nullptr));
}

EffectArgValueData::EffectArgValueData(
        int id_,
        int argSetId_,
        int argId_,
        const QString &value_,
        const QString &description_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_argSetId(argSetId_),
      m_argId(argId_),
      m_value(value_),
      m_description(description_),
      m_created(created_)
{
    m_effectArgsData = ::data_object::utils::createDataVector(static_cast<const EffectArgData *>(nullptr));
}

EffectArgValueData::EffectArgValueData(EffectArgValueData &&data_)
{
    operator=(std::move(data_));
}

EffectArgValueData &EffectArgValueData::operator=(EffectArgValueData &&data_)
{
    m_id = std::move(data_.m_id);
    m_argSetId = std::move(data_.m_argSetId);
    m_argId = std::move(data_.m_argId);
    m_value = std::move(data_.m_value);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_effectArgsData = std::move(data_.m_effectArgsData);
    return *this;
}

std::unique_ptr<EffectArgValueData> EffectArgValueData::getDataCopy() const
{
    std::unique_ptr<EffectArgValueData> result = std::make_unique<EffectArgValueData>();
    result->m_id = m_id;
    result->m_argSetId = m_argSetId;
    result->m_argId = m_argId;
    result->m_value = m_value;
    result->m_description = m_description;
    result->m_created = m_created;
    ::data_object::utils::copyDataVector(m_effectArgsData.get(), result->m_effectArgsData.get());
    return result;
}
