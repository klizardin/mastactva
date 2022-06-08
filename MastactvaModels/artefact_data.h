/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ARTEFACT_DATA_H
#define ARTEFACT_DATA_H


#include <memory>
#include <QString>
#include <QVector>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaModels/artefactarg_data.h"
#include "../MastactvaModels/artefacttype_data.h"


/*
    class that contains information about arterfact
    1 artefact - 1 file of it
    1 artefact can ref to N arguments
*/
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
            const QDateTime &created_,
            const MergeId &mergeid_
            );
    ArtefactData(ArtefactData &&data_);
    virtual ~ArtefactData() = default;

    ArtefactData &operator = (ArtefactData &&data_);
    virtual std::unique_ptr<ArtefactData> getDataCopy() const;

private:
    void createArrays();

public:
    int m_id = -1;              /* id to optimize server's operations */
    QString m_name;             /* artefact name */
    FileSource m_filename;      /* file of the artefact */
    QString m_hash;             /* hash of the artefact's file data */
    int m_typeId = -1;          /* type of the artefact */
    QString m_description;      /* description of the artefact */
    QDateTime m_created;        /* creation data time */
    MergeId m_mergeid;          /* unique id for merges of artafects. for a colabartive work on a project */

    std::shared_ptr<QVector<ArtefactArgData *>> m_artefactArgData;  /* list of arguments */

    ArtefactData(const ArtefactData &data_) = delete;
    ArtefactData &operator = (const ArtefactData &data_) = delete;
};


#endif // ARTEFACT_DATA_H
