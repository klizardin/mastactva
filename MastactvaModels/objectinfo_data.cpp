/*
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

#include "objectinfo_data.h"


ObjectInfoData::ObjectInfoData(
        int id_,
        const QString &name_,
        const QString &programmerName_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_name(name_),
      m_programmerName(programmerName_),
      m_created(created_)
{
}

std::unique_ptr<ObjectInfoData> ObjectInfoData::getDataCopy() const
{
    std::unique_ptr<ObjectInfoData> result = std::make_unique<ObjectInfoData>();
    result->m_id = m_id;
    result->m_name = m_name;
    result->m_programmerName = m_programmerName;
    result->m_description = m_description;
    result->m_created = m_created;
    return result;
}
