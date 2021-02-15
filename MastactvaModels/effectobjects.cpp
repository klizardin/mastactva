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

int EffectObjects::effectObjectInfoId() const
{
    return m_effectObjectInfoId;
}

void EffectObjects::setEffectObjectInfoId(const int &effectObjectInfoId_)
{
    m_effectObjectInfoId = effectObjectInfoId_;

    emit effectObjectInfoChanged();
}

QVariant EffectObjects::effectObjectInfo() const
{
    if(nullptr == m_effectObjectInfoModel)
    {
        const_cast<EffectObjects *>(this)->m_effectObjectInfoModel = const_cast<EffectObjects *>(this)
                ->createEffectObjectInfoModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ObjectInfoModel *>(
                                       m_effectObjectInfoModel)
                                   )
                               );
}

void EffectObjects::setEffectObjectInfo(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectObjectInfoModel)
    {
        delete m_effectObjectInfoModel;
        m_effectObjectInfoModel = nullptr;

        emit effectObjectInfoChanged();
    }
}

QVariant EffectObjects::effectObjectArtefacts() const
{
    if(nullptr == m_effectObjectArtefactModel)
    {
        const_cast<EffectObjects *>(this)->m_effectObjectArtefactModel = const_cast<EffectObjects *>(this)
                ->createEffectObjectArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ObjectArtefactModel *>(
                                       m_effectObjectArtefactModel)
                                   )
                               );
}

void EffectObjects::setEffectObjectArtefacts(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectObjectArtefactModel)
    {
        delete m_effectObjectArtefactModel;
        m_effectObjectArtefactModel = nullptr;

        emit effectObjectArtefactsChanged();
    }
}

ObjectArtefactModel *EffectObjects::getEffectObjectArtefacts()
{
    return m_effectObjectArtefactModel;
}

const ObjectArtefactModel *EffectObjects::getEffectObjectArtefacts() const
{
    return m_effectObjectArtefactModel;
}

ObjectInfoModel *EffectObjects::createEffectObjectInfoModel()
{
    ObjectInfoModel *m = new ObjectInfoModel(this);
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
    m->loadList();
    return m;
}

ObjectArtefactModel *EffectObjects::createEffectObjectArtefactModel()
{
    ObjectArtefactModel *m = new ObjectArtefactModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect_object", m_effectObjectsModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("effect_object");
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

    init(this);
}
