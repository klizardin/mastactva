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
