#include "timeutils.h"
#include <QTextStream>
#include <QTimeZone>
#include <QDebug>


namespace date_time
{
    QDateTime nowTz()
    {
        const QDateTime cdt = QDateTime::currentDateTime();
        const QDateTime cdtz = QDateTime(cdt.date(), cdt.time(), QTimeZone::systemTimeZone());
        //qDebug() << dateTimeToJsonString(cdtz);
        return cdtz.toLocalTime();
    }

    QDateTime dateTimeFromJsonString(const QString& dateTimeZ_)
    {
        QString dateTimeZ(dateTimeZ_);
        QTextStream s(&dateTimeZ);
        int year = 0, month = 0, day = 0, hours = 0, minites = 0, seconds = 0, ms = 0, tz = 0;
        char tmp;
        s >> year >> tmp >> month >> tmp >> day >> tmp;
        s >> hours >> tmp >> minites >> tmp >> seconds >> tmp >> ms >> tmp >> tz;
        if(seconds > 100)
        {
            seconds /= 1000;
            ms = seconds % 1000;
        }
        while(ms > 1000) { ms /= 10; }
        const QDateTime cdt = QDateTime(QDate(year, month, day), QTime(hours, minites, seconds, ms), QTimeZone(tz*3600));
        //qDebug() << dateTimeToJsonString(cdt);
        return cdt.toLocalTime();
    }

    QString dateTimeToJsonString(const QDateTime &dt_)
    {
        // server uses UTC
        const QString res = dt_.toUTC().toString(Qt::DateFormat::ISODateWithMs);
        // qDebug() << res;
        return res;
    }
}
