#include "objectinfo_data.h"


ObjectInfoData::ObjectInfoData(
        int id_,
        const QString &name_,
        const QString &programmerName_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_name(name_),
      m_programmerName(programmerName_),
      m_created(created_)
{
}

std::unique_ptr<ObjectInfoData> ObjectInfoData::getDataCopy() const
{
    std::unique_ptr<ObjectInfoData> result = std::make_unique<ObjectInfoData>();
    result->m_id = m_id;
    result->m_name = m_name;
    result->m_programmerName = m_programmerName;
    result->m_description = m_description;
    result->m_created = m_created;
    return result;
}
