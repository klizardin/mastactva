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

#include "concents.h"

DataObject::DataObject(QObject *parent) : QObject(parent)
{
}

int DataObject::intValue() const
{
    return m_intValue;
}

void DataObject::setIntValue(int v_)
{
    m_intValue = v_;
}

QString DataObject::strValue() const
{
    return m_strValue;
}

void DataObject::setStrValue(const QString &v_)
{
    m_strValue = v_;
}

void testCreatePush()
{
    ModelConcept<DataType, DataObject> concept;
    for(int i = 0; i < 3; ++i)
    {
        auto obj = concept.create();
        Q_ASSERT(obj);
        concept.add(obj);
    }
}
