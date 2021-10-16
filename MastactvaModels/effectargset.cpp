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

#include "effectargset.h"
#include "../MastactvaBase/defines.h"


EffectArgSet::EffectArgSet(EffectArgSetModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgSet::EffectArgSet()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgSet::EffectArgSet()" << this;
#endif

    m_effectArgSetModel = parent_;
    m_objectModelInfo = this;
}

EffectArgSet::EffectArgSet(EffectArgSetData &&data_, EffectArgSetModel *parent_ /*= nullptr*/)
    : QObject(parent_),
      EffectArgSetData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgSet::EffectArgSet()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgSet::EffectArgSet()" << this;
#endif

    m_effectArgSetModel = parent_;
    m_objectModelInfo = this;
}

EffectArgSet::~EffectArgSet()
{
    if(m_affectArgValueModel)
    {
        m_affectArgValueModel->deleteLater();
    }
    m_affectArgValueModel = nullptr;
}

QVariantList EffectArgSet::argValuesOfSetIdList()
{
    if(!m_affectArgValueModel
            || !m_affectArgValueModel->isListLoaded()
            ) { return QVariantList(); }
    for(int i = 0; i < m_affectArgValueModel->size(); i++)
    {
        EffectArgValue *effectArgValue = m_affectArgValueModel->dataItemAtImpl(i);
        if(!effectArgValue ||
                !effectArgValue->getArg() ||
                !effectArgValue->getArg()->isListLoaded()
                ) { return QVariantList(); }
    }

    QVariantList res;
    for(int i = 0; i < m_affectArgValueModel->size(); i++)
    {
        EffectArgValue *effectArgValue = m_affectArgValueModel->dataItemAtImpl(i);
        if(!effectArgValue) { continue; }
        EffectArg *effectArg = effectArgValue->getArg()->getCurrentDataItem();
        if(!effectArg) { continue; }
        res.push_back(QVariant::fromValue(effectArg->id()));
    }

    qDebug() << "argValuesOfSetIdList() res = " << res;
    return res;
}

bool EffectArgSet::isChildrenLoaded()
{
    return IListModelInfoObjectImpl::isListLoadedImpl();
}

EffectArgValueModel *EffectArgSet::getArgumentValues()
{
    return m_affectArgValueModel;
}

const EffectArgValueModel *EffectArgSet::getArgumentValues() const
{
    return m_affectArgValueModel;
}

int EffectArgSet::id() const
{
    return m_id;
}

void EffectArgSet::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArgSet::effectId() const
{
    return m_effectId;
}

void EffectArgSet::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

QString EffectArgSet::description() const
{
    return m_description;
}

void EffectArgSet::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QVariant EffectArgSet::values() const
{
    if(!m_affectArgValueModel)
    {
        const_cast<EffectArgSet *>(this)->m_affectArgValueModel = const_cast<EffectArgSet *>(this)
                ->createAffectArgValueModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectArgValueModel *>(
                                       m_affectArgValueModel)
                                   )
                               );
}

void EffectArgSet::setValues(const QVariant &obj_)
{
    if(obj_.isNull() && m_affectArgValueModel)
    {
        delete m_affectArgValueModel;
        m_affectArgValueModel = nullptr;

        emit valuesChanged();
    }
}

int EffectArgSet::easingId() const
{
    return m_easingTypeId;
}

void EffectArgSet::setEasingId(const int &easingId_)
{
    if(easingId_ == m_easingTypeId) { return; }

    m_easingTypeId = easingId_;

    emit easingChanged();
}

QDateTime EffectArgSet::created() const
{
    return m_created;
}

void EffectArgSet::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

MergeId EffectArgSet::mergeid() const
{
    return m_mergeid;
}

void EffectArgSet::setMergeid(const MergeId &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}

EffectArgValueModel *EffectArgSet::createAffectArgValueModel()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    EffectArgValueModel *m = new EffectArgValueModel(this, m_effectArgValuesData);
    m->initResponse();
    m->setLayoutRefImpl("arg_set", m_effectArgSetModel->getQMLLayoutName(), "id");
    m->setCurrentRef("arg_set");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArgSetModel->getQMLLayoutName() + QString("_EffectArgSet_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArgValueModel_"));
    m->registerListModel();
    m->setParentListModelInfo(this);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

QString EffectArgSet::getObjectName() const
{
    return m_effectArgSetModel->getQMLLayoutName() + QString("_EffectArgSet_") +
            QVariant::fromValue(m_appId).toString();
}

void EffectArgSet::loadChildrenVF()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void EffectArgSet::listLoadedVF()
{
    emit childrenLoaded();
}


EffectArgSetModel::EffectArgSetModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<EffectArgSetData *>> data_ /*= std::shared_ptr<QVector<EffectArgSetData *>>{nullptr}*/
        )
    : base(parent_, data_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgSetModel::EffectArgSetModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgSetModel::EffectArgSetModel()" << this;
#endif

    init(this);
}
