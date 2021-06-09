#ifndef OBJECTINFO_DATA_H
#define OBJECTINFO_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>


struct ObjectInfoData
{
public:
    ObjectInfoData() = default;
    ObjectInfoData(
            int id_,
            const QString &name_,
            const QString &programmerName_,
            const QDateTime &created_
            );
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
