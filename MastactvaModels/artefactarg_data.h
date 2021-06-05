#ifndef ARTEFACTARG_DATA_H
#define ARTEFACTARG_DATA_H


#include <QString>
#include <QDateTime>


struct ArtefactArgData
{
    virtual ~ArtefactArgData() = default;

    int m_id = -1;
    int m_artefactId = -1;
    int m_argTypeId = -1;
    int m_argStorageId = -1;
    QString m_name;
    QString m_defaultValue;
    QString m_description;
    QDateTime m_created;
};


#endif // ARTEFACTARG_DATA_H
