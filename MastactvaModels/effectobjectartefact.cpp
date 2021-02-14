#include "effectobjectartefact.h"
#include "../MastactvaBase/defines.h"


EffectObjectArtefact::EffectObjectArtefact(EffectObjectArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefact::EffectArtefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectArtefactModel = parent_;
}

int EffectObjectArtefact::id() const
{
    return m_id;
}

void EffectObjectArtefact::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectObjectArtefact::effectObjectId() const
{
    return m_effectId;
}

void EffectObjectArtefact::setEffectObjectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectObjectIdChanged();
}

int EffectObjectArtefact::artefactId() const
{
    return m_artefactId;
}

void EffectObjectArtefact::setArtefactId(const int &artefactId_)
{
    m_artefactId = artefactId_;
    if(nullptr != m_artefactModel)
    {
        m_artefactModel->parentItemChanged();
    }

    emit artefactChanged();
}

QVariant EffectObjectArtefact::artefact() const
{
    if(nullptr == m_artefactModel)
    {
        const_cast<EffectObjectArtefact *>(this)->m_artefactModel = const_cast<EffectObjectArtefact *>(this)
                ->createArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ArtefactModel *>(
                                       m_artefactModel)
                                   )
                               );
}

void EffectObjectArtefact::setArtefact(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_artefactModel)
    {
        delete m_artefactModel;
        m_artefactModel = nullptr;

        emit artefactChanged();
    }
}

ArtefactModel *EffectObjectArtefact::getArtefact()
{
    return m_artefactModel;
}

const ArtefactModel *EffectObjectArtefact::getArtefact() const
{
    return m_artefactModel;
}

QVariant EffectObjectArtefact::effectArtefactArgSet() const
{
    if(nullptr == m_effectArtefactArgSetModel)
    {
        const_cast<EffectObjectArtefact *>(this)->m_effectArtefactArgSetModel = const_cast<EffectObjectArtefact *>(this)
                ->createEffectArtefactArgSetModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectArtefactArgSetModel *>(
                                       m_effectArtefactArgSetModel)
                                   )
                               );
}

void EffectObjectArtefact::setEffectArtefactArgSet(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectArtefactArgSetModel)
    {
        delete m_effectArtefactArgSetModel;
        m_effectArtefactArgSetModel = nullptr;

        emit effectArtefactArgSetChanged();
    }
}

ArtefactModel *EffectObjectArtefact::createArtefactModel()
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

EffectArtefactArgSetModel *EffectObjectArtefact::createEffectArtefactArgSetModel()
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


EffectObjectArtefactModel::EffectObjectArtefactModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefactModel::EffectArtefactModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
