#include "effectarg_data.h"


EffectArgData::EffectArgData(int id_,
              int effectId_,
              int objectArtefactId_,
              int argTypeId_,
              int argStorageId_,
              const QString &name_,
              const QString &defaultValue_,
              const QString &description_,
              const QDateTime &created_,
              const QString &mergeid_
              )
    : EffectArgumentData(
          argStorageId_,
          argTypeId_,
          name_,
          defaultValue_
          ),
      m_id(id_),
      m_effectId(effectId_),
      m_objectArtefactId(objectArtefactId_),
      m_description(description_),
      m_created(created_),
      m_mergeid(mergeid_)
{
}

EffectArgData::EffectArgData(EffectArgData &&data_)
{
    operator = (std::move(data_));
}

EffectArgData &EffectArgData::operator = (EffectArgData &&data_)
{
    m_id = std::move(data_.m_id);
    m_effectId = std::move(data_.m_effectId);
    m_objectArtefactId = std::move(data_.m_objectArtefactId);
    m_argTypeId = std::move(data_.m_argTypeId);
    m_argStorageId = std::move(data_.m_argStorageId);
    m_name = std::move(data_.m_name);
    m_defaultValue = std::move(data_.m_defaultValue);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_mergeid = std::move(data_.m_mergeid);

    return *this;
}

std::unique_ptr<EffectArgData> EffectArgData::getDataCopy() const
{
    std::unique_ptr<EffectArgData> result = std::make_unique<EffectArgData>();
    result->m_id = m_id;
    result->m_effectId = m_effectId;
    result->m_objectArtefactId = m_objectArtefactId;
    result->m_argTypeId = m_argTypeId;
    result->m_argStorageId = m_argStorageId;
    result->m_name = m_name;
    result->m_defaultValue = m_defaultValue;
    result->m_description = m_description;
    result->m_created = m_created;
    result->m_mergeid = m_mergeid;
    return result;
}
