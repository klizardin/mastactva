#ifndef OBJECTINFO_DATA_H
#define OBJECTINFO_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>


struct ObjectInfoData
{
public:
    ObjectInfoData() = default;
    virtual ~ObjectInfoData() = default;

    std::unique_ptr<ObjectInfoData> copy() const;

public:
    int m_id = -1;
    QString m_name;
    QString m_programmerName;
    QString m_description;
    QDateTime m_created;
};


#endif // OBJECTINFO_DATA_H
