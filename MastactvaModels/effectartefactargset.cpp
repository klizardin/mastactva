#include "effectartefactargset.h"
#include "../MastactvaBase/defines.h"


EffectArtefactArgSet::EffectArtefactArgSet(EffectArtefactArgSetModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_effectArtefactArgModel = parent_;
}

int EffectArtefactArgSet::id() const
{
    return m_id;
}

void EffectArtefactArgSet::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArtefactArgSet::effectArtefactId() const
{
    return m_effectArtefactId;
}

void EffectArtefactArgSet::setEffectArtefactId(const int &effectArtefactId_)
{
    m_effectArtefactId = effectArtefactId_;

    emit effectArtefactIdChanged();
}

QString EffectArtefactArgSet::description() const
{
    return m_description;
}

void EffectArtefactArgSet::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}


EffectArtefactArgSetModel::EffectArtefactArgSetModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefactArgSetModel::EffectArtefactArgSetModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
