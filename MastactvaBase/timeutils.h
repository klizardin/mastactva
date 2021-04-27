#ifndef TIMEUTILS_H
#define TIMEUTILS_H


#include <QString>
#include <QDateTime>


namespace date_time
{
    QDateTime nowTz();
    QDateTime dateTimeFromJsonString(const QString& dateTimeZ_);
    QString dateTimeToJsonString(const QDateTime &dt_);
}


#endif // TIMEUTILS_H
