#include "artefactargstorage.h"
#include "../MastactvaBase/defines.h"


ArtefactArgStorage::ArtefactArgStorage(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArgStorage::ArtefactArgStorage()" << QThread::currentThread() << QThread::currentThreadId();
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
    init(this);
}
