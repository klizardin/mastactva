#include "effectartefact.h"
#include "../MastactvaBase/defines.h"


EffectArtefact::EffectArtefact(EffectArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefact::EffectArtefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectArtefactModel = parent_;
}

int EffectArtefact::id() const
{
    return m_id;
}

void EffectArtefact::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArtefact::effectId() const
{
    return m_effectId;
}

void EffectArtefact::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

int EffectArtefact::artefactId() const
{
    return m_artefactId;
}

void EffectArtefact::setArtefactId(const int &artefactId_)
{
    m_artefactId = artefactId_;
    if(nullptr != m_artefactModel)
    {
        m_artefactModel->parentItemChanged();
    }

    emit artefactChanged();
}

QVariant EffectArtefact::artefact() const
{
    if(nullptr == m_artefactModel)
    {
        const_cast<EffectArtefact *>(this)->m_artefactModel = const_cast<EffectArtefact *>(this)
                ->createArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ArtefactModel *>(
                                       m_artefactModel)
                                   )
                               );
}

void EffectArtefact::setArtefact(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_artefactModel)
    {
        delete m_artefactModel;
        m_artefactModel = nullptr;

        emit artefactChanged();
    }
}

ArtefactModel *EffectArtefact::getArtefact()
{
    return m_artefactModel;
}

const ArtefactModel *EffectArtefact::getArtefact() const
{
    return m_artefactModel;
}

QVariant EffectArtefact::effectArtefactArgSet() const
{
    if(nullptr == m_effectArtefactArgSetModel)
    {
        const_cast<EffectArtefact *>(this)->m_effectArtefactArgSetModel = const_cast<EffectArtefact *>(this)
                ->createEffectArtefactArgSetModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectArtefactArgSetModel *>(
                                       m_effectArtefactArgSetModel)
                                   )
                               );
}

void EffectArtefact::setEffectArtefactArgSet(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectArtefactArgSetModel)
    {
        delete m_effectArtefactArgSetModel;
        m_effectArtefactArgSetModel = nullptr;

        emit effectArtefactArgSetChanged();
    }
}

ArtefactModel *EffectArtefact::createArtefactModel()
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

EffectArtefactArgSetModel *EffectArtefact::createEffectArtefactArgSetModel()
{
    EffectArtefactArgSetModel *m = new EffectArtefactArgSetModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect_artefact", m_effectArtefactModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect_artefact");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArtefactModel->getQMLLayoutName() + QString("_EffectArtefact_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArtefactArgSetModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


EffectArtefactModel::EffectArtefactModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefactModel::EffectArtefactModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
