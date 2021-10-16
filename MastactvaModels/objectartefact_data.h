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

#ifndef OBJECTARTEFACT_DATA_H
#define OBJECTARTEFACT_DATA_H


#include <memory>
#include <QVector>
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaModels/artefact_data.h"


struct ObjectArtefactData
{
public:
    ObjectArtefactData();
    ObjectArtefactData(
            int id_,
            int effectId_,
            int artefactId_,
            int stepIndex_,
            ArtefactData *artefact_,
            const MergeId &mergeid_
            );
    ObjectArtefactData(ObjectArtefactData &&data_);
    virtual ~ObjectArtefactData() = default;

    ObjectArtefactData &operator = (ObjectArtefactData &&data_);
    virtual std::unique_ptr<ObjectArtefactData> getDataCopy() const;

private:
    void createArrays();

public:
    int m_id = -1;
    int m_objectInfoId = -1;
    int m_artefactId = -1;
    int m_stepIndex = -1;
    MergeId m_mergeid;

    std::shared_ptr<QVector<ArtefactData *>> m_artefactData;

    ObjectArtefactData(const ObjectArtefactData &) = delete;
    ObjectArtefactData & operator = (const ObjectArtefactData &) = delete;
};


#endif // OBJECTARTEFACT_DATA_H
