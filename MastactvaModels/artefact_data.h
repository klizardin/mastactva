#ifndef ARTEFACT_DATA_H
#define ARTEFACT_DATA_H


#include <memory>
#include <QString>
#include <QVector>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaModels/artefactarg_data.h"
#include "../MastactvaModels/artefacttype_data.h"


struct ArtefactData
{
public:
    ArtefactData();
    ArtefactData(
            int id_,
            const QString &name_,
            const FileSource &filename_,
            const QString &hash_,
            ArtefactTypeEn typeId_,
            const QString &description_,
            const QDateTime &created_
            );
    ArtefactData(ArtefactData &&data_);
    virtual ~ArtefactData() = default;

    ArtefactData &operator = (ArtefactData &&data_);
    std::unique_ptr<ArtefactData> copy() const;

public:
    int m_id = -1;
    QString m_name;
    FileSource m_filename;
    QString m_hash;
    int m_typeId = -1;
    QString m_description;
    QDateTime m_created;

    std::shared_ptr<QVector<ArtefactArgData *>> m_artefactArgData;

    ArtefactData(const ArtefactData &data_) = delete;
    ArtefactData &operator = (const ArtefactData &data_) = delete;
};


#endif // ARTEFACT_DATA_H
