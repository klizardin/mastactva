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
