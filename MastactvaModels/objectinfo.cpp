/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "objectinfo.h"
#include "../MastactvaBase/defines.h"


ObjectInfo::ObjectInfo(ObjectInfoModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectInfo::ObjectInfo()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectInfo::ObjectInfo()" << this;
#endif

    m_effectObjectInfoModel = parent_;
}

ObjectInfo::ObjectInfo(ObjectInfoData &&data_, ObjectInfoModel *parent_ /*= nullptr*/)
    : QObject(parent_)
    , ObjectInfoData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectInfo::ObjectInfo()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectInfo::ObjectInfo()" << this;
#endif

    m_effectObjectInfoModel = parent_;
}

bool ObjectInfo::isInitializeObject() const
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

QString ObjectInfo::mergeid() const
{
    return m_mergeid;
}

void ObjectInfo::setMergeid(const QString &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}


ObjectInfoModel::ObjectInfoModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<ObjectInfoData *>> data_
        /*= std::shared_ptr<QVector<ObjectInfoData *>>{nullptr}*/,
        IModelConfig *config_ /*= &DefaultModelConfig::instance()*/
        )
    :base(parent_, data_, config_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectInfoModel::ObjectInfoModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectInfoModel::ObjectInfoModel()" << this;
#endif

    init(this);
}
