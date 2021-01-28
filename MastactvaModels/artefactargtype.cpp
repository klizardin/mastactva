#include "artefactargtype.h"
#include "../MastactvaBase/defines.h"


ArtefactArgType::ArtefactArgType(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ShaderArgType::ShaderArgType()" << QThread::currentThread() << QThread::currentThreadId();
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
    qDebug() << "ShaderArgTypeModel::ShaderArgTypeModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
