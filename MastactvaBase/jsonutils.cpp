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

#include "jsonutils.h"
#include "../MastactvaBase/timeutils.h"


namespace json
{
    QString toString(const QJsonValue &jv_, layout::JsonTypesEn type_)
    {
        if(layout::JsonTypesEn::jt_datetime == type_)
        {
            return date_time::dateTimeToJsonString(date_time::dateTimeFromJsonString(jv_.toString()));
        }

        if(jv_.isBool())
        {
            const bool v = jv_.toBool();
            return QString::number(v ? 1 : 0);
        }
        else if(jv_.isDouble())
        {
            const double v = jv_.toDouble();
            return QString::number(v);
        }
        else if(jv_.isString())
        {
            return jv_.toString();
        }
        else
        {
            return QString();
        }
    }

    int toInt(const QJsonValue &jv_, layout::JsonTypesEn type_)
    {
        Q_UNUSED(type_);

        if(jv_.isBool())
        {
            const bool v = jv_.toBool();
            return v ? 1 : 0;
        }
        else if(jv_.isDouble())
        {
            const double v = jv_.toDouble();
            return (int)v;
        }
        else if(jv_.isString())
        {
            return QVariant::fromValue(jv_.toString()).toInt();
        }
        else
        {
            return 0;
        }
    }
}
