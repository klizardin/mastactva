#include "effectargument_data.h"


EffectArgumentData::EffectArgumentData(
        int argStorageId_,
        int argTypeId_,
        const QString &name_,
        const QString &defaultValue_
        )
    : m_argStorageId(argStorageId_),
      m_argTypeId(argTypeId_),
      m_name(name_),
      m_defaultValue(defaultValue_)
{
}

EffectArgumentData::EffectArgumentData(EffectArgumentData &&data_)
{
    operator=(std::move(data_));
}

EffectArgumentData & EffectArgumentData::operator = (EffectArgumentData &&data_)
{
    m_argTypeId = std::move(data_.m_argTypeId);
    m_argStorageId = std::move(data_.m_argStorageId);
    m_name = std::move(data_.m_name);
    m_defaultValue = std::move(data_.m_defaultValue);

    return *this;
}

std::unique_ptr<EffectArgumentData> EffectArgumentData::getDataCopy() const
{
    std::unique_ptr<EffectArgumentData> result = std::make_unique<EffectArgumentData>();

    result->m_argTypeId = m_argTypeId;
    result->m_argStorageId = m_argStorageId;
    result->m_name = m_name;
    result->m_defaultValue = m_defaultValue;

    return result;
}
