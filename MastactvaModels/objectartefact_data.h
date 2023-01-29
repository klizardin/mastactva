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


/*
 * structure of object's artefact data
 * (data holder)
*/
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
    int m_objectInfoId = -1;        // object info id reference
    int m_artefactId = -1;          // artefact id
    int m_stepIndex = -1;           // step index
    MergeId m_mergeid;              // unique id to merge
    QVector<QString> m_textures;    // names of textures to render object's step into them
                                    // if it is empty then render to the main texture

    std::shared_ptr<QVector<ArtefactData *>> m_artefactData;    // an artefact 1:1

    ObjectArtefactData(const ObjectArtefactData &) = delete;
    ObjectArtefactData & operator = (const ObjectArtefactData &) = delete;
};


#endif // OBJECTARTEFACT_DATA_H
