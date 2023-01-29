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

#include "objectartefact_data.h"
#include "../MastactvaBase/data_utils.h"


ObjectArtefactData::ObjectArtefactData()
{
    createArrays();
}

ObjectArtefactData::ObjectArtefactData(
        int id_,
        int objectInfoId_,
        int artefactId_,
        int stepIndex_,
        ArtefactData *artefact_,
        const MergeId &mergeid_,
        const QStringList &textures_
        )
    : m_id(id_),
     m_objectInfoId(objectInfoId_),
     m_artefactId(artefactId_),
     m_stepIndex(stepIndex_),
     m_mergeid(mergeid_),
     m_textures(textures_)
{
    createArrays();
    if(m_artefactData.operator bool())
    {
        m_artefactData->push_back(artefact_);
    }
}

ObjectArtefactData::ObjectArtefactData(ObjectArtefactData &&data_)
{
    operator = (std::move(data_));
}

ObjectArtefactData &ObjectArtefactData::operator = (ObjectArtefactData &&data_)
{
    m_id = std::move(data_.m_id);
    m_objectInfoId = std::move(data_.m_objectInfoId);
    m_artefactId = std::move(data_.m_artefactId);
    m_stepIndex = std::move(data_.m_stepIndex);
    m_artefactData = std::move(data_.m_artefactData);
    m_mergeid = std::move(data_.m_mergeid);
    m_textures = std::move(data_.m_textures);

    return *this;
}

std::unique_ptr<ObjectArtefactData> ObjectArtefactData::getDataCopy() const
{
    std::unique_ptr<ObjectArtefactData> result = std::make_unique<ObjectArtefactData>();
    result->m_id = m_id;
    result->m_objectInfoId = m_objectInfoId;
    result->m_artefactId = m_artefactId;
    result->m_stepIndex = m_stepIndex;
    result->m_mergeid = m_mergeid;
    result->m_textures = m_textures;
    data_object::utils::copyDataVector(m_artefactData.get(), result->m_artefactData.get());
    return result;
}

void ObjectArtefactData::createArrays()
{
    m_artefactData = data_object::utils::createDataVector(static_cast<const ArtefactData *>(nullptr));
}
