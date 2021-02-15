#include "effectobjectartefact.h"
#include "../MastactvaBase/defines.h"


ObjectArtefact::ObjectArtefact(ObjectArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefact::EffectArtefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectArtefactModel = parent_;
}

int ObjectArtefact::id() const
{
    return m_id;
}

void ObjectArtefact::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ObjectArtefact::effectObjectId() const
{
    return m_effectId;
}

void ObjectArtefact::setEffectObjectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectObjectIdChanged();
}

int ObjectArtefact::artefactId() const
{
    return m_artefactId;
}

void ObjectArtefact::setArtefactId(const int &artefactId_)
{
    m_artefactId = artefactId_;
    if(nullptr != m_artefactModel)
    {
        m_artefactModel->parentItemChanged();
    }

    emit artefactChanged();
}

QVariant ObjectArtefact::artefact() const
{
    if(nullptr == m_artefactModel)
    {
        const_cast<ObjectArtefact *>(this)->m_artefactModel = const_cast<ObjectArtefact *>(this)
                ->createArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ArtefactModel *>(
                                       m_artefactModel)
                                   )
                               );
}

void ObjectArtefact::setArtefact(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_artefactModel)
    {
        delete m_artefactModel;
        m_artefactModel = nullptr;

        emit artefactChanged();
    }
}

ArtefactModel *ObjectArtefact::getArtefact()
{
    return m_artefactModel;
}

const ArtefactModel *ObjectArtefact::getArtefact() const
{
    return m_artefactModel;
}

QVariant ObjectArtefact::effectArtefactArgSet() const
{
    if(nullptr == m_effectArtefactArgSetModel)
    {
        const_cast<ObjectArtefact *>(this)->m_effectArtefactArgSetModel = const_cast<ObjectArtefact *>(this)
                ->createEffectArtefactArgSetModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectArtefactArgSetModel *>(
                                       m_effectArtefactArgSetModel)
                                   )
                               );
}

void ObjectArtefact::setEffectArtefactArgSet(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectArtefactArgSetModel)
    {
        delete m_effectArtefactArgSetModel;
        m_effectArtefactArgSetModel = nullptr;

        emit effectArtefactArgSetChanged();
    }
}

ArtefactModel *ObjectArtefact::createArtefactModel()
{
    ArtefactModel *m = new ArtefactModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectArtefactModel->getQMLLayoutName(), "artefact");
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArtefactModel->getQMLLayoutName() + QString("_EffectArtefact_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ArtefactModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

EffectArtefactArgSetModel *ObjectArtefact::createEffectArtefactArgSetModel()
{
    EffectArtefactArgSetModel *m = new EffectArtefactArgSetModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect_object_artefact", m_effectArtefactModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect_object_artefact");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArtefactModel->getQMLLayoutName() + QString("_EffectArtefact_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArtefactArgSetModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


ObjectArtefactModel::ObjectArtefactModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefactModel::EffectArtefactModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
