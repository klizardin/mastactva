#include "objectinfo.h"
#include "../MastactvaBase/defines.h"


ObjectInfo::ObjectInfo(ObjectInfoModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjectInfo::EffectObjectInfo()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectObjectInfoModel = parent_;
}

bool ObjectInfo::isInitializeObject()
{
    return programmerName().isEmpty();
}

int ObjectInfo::id() const
{
    return m_id;
}

void ObjectInfo::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString ObjectInfo::name() const
{
    return m_name;
}

void ObjectInfo::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString ObjectInfo::programmerName() const
{
    return m_programmerName;
}

void ObjectInfo::setProgrammerName(const QString &programmerName_)
{
    m_programmerName = programmerName_;

    emit programmerNameChanged();
}

QString ObjectInfo::description() const
{
    return m_description;
}

void ObjectInfo::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime ObjectInfo::created() const
{
    return m_created;
}

void ObjectInfo::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}


ObjectInfoModel::ObjectInfoModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjectInfoModel::EffectObjectInfoModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
