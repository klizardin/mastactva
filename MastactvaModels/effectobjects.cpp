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
                                   const_cast<EffectObjectInfoModel *>(
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

EffectObjectInfoModel *EffectObjects::createEffectObjectInfoModel()
{
    EffectObjectInfoModel *m = new EffectObjectInfoModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectObjectsModel->getQMLLayoutName(), "effect_object_info", false);
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectObjectsModel->getQMLLayoutName() + QString("_EffectObjects_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectObjectInfoModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setReadonlyImpl(false);
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
