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

    virtual std::unique_ptr<ObjectInfoData> getDataCopy() const;

public:
    int m_id = -1;
    QString m_name;
    QString m_programmerName;
    QString m_description;
    QDateTime m_created;
};


#endif // OBJECTINFO_DATA_H
