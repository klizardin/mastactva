/*
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

#ifndef EFFECTOBJECTS_DATA_H
#define EFFECTOBJECTS_DATA_H


#include <memory>
#include <QVector>
#include "../MastactvaModels/objectinfo_data.h"
#include "../MastactvaModels/objectartefact_data.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"


struct EffectObjectsData
{
public:
    EffectObjectsData();
    EffectObjectsData(
            int id_,
            int effectId_,
            int objectInfoId_,
            int stepIndex_
            );
    EffectObjectsData(EffectObjectsData &&data_);
    virtual ~EffectObjectsData() = default;

    EffectObjectsData & operator = (EffectObjectsData &&data_);

    // to extract data from model object
    virtual std::unique_ptr<EffectObjectsData> getDataCopy() const;

public:
    int m_id = -1;
    int m_effectId = -1;
    int m_objectInfoId = -1;
    int m_stepIndex = -1;

    std::shared_ptr<QVector<ObjectInfoData *>> m_objectInfoData;
    std::shared_ptr<QVector<ObjectArtefactData *>> m_objectArtefactData;

    EffectObjectsData(const EffectObjectsData &data_) = delete;
    EffectObjectsData & operator = (const EffectObjectsData &data_) = delete;
};


#endif // EFFECTOBJECTS_DATA_H
