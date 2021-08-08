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
