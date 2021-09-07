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

#include "artefactargtype.h"
#include "../MastactvaBase/defines.h"


ArtefactArgType::ArtefactArgType(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArgType::ArtefactArgType()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactArgType::ArtefactArgType()" << this;
#endif
}

int ArtefactArgType::id() const
{
    return m_id;
}

void ArtefactArgType::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString ArtefactArgType::type() const
{
    return m_type;
}

void ArtefactArgType::setType(const QString &type_)
{
    m_type = type_;

    emit typeChanged();
}

ArtefactArgTypeModel::ArtefactArgTypeModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArgTypeModel::ArtefactArgTypeModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactArgTypeModel::ArtefactArgTypeModel()" << this;
#endif

    init(this);
}
