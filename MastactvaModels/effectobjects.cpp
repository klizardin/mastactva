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

#include "effectobjects.h"
#include "../MastactvaBase/defines.h"


EffectObjects::EffectObjects(EffectObjectsModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjects::EffectObjects()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectObjects::EffectObjects()" << this;
#endif

    m_effectObjectsModel = parent_;
}

EffectObjects::EffectObjects(EffectObjectsData &&dataBase_, EffectObjectsModel *parent_ /*= nullptr*/)
    : QObject(parent_),
      EffectObjectsData(std::move(dataBase_))
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjects::EffectObjects()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectObjects::EffectObjects()" << this;
#endif

    m_effectObjectsModel = parent_;
}

EffectObjects::~EffectObjects()
{
    if(m_objectInfoModel)
    {
        m_objectInfoModel->deleteLater();
    }
    m_objectInfoModel = nullptr;
    if(m_objectArtefactModel)
    {
        m_objectArtefactModel->deleteLater();
    }
    m_objectArtefactModel = nullptr;
}

bool EffectObjects::updateObjectInfoId(const QVariant &objectInfo_)
{
    if(!m_objectInfoModel)
    {
        return false;
    }
    QObject *obj = qvariant_cast<QObject *>(objectInfo_);
    const ObjectInfo *objectInfo = qobject_cast<ObjectInfo *>(obj);
    if(!objectInfo)
    {
        return false;
    }
    const int index = m_objectInfoModel->indexOfDataItemImpl(objectInfo);
    if(0 > index)
    {
        return false;
    }
    m_objectInfoModel->setCurrentIndexImpl(index);
    const ObjectInfo* item = m_objectInfoModel->getCurrentDataItem();
    if(!item)
    {
        return false;
    }
    setObjectInfoId(item->id());
    return true;
}

int EffectObjects::id() const
{
    return m_id;
}

void EffectObjects::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectObjects::effectId() const
{
    return m_effectId;
}

void EffectObjects::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

int EffectObjects::objectInfoId() const
{
    return m_objectInfoId;
}

void EffectObjects::setObjectInfoId(const int &effectObjectInfoId_)
{
    m_objectInfoId = effectObjectInfoId_;

    emit objectInfoChanged();
}

QVariant EffectObjects::objectInfo() const
{
    if(!m_objectInfoModel)
    {
        const_cast<EffectObjects *>(this)->m_objectInfoModel = const_cast<EffectObjects *>(this)
                ->createObjectInfoModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ObjectInfoModel *>(
                                       m_objectInfoModel)
                                   )
                               );
}

void EffectObjects::setObjectInfo(const QVariant &obj_)
{
    if(obj_.isNull() && m_objectInfoModel)
    {
        delete m_objectInfoModel;
        m_objectInfoModel = nullptr;

        emit objectInfoChanged();
    }
}

int EffectObjects::stepIndex() const
{
    return m_stepIndex;
}

void EffectObjects::setStepIndex(const int &stepIndex_)
{
    m_stepIndex = stepIndex_;

    emit stepIndexChanged();
}

QVariant EffectObjects::objectArtefacts() const
{
    if(!m_objectArtefactModel)
    {
        const_cast<EffectObjects *>(this)->m_objectArtefactModel = const_cast<EffectObjects *>(this)
                ->createObjectArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ObjectArtefactModel *>(
                                       m_objectArtefactModel)
                                   )
                               );
}

void EffectObjects::setObjectArtefacts(const QVariant &obj_)
{
    if(obj_.isNull() && m_objectArtefactModel)
    {
        delete m_objectArtefactModel;
        m_objectArtefactModel = nullptr;

        emit objectArtefactsChanged();
    }
}

QString EffectObjects::mergeid() const
{
    return m_mergeid;
}

void EffectObjects::setMergeid(const QString &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}

ObjectInfoModel *EffectObjects::getObjectInfoModel()
{
    return m_objectInfoModel;
}

const ObjectInfoModel *EffectObjects::getObjectInfoModel() const
{
    return m_objectInfoModel;
}

ObjectArtefactModel *EffectObjects::getObjectArtefacts()
{
    return m_objectArtefactModel;
}

const ObjectArtefactModel *EffectObjects::getObjectArtefacts() const
{
    return m_objectArtefactModel;
}

ObjectInfoModel *EffectObjects::createObjectInfoModel()
{
    ObjectInfoModel *m = new ObjectInfoModel(this, m_objectInfoData);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectObjectsModel->getQMLLayoutName(), "object_info", false);
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectObjectsModel->getQMLLayoutName() + QString("_EffectObjects_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectObjectInfoModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setReadonlyImpl(true);
    if(m->sizeImpl() == 0)
    {
        m->loadList();
    }
    return m;
}

ObjectArtefactModel *EffectObjects::createObjectArtefactModel()
{
    ObjectArtefactModel *m = new ObjectArtefactModel(this, m_objectArtefactData);
    m->initResponse();
    m->setLayoutRefImpl("object_info", m_effectObjectsModel->getQMLLayoutName(), "object_info", false);
    m->setCurrentRef("object_info");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectObjectsModel->getQMLLayoutName() + QString("_EffectObjects_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectObjectArtefactModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setReadonlyImpl(true);
    m->loadList();
    return m;
}


EffectObjectsModel::EffectObjectsModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<EffectObjectsData *>> data_ /*= std::shared_ptr<QVector<EffectObjectsData *>>{nullptr}*/,
        IModelConfig *config_ /*= &DefaultModelConfig::instance()*/
        )
    : base(parent_, data_, config_),
      m_sort(this)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjectsModel::EffectObjectsModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectObjectsModel::EffectObjectsModel()" << this;
#endif

    base::init(this);
    m_sort.init();
}

bool EffectObjectsModel::compareModelItems(const EffectObjects *i1_, const EffectObjects *i2_) const
{
    if(!i1_->getObjectInfoModel() ||
            !i1_->getObjectInfoModel()->isListLoadedImpl() ||
            !i1_->getObjectInfoModel()->getCurrentDataItem() ||
            !i2_->getObjectInfoModel() ||
            !i2_->getObjectInfoModel()->isListLoadedImpl() ||
            !i2_->getObjectInfoModel()->getCurrentDataItem())
    {
        // unable correctly sort
        return i1_->stepIndex() < i2_->stepIndex();
    }
    const bool i1IsInitObject = i1_->getObjectInfoModel()->getCurrentDataItem()->isInitializeObject();
    const bool i2IsInitObject = i2_->getObjectInfoModel()->getCurrentDataItem()->isInitializeObject();
    if(i1IsInitObject || i2IsInitObject) { return (i1IsInitObject ? 0 : 1 ) < (i2IsInitObject ? 0 : 1 ); }
    return i1_->stepIndex() < i2_->stepIndex();
}

void EffectObjectsModel::currentIndexAfterSortChanged()
{
    emit currentIndexChanged();
}
