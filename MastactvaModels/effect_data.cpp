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

#include "effect_data.h"
#include "../MastactvaBase/data_utils.h"


EffectData::EffectData()
{
    createArrays();
}

EffectData::EffectData(
        int id_,
        const QString &name_,
        const QString &description_,
        const QDateTime &created_,
        const MergeId &mergeid_
        )
    : m_id(id_),
      m_name(name_),
      m_description(description_),
      m_created(created_),
      m_mergeid(mergeid_)
{
    createArrays();
}

EffectData::EffectData(EffectData &&data_)
{
    operator = (std::move(data_));
}

EffectData &EffectData::operator = (EffectData &&data_)
{
    m_id = std::move(data_.m_id);
    m_name = std::move(data_.m_name);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_mergeid = std::move(data_.m_mergeid);
    m_effectObjectsData = std::move(data_.m_effectObjectsData);
    m_effectArgsData = std::move(data_.m_effectArgsData);
    m_effectArgSetsData = std::move(data_.m_effectArgSetsData);
    return *this;
}

std::unique_ptr<EffectData> EffectData::getDataCopy() const
{
    std::unique_ptr<EffectData> result = std::make_unique<EffectData>();
    result->m_id = m_id;
    result->m_name = m_name;
    result->m_description = m_description;
    result->m_created = m_created;
    result->m_mergeid = m_mergeid;
    ::data_object::utils::copyDataVector(m_effectObjectsData.get(), result->m_effectObjectsData.get());
    ::data_object::utils::copyDataVector(m_effectArgsData.get(), result->m_effectArgsData.get());
    ::data_object::utils::copyDataVector(m_effectArgSetsData.get(), result->m_effectArgSetsData.get());
    return result;
}

void EffectData::createArrays()
{
    m_effectObjectsData = ::data_object::utils::createDataVector(static_cast<const EffectObjectsData *>(nullptr));
    m_effectArgsData = ::data_object::utils::createDataVector(static_cast<const EffectArgData *>(nullptr));
    m_effectArgSetsData = ::data_object::utils::createDataVector(static_cast<const EffectArgSetData *>(nullptr));
}
