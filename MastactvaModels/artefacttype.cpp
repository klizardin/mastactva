#include "artefacttype.h"
#include "../MastactvaBase/defines.h"


ArtefactType::ArtefactType(QObject *parent_)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ShaderType::ShaderType()" << QThread::currentThread() << QThread::currentThreadId();
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


ArtefactTypeModel::ArtefactTypeModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ShaderTypeModel::ShaderTypeModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
