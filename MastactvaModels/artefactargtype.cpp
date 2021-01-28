#include "artefactargtype.h"
#include "../MastactvaBase/defines.h"


ShaderArgType::ShaderArgType(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ShaderArgType::ShaderArgType()" << QThread::currentThread() << QThread::currentThreadId();
#endif
}

int ShaderArgType::id() const
{
    return m_id;
}

void ShaderArgType::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString ShaderArgType::type() const
{
    return m_type;
}

void ShaderArgType::setType(const QString &type_)
{
    m_type = type_;

    emit typeChanged();
}

ShaderArgTypeModel::ShaderArgTypeModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ShaderArgTypeModel::ShaderArgTypeModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
