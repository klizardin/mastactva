/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "artefact_data.h"
#include "../MastactvaBase/data_utils.h"
#include "../MastactvaBase/utils.h"


ArtefactData::ArtefactData()
{
    createArrays();
}

ArtefactData::ArtefactData(
        int id_,
        const QString &name_,
        const FileSource &filename_,
        const QString &hash_,
        ArtefactTypeEn typeId_,
        const QString &description_,
        const QDateTime &created_,
        const MergeId &mergeid_
        )
    : m_id(id_),
      m_name(name_),
      m_filename(filename_),
      m_hash(hash_),
      m_typeId(to_underlying(typeId_)),
      m_description(description_),
      m_created(created_),
      m_mergeid(mergeid_)
{
    createArrays();
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
    m_mergeid = std::move(data_.m_mergeid);
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
    result->m_mergeid = m_mergeid;
    data_object::utils::copyDataVector(m_artefactArgData.get(), result->m_artefactArgData.get());
    return result;
}

void ArtefactData::createArrays()
{
    m_artefactArgData = data_object::utils::createDataVector(static_cast<const ArtefactArgData *>(nullptr));
}
