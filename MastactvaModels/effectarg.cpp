#include "effectarg.h"
#include "../MastactvaBase/defines.h"


EffectArg::EffectArg(EffectArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArg::EffectArg()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectArgModel = parent_;
}

int EffectArg::id() const
{
    return m_id;
}

void EffectArg::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArg::effectId() const
{
    return m_effectId;
}

void EffectArg::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

int EffectArg::argTypeId() const
{
    return m_argTypeId;
}

void EffectArg::setArgTypeId(const int &argTypeId_)
{
    if(argTypeId_ == m_argTypeId) { return; }
    m_argTypeId = argTypeId_;
    emit argTypeChanged();
}

int EffectArg::argStorageId() const
{
    return m_argStorageId;
}

void EffectArg::setArgStorageId(const int &argStorageId_)
{
    m_argStorageId = argStorageId_;

    emit argStorageChanged();
}

QString EffectArg::name() const
{
    return m_name;
}

void EffectArg::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString EffectArg::defaultValue() const
{
    return m_defaultValue;
}

void EffectArg::setDefaultValue(const QString &defaultValue_)
{
    m_defaultValue = defaultValue_;

    emit defaultValueChanged();
}

QString EffectArg::description() const
{
    return m_description;
}

void EffectArg::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}


EffectArgModel::EffectArgModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgModel::EffectArgModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
