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

#ifndef EFFECT_DATA_H
#define EFFECT_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaModels/effectobjects_data.h"
#include "../MastactvaModels/effectarg_data.h"
#include "../MastactvaModels/effectargset_data.h"


/*
 * structure that contains effect data
 * (data holder)
*/
struct EffectData
{
public:
    EffectData();
    EffectData(
            int id_,
            const QString &name_,
            const QString &description_,
            const QDateTime &created_,
            const MergeId &mergeid_
            );
    EffectData(EffectData &&data_);
    virtual ~EffectData() = default;

    EffectData &operator = (EffectData &&data_);

    // to extract data from model object
    virtual std::unique_ptr<EffectData> getDataCopy() const;

private:
    void createArrays();

public:
    int m_id = -1;
    QString m_name;                 // effect name
    QString m_description;          // effect description
    QDateTime m_created;            // creation date and time
    MergeId m_mergeid;              // unique id to merge items

    std::shared_ptr<QVector<EffectObjectsData *>> m_effectObjectsData;  // vector of objects
    std::shared_ptr<QVector<EffectArgData *>> m_effectArgsData;         // vector of effect arguments
    std::shared_ptr<QVector<EffectArgSetData *>> m_effectArgSetsData;   // vector of effect arguments' sets

    EffectData(const EffectData &data_) = delete;
    EffectData &operator = (const EffectData &data_) = delete;
};



#endif // EFFECT_DATA_H
