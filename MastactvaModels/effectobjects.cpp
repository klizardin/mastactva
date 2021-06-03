#include "effectobjects.h"
#include "../MastactvaBase/defines.h"


EffectObjects::EffectObjects(EffectObjectsModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjects::EffectObjects()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    m_effectObjectsModel = parent_;
}

EffectObjects::EffectObjects(EffectObjectsData &&dataBase_)
    : EffectObjectsData(std::move(dataBase_))
{
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
    m->setLayoutRefImpl("id", m_effectObjectsModel->getQMLLayoutName(), "effect_object_info", false);
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
    ObjectArtefactModel *m = new ObjectArtefactModel(this);
    m->initResponse();
    m->setLayoutRefImpl("object_info", m_effectObjectsModel->getQMLLayoutName(), "effect_object_info", false);
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


EffectObjectsModel::EffectObjectsModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectObjectsModel::EffectObjectsModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    base::init(this);
    sortModelAfterChange::init();
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
