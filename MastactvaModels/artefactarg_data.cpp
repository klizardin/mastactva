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

ArtefactArgData::ArtefactArgData(ArtefactArgData &&data_)
{
    operator=(std::move(data_));
}

ArtefactArgData & ArtefactArgData::operator = (ArtefactArgData && data_)
{
    m_id = std::move(data_.m_id);
    m_artefactId = std::move(data_.m_artefactId);
    m_argTypeId = std::move(data_.m_argTypeId);
    m_argStorageId = std::move(data_.m_argStorageId);
    m_name = std::move(data_.m_name);
    m_defaultValue = std::move(data_.m_defaultValue);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);

    return *this;
}

std::unique_ptr<ArtefactArgData> ArtefactArgData::getDataCopy() const
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
