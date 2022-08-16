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

#ifndef EFFECTARGUMENT_DATA_H
#define EFFECTARGUMENT_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include "../MastactvaBase/imagesource.h"


/*
 * structure of effect argument data
 * (data holder)
*/
struct EffectArgumentData
{
public:
    EffectArgumentData() = default;
    EffectArgumentData(
            int argStorageId_,
            int argTypeId_,
            const QString &name_,
            const QString &defaultValue_,
            bool hasFileReference_,
            bool hasFile_,
            const FileSource &file_,
            const QString &fileHash_
            );
    EffectArgumentData(EffectArgumentData &&data_);
    virtual ~EffectArgumentData() = default;
    EffectArgumentData & operator = (EffectArgumentData &&data_);
    std::unique_ptr<EffectArgumentData> getDataCopy() const;
    EffectArgumentData(const EffectArgumentData &data_) = default;
    EffectArgumentData &operator = (const EffectArgumentData &data_) = default;

public:
    int m_argStorageId = -1;            /* storage type */
    int m_argTypeId = -1;               /* argument type */
    QString m_name;                     /* name of the argument */
    QString m_defaultValue;             /* default value of the argument */
    bool m_hasFileReference = false;    /* does argument have a file reference TODO: explain */
    bool m_hasFile = false;             /* does argument have a fie TODO: explain */
    FileSourceReference m_file;         /* file of the argument data(?) TODO: explain */
    QString m_fileHash;                 /* hash of file data of the argument */
};


#endif // EFFECTARGUMENT_DATA_H
