#include "jsonutils.h"
#include "../MastactvaBase/utils.h"


namespace json
{
    QString toString(const QJsonValue &jv_, layout::JsonTypesEn type_)
    {
        if(jv_.isBool())
        {
            if(layout::JsonTypesEn::jt_datetime == type_)
            {
                return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
            }
            bool v = jv_.toBool();
            return QString::number(v?1:0);
        }
        else if(jv_.isDouble())
        {
            if(layout::JsonTypesEn::jt_datetime == type_)
            {
                return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
            }
            double v = jv_.toDouble();
            return QString::number(v);
        }
        else if(jv_.isString())
        {
            if(layout::JsonTypesEn::jt_datetime == type_)
            {
                return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
            }
            return jv_.toString();
        }
        else if(jv_.isNull())
        {
            if(layout::JsonTypesEn::jt_datetime == type_)
            {
                return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
            }
            return QString();
        }
        else
        {
            if(layout::JsonTypesEn::jt_datetime == type_)
            {
                return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
            }
            return QString();
        }
    }

    int toInt(const QJsonValue &jv_, layout::JsonTypesEn type_)
    {
        Q_UNUSED(type_);

        if(jv_.isBool())
        {
            bool v = jv_.toBool();
            return v?1:0;
        }
        else if(jv_.isDouble())
        {
            double v = jv_.toDouble();
            return (int)v;
        }
        else if(jv_.isString())
        {
            return QVariant::fromValue(jv_.toString()).toInt();
        }
        else if(jv_.isNull())
        {
            return 0;
        }
        else
        {
            return 0;
        }
    }
}
