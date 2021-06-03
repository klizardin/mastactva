#ifndef OBJECTINFO_DATA_H
#define OBJECTINFO_DATA_H


#include <QString>
#include <QDateTime>


struct ObjectInfoData
{
    virtual ~ObjectInfoData() = default;

    int m_id = -1;
    QString m_name;
    QString m_programmerName;
    QString m_description;
    QDateTime m_created;
};


#endif // OBJECTINFO_DATA_H
