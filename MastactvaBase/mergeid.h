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

#ifndef MERGEID_H
#define MERGEID_H


#include <QString>
#include <QJsonValue>
#include "../MastactvaBase/layout_enums.h"


/*
 * class with unique id to merge data items
*/
class MergeId : public QString
{
public:
    MergeId();
    void set(const QString &val_);
    void reset();
};


/*
 * layout extensions for MergeId
*/
namespace layout
{
    inline void setValue(const QJsonValue &var_, MergeId &dta_)
    {
        if(var_.isNull() || !var_.isString()) { dta_.reset(); }
        else { dta_.set(var_.toString()); }
    }

    inline QVariant getValue(const MergeId &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(static_cast<const QString &>(val_));
    }

    inline void setValue(const QVariant &var_, MergeId &dta_)
    {
        dta_.set(qvariant_cast<QString>(var_));
    }

    inline JsonTypesEn getJsonType(const MergeId *)
    {
        return JsonTypesEn::jt_string;
    }
}


#endif // MERGEID_H
