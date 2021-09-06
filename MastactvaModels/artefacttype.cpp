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

#include "artefacttype.h"
#include "../MastactvaBase/defines.h"


ArtefactType::ArtefactType(QObject *parent_)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactType::ArtefactType()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactType::ArtefactType()" << this;
#endif
}

int ArtefactType::id() const
{
    return m_id;
}

void ArtefactType::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString ArtefactType::type() const
{
    return m_type;
}

void ArtefactType::setType(const QString &type_)
{
    m_type = type_;

    emit typeChanged();
}

bool ArtefactType::uniqueForObject() const
{
    return m_uniqueForObject;
}

void ArtefactType::setUniqueForObject(const bool &uniqueForObject_)
{
    m_uniqueForObject = uniqueForObject_;

    emit uniqueForObjectChanged();
}


ArtefactTypeModel::ArtefactTypeModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactTypeModel::ArtefactTypeModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactTypeModel::ArtefactTypeModel()" << this;
#endif
    init(this);
}
