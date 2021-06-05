#include "artefact_data.h"


ArtefactData::ArtefactData()
{
    m_artefactArgData = std::make_shared<QVector<ArtefactArgData *>>();
}

ArtefactData::ArtefactData(ArtefactData &&data_)
{
    operator = (std::move(data_));
}

ArtefactData &ArtefactData::operator = (ArtefactData &&data_)
{
    m_id = std::move(data_.m_id);
    m_name = std::move(data_.m_name);
    m_filename = std::move(data_.m_filename);
    m_hash = std::move(data_.m_hash);
    m_typeId = std::move(data_.m_typeId);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_artefactArgData = std::move(data_.m_artefactArgData);

    return *this;
}
