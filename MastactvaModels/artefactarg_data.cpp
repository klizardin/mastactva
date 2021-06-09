#include "artefactarg_data.h"
#include "../MastactvaBase/utils.h"


ArtefactArgData::ArtefactArgData(
        int id_,
        int artefactId_,
        ArtefactArgTypeEn argTypeId_,
        ArtefactArgStorageEn argStorageId_,
        const QString &name_,
        const QString &defaultValue_,
        const QString &description_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_artefactId(artefactId_),
      m_argTypeId(to_underlying(argTypeId_)),
      m_argStorageId(to_underlying(argStorageId_)),
      m_name(name_),
      m_defaultValue(defaultValue_),
      m_description(description_),
      m_created(created_)
{
}

std::unique_ptr<ArtefactArgData> ArtefactArgData::copy() const
{
    std::unique_ptr<ArtefactArgData> result = std::make_unique<ArtefactArgData>();
    result->m_id = m_id;
    result->m_artefactId = m_artefactId;
    result->m_argTypeId = m_argTypeId;
    result->m_argStorageId = m_argStorageId;
    result->m_name = m_name;
    result->m_defaultValue = m_defaultValue;
    result->m_description = m_description;
    result->m_created = m_created;
    return result;
}
