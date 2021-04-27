#include "jsonutils.h"
#include "../MastactvaBase/utils.h"


namespace json
{
    QString toString(const QJsonValue &jv_, layout::JsonTypesEn type_)
    {
        if(layout::JsonTypesEn::jt_datetime == type_)
        {
            return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
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
