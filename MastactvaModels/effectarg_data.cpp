#include "effectarg_data.h"


EffectArgData::EffectArgData(int id_,
              int effectId_,
              int objectArtefactId_,
              int argTypeId_,
              int argStorageId_,
              const QString &name_,
              const QString &defaultValue_,
              const QString &description_,
              const QDateTime &created_
              )
    :m_id(id_),
     m_effectId(effectId_),
     m_objectArtefactId(objectArtefactId_),
     m_argTypeId(argTypeId_),
     m_argStorageId(argStorageId_),
     m_name(name_),
     m_defaultValue(defaultValue_),
     m_description(description_),
     m_created(created_)
{
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
    return result;
}
