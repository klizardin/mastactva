#include "artefactarg_data.h"


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
