#include "objectinfo_data.h"


std::unique_ptr<ObjectInfoData> ObjectInfoData::copy() const
{
    std::unique_ptr<ObjectInfoData> result = std::make_unique<ObjectInfoData>();
    result->m_id = m_id;
    result->m_name = m_name;
    result->m_programmerName = m_programmerName;
    result->m_description = m_description;
    result->m_created = m_created;
    return result;
}
