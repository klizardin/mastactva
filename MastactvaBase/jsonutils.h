#ifndef JSONUTILS_H
#define JSONUTILS_H


#include <QString>
#include <QJsonValue>
#include "../MastactvaBase/layout_enums.h"


namespace json
{
    QString toString(const QJsonValue &jv_, layout::JsonTypesEn type_);
    int toInt(const QJsonValue &jv_, layout::JsonTypesEn type_);
}


#endif // JSONUTILS_H
