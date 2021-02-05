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

int EffectArtefactArg::effectArtefactArgSetId() const
{
    return m_effectArtefactArgSetId;
}

void EffectArtefactArg::setEffectArtefactArgSetId(const int &effectArtefactArgSetId_)
{
    m_effectArtefactArgSetId = effectArtefactArgSetId_;

    emit effectArtefactArgSetIdChanged();
}

int EffectArtefactArg::argTypeId() const
{
    return m_argTypeId;
}

void EffectArtefactArg::setArgTypeId(const int &argTypeId_)
{
    m_argTypeId = argTypeId_;

    emit argTypeChanged();
}

QString EffectArtefactArg::name() const
{
    return m_name;
}

void EffectArtefactArg::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString EffectArtefactArg::defaultValue() const
{
    return m_defaultValue;
}

void EffectArtefactArg::setDefaultValue(const QString &defaultValue_)
{
    m_defaultValue = defaultValue_;

    emit defaultValueChanged();
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
