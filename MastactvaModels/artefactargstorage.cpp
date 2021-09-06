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

#include "artefactargstorage.h"
#include "../MastactvaBase/defines.h"


ArtefactArgStorage::ArtefactArgStorage(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArgStorage::ArtefactArgStorage()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactArgStorage::ArtefactArgStorage()" << this;
#endif
}

int ArtefactArgStorage::id() const
{
    return m_id;
}

void ArtefactArgStorage::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString ArtefactArgStorage::storage() const
{
    return m_storage;
}

void ArtefactArgStorage::setStorage(const QString &storage_)
{
    m_storage = storage_;

    emit storageChanged();
}


ArtefactArgStorageModel::ArtefactArgStorageModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArgStorageModel::ArtefactArgStorageModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactArgStorageModel::ArtefactArgStorageModel()" << this;
#endif

    init(this);
}
