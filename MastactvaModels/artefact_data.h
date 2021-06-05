#ifndef ARTEFACT_DATA_H
#define ARTEFACT_DATA_H


#include <QString>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"


struct ArtefactData
{
    virtual ~ArtefactData() = default;

    int m_id = -1;
    QString m_name;
    FileSource m_filename;
    QString m_hash;
    int m_typeId = -1;
    QString m_description;
    QDateTime m_created;
};


#endif // ARTEFACT_DATA_H
