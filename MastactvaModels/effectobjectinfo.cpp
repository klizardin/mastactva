#include "effectobjectinfo.h"
#include "../MastactvaBase/defines.h"


EffectObjectInfo::EffectObjectInfo(EffectObjectInfoModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjectInfo::EffectObjectInfo()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectObjectInfoModel = parent_;
}

int EffectObjectInfo::id() const
{
    return m_id;
}

void EffectObjectInfo::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString EffectObjectInfo::name() const
{
    return m_name;
}

void EffectObjectInfo::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString EffectObjectInfo::programmerName() const
{
    return m_programmerName;
}

void EffectObjectInfo::setProgrammerName(const QString &programmerName_)
{
    m_programmerName = programmerName_;

    emit programmerNameChanged();
}

QString EffectObjectInfo::description() const
{
    return m_description;
}

void EffectObjectInfo::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime EffectObjectInfo::created() const
{
    return m_created;
}

void EffectObjectInfo::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}


EffectObjectInfoModel::EffectObjectInfoModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjectInfoModel::EffectObjectInfoModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
