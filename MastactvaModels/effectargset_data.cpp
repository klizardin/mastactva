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

std::unique_ptr<EffectArgSetData> EffectArgSetData::getDataCopy() const
{
    std::unique_ptr<EffectArgSetData> result = std::make_unique<EffectArgSetData>();
    return result;
}
