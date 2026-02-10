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

#ifndef EFFECTARGSET_DATA_H
#define EFFECTARGSET_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include <QVector>
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaModels/effectargvalue_data.h"


/*
 * structure of effect arguments set data
 * (data holder)
*/
struct EffectArgSetData
{
public:
    EffectArgSetData();
    EffectArgSetData(
            int id_,
            int effectId_,
            int easingTypeId_,
            const QString &description_,
            const QDateTime &created_,
            const MergeId &mergeid_
            );
    EffectArgSetData(EffectArgSetData &&data_);
    virtual ~EffectArgSetData() = default;
    EffectArgSetData &operator=(EffectArgSetData &&data_);
    virtual std::unique_ptr<EffectArgSetData> getDataCopy() const;

private:
    void createArrays();

public:
    int m_id = -1;
    int m_effectId = -1;                // effect id
    int m_easingTypeId = -1;            // easing type
    QString m_description;              // description
    QDateTime m_created;                // creation date and time
    MergeId m_mergeid;                  // unique id to merge

    std::shared_ptr<QVector<EffectArgValueData *>> m_effectArgValuesData;   // effect arguments' values

    EffectArgSetData(const EffectArgData &data_) = delete;
    EffectArgSetData &operator = (const EffectArgData &data_) = delete;
};


#endif // EFFECTARGSET_DATA_H
