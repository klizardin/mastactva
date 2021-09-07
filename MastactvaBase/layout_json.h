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

#ifndef LAYOUT_JSON_H
#define LAYOUT_JSON_H


#include <QJsonValue>
#include "../MastactvaBase/layout_enums.h"
#include "../MastactvaBase/timeutils.h"



namespace layout
{
    // TODO: add more types
    inline void setValue(const QJsonValue &var_, int &dta_)
    {
        if(var_.isNull() || !var_.isDouble()) { dta_ = 0; }
        else { dta_ = var_.toInt(dta_); }
    }

    inline void setValue(const QJsonValue &var_, bool &dta_)
    {
        if(var_.isNull() || !var_.isDouble()) { dta_ = false; }
        else { dta_ = var_.toInt(dta_) != 0; }
    }

    inline void setValue(const QJsonValue &var_, qreal &dta_)
    {
        if(var_.isNull() || !var_.isDouble()) { dta_ = 0.0; }
        else { dta_ = var_.toDouble(dta_); }
    }

    inline void setValue(const QJsonValue &var_, QString &dta_)
    {
        if(var_.isNull() || !var_.isString()) { dta_ = QString(); }
        else { dta_ = var_.toString(); }
    }

    inline void setValue(const QJsonValue &var_, QDateTime &dta_)
    {
        if(var_.isNull() || !var_.isString()) { dta_ = date_time::dateTimeFromJsonString(QString()); }
        else { dta_ = date_time::dateTimeFromJsonString(var_.toString()); }
    }

    template<typename T_>
    inline JsonTypesEn getJsonType(const T_ *)
    {
        return JsonTypesEn::jt_undefined;
    }

    inline JsonTypesEn getJsonType(const bool *)
    {
        return JsonTypesEn::jt_bool;
    }

    inline JsonTypesEn getJsonType(const int *)
    {
        return JsonTypesEn::jt_int;
    }

    inline JsonTypesEn getJsonType(const qreal *)
    {
        return JsonTypesEn::jt_double;
    }

    inline JsonTypesEn getJsonType(const QString *)
    {
        return JsonTypesEn::jt_string;
    }

    inline JsonTypesEn getJsonType(const QDateTime *)
    {
        return JsonTypesEn::jt_datetime;
    }
}


#endif // LAYOUT_JSON_H
