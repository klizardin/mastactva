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

#include "effectargvalue.h"
#include "../MastactvaBase/defines.h"
#include "../MastactvaBase/data_utils.h"


EffectArgValueData::EffectArgValueData()
{
    m_effectArgsData = ::data_object::utils::createDataVector(static_cast<const EffectArgData *>(nullptr));
}

EffectArgValueData::EffectArgValueData(
        int id_,
        int argSetId_,
        int argId_,
        const QString &value_,
        const QString &description_,
        const QDateTime &created_
        )
    : m_id(id_),
      m_argSetId(argSetId_),
      m_argId(argId_),
      m_value(value_),
      m_description(description_),
      m_created(created_)
{
    m_effectArgsData = ::data_object::utils::createDataVector(static_cast<const EffectArgData *>(nullptr));
}

std::unique_ptr<EffectArgValueData> EffectArgValueData::getDataCopy() const
{
    std::unique_ptr<EffectArgValueData> result = std::make_unique<EffectArgValueData>();
    result->m_id = m_id;
    result->m_argSetId = m_argSetId;
    result->m_argId = m_argId;
    result->m_value = m_value;
    result->m_description = m_description;
    result->m_created = m_created;
    return result;
}


EffectArgValue::EffectArgValue(EffectArgValueModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgValue::EffectArgValue()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgValue::EffectArgValue()" << this;
#endif

    m_effectArgValueModel = parent_;
}

EffectArgValue::EffectArgValue(EffectArgValueData &&data_, EffectArgValueModel *parent_ /*= nullptr*/)
    : QObject(parent_),
      EffectArgValueData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgValue::EffectArgValue()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgValue::EffectArgValue()" << this;
#endif

    m_effectArgValueModel = parent_;
}

EffectArgValue::~EffectArgValue()
{
    if(m_effectArgModel)
    {
        m_effectArgModel->deleteLater();
    }
    m_effectArgModel = nullptr;
}

int EffectArgValue::id() const
{
    return m_id;
}

void EffectArgValue::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArgValue::argSetId() const
{
    return m_argSetId;
}

void EffectArgValue::setArgSetId(const int &argSetId_)
{
    m_argSetId = argSetId_;

    emit argSetIdChanged();
}

int EffectArgValue::argId() const
{
    return m_argId;
}

void EffectArgValue::setArgId(const int &argId_)
{
    if(argId_ == m_argId) { return; }

    m_argId = argId_;

    if(m_effectArgModel)
    {
        m_effectArgModel->parentItemChanged();
    }
    else
    {
        (void)arg();
    }

    emit argChanged();
}

QVariant EffectArgValue::arg() const
{
    if(!m_effectArgModel)
    {
        const_cast<EffectArgValue *>(this)->m_effectArgModel = const_cast<EffectArgValue *>(this)
                ->createEffectArgModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgModel *>(m_effectArgModel)));
}

void EffectArgValue::setArg(const QVariant &obj_)
{
    if(obj_.isNull() && m_effectArgModel)
    {
        delete m_effectArgModel;
        m_effectArgModel = nullptr;

        emit argChanged();
    }
}

QString EffectArgValue::value() const
{
    return m_value;
}

void EffectArgValue::setValue(const QString &value_)
{
    m_value = value_;

    emit valueChanged();
}

QString EffectArgValue::description() const
{
    return m_description;
}

void EffectArgValue::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime EffectArgValue::created() const
{
    return m_created;
}

void EffectArgValue::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

EffectArgModel *EffectArgValue::getArg()
{
    return m_effectArgModel;
}

int EffectArgValue::getArgId() const
{
    return m_argId;
}

EffectArgModel *EffectArgValue::createEffectArgModel()
{
    EffectArgModel *m = new EffectArgModel(this, m_effectArgsData);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectArgValueModel->getQMLLayoutName(), "arg");
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArgValueModel->getQMLLayoutName() + QString("_EffectArgValue_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArgModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


EffectArgValueModel::EffectArgValueModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<EffectArgValueData *>> data_ /*= std::shared_ptr<QVector<EffectArgValueData *>>{nullptr}*/
        )
    : base(parent_, data_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgValueModel::EffectArgValueModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgValueModel::EffectArgValueModel()" << this;
#endif

    init(this);
}
