#ifndef ARTEFACTARG_DATA_H
#define ARTEFACTARG_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include "../MastactvaModels/artefactargtype_data.h"
#include "../MastactvaModels/artefactargstorage_data.h"


struct ArtefactArgData
{
public:
    ArtefactArgData() = default;
    ArtefactArgData(
            int id_,
            int artefactId_,
            ArtefactArgTypeEn argTypeId_,
            ArtefactArgStorageEn argStorageId_,
            const QString &name_,
            const QString &defaultValue_,
            const QString &description_,
            const QDateTime &created_
            );
    virtual ~ArtefactArgData() = default;

    virtual std::unique_ptr<ArtefactArgData> getDataCopy() const;

public:
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
