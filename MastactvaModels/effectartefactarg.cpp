#include "effectartefactarg.h"
#include "../MastactvaBase/defines.h"


EffectArtefactArg::EffectArtefactArg(EffectArtefactArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_effectArtefactArgModel = parent_;
}

int EffectArtefactArg::id() const
{
    return m_id;
}

void EffectArtefactArg::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArtefactArg::effectArtefactId() const
{
    return m_effectArtefactId;
}

void EffectArtefactArg::setEffectArtefactId(const int &effectArtefactId_)
{
    m_effectArtefactId = effectArtefactId_;

    emit effectArtefactIdChanged();
}

QString EffectArtefactArg::description() const
{
    return m_description;
}

void EffectArtefactArg::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}


EffectArtefactArgModel::EffectArtefactArgModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefactArgModel::EffectArtefactArgModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
