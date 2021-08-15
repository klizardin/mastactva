#include "artefact_data.h"
#include "../MastactvaBase/data_utils.h"
#include "../MastactvaBase/utils.h"


ArtefactData::ArtefactData()
{
    m_artefactArgData = data_object::utils::createDataVector(static_cast<const ArtefactArgData *>(nullptr));
}

ArtefactData::ArtefactData(
        int id_,
        const QString &name_,
        const FileSource &filename_,
        const QString &hash_,
        ArtefactTypeEn typeId_,
        const QString &description_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_name(name_),
      m_filename(filename_),
      m_hash(hash_),
      m_typeId(to_underlying(typeId_)),
      m_description(description_),
      m_created(created_)
{
    m_artefactArgData = data_object::utils::createDataVector(static_cast<const ArtefactArgData *>(nullptr));
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

std::unique_ptr<ArtefactData> ArtefactData::getDataCopy() const
{
    std::unique_ptr<ArtefactData> result = std::make_unique<ArtefactData>();
    result->m_id = m_id;
    result->m_name = m_name;
    result->m_filename = m_filename;
    result->m_hash = m_hash;
    result->m_typeId = m_typeId;
    result->m_description = m_description;
    result->m_created = m_created;
    data_object::utils::copyDataVector(m_artefactArgData.get(), result->m_artefactArgData.get());
    return result;
}
