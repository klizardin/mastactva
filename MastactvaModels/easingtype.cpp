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

#include "easingtype.h"
#include "../MastactvaBase/defines.h"


EasingType::EasingType(EasingTypeModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_easingTypeModel = parent_;
#if defined(TRACE_THREADS)
    qDebug() << "EasingType::EasingType()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EasingType::EasingType()" << this;
#endif
}

int EasingType::id() const
{
    return m_id;
}

void EasingType::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString EasingType::type() const
{
    return m_type;
}

void EasingType::setType(const QString &type_)
{
    m_type = type_;

    emit typeChanged();
}

MergeId EasingType::mergeid() const
{
    return m_mergeid;
}

void EasingType::setMergeid(const MergeId &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}


EasingTypeModel::EasingTypeModel(
        QObject *parent_ /*= nullptr*/,
        IModelConfig *config_ /*= &DefaultModelConfig::instance()*/
        )
    : base(parent_, std::shared_ptr<QVector<EasingType *>>{nullptr}, config_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EasingTypeModel::EasingTypeModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EasingTypeModel::EasingTypeModel()" << this;
#endif
    init(this);
}
