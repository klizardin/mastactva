#include "objectartefact.h"
#include "../MastactvaBase/defines.h"


ObjectArtefact::ObjectArtefact(ObjectArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << this;
#endif

    m_effectArtefactModel = parent_;
}

ObjectArtefact::ObjectArtefact(ObjectArtefactData &&data_, ObjectArtefactModel *parent_ /*= nullptr*/)
    : QObject(parent_)
    , ObjectArtefactData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << this;
#endif

    m_effectArtefactModel = parent_;
}

ObjectArtefact::~ObjectArtefact()
{
    if(m_artefactModel)
    {
        m_artefactModel->deleteLater();
    }
    m_artefactModel = nullptr;
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

int ObjectArtefact::objectInfoId() const
{
    return m_effectId;
}

void ObjectArtefact::setObjectInfoId(const int &effectId_)
{
    m_effectId = effectId_;

    emit objectInfoIdChanged();
}

int ObjectArtefact::artefactId() const
{
    return m_artefactId;
}

void ObjectArtefact::setArtefactId(const int &artefactId_)
{
    m_artefactId = artefactId_;
    if(m_artefactModel)
    {
        m_artefactModel->parentItemChanged();
    }

    emit artefactChanged();
}

QVariant ObjectArtefact::artefact() const
{
    if(!m_artefactModel)
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
    if(obj_.isNull() && m_artefactModel)
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

int ObjectArtefact::stepIndex() const
{
    return m_stepIndex;
}

void ObjectArtefact::setStepIndex(const int &stepIndex_)
{
    m_stepIndex = stepIndex_;

    emit stepIndexChanged();
}

bool ObjectArtefact::updateArtefactId(const QVariant &artefact_)
{
    if(!m_artefactModel
            || 0 == m_artefactModel->sizeImpl()
            )
    {
        return false;
    }
    QObject *obj = qvariant_cast<QObject *>(artefact_);
    const Artefact *artefact = qobject_cast<Artefact *>(obj);
    if(!artefact)
    {
        return false;
    }
    const int index = m_artefactModel->indexOfDataItemImpl(artefact);
    if(index < 0)
    {
        return false;
    }
    m_artefactModel->setCurrentIndexImpl(index);
    return true;
}

ArtefactModel *ObjectArtefact::createArtefactModel()
{
    ArtefactModel *m = new ArtefactModel(this, m_artefactData);
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


ObjectArtefactModel::ObjectArtefactModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<ObjectArtefactData *>> data_
        /*= std::shared_ptr<QVector<ObjectArtefactData *>>{nullptr}*/
        )
    : base(parent_, data_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectArtefactModel::ObjectArtefactModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectArtefactModel::ObjectArtefactModel()" << this;
#endif

    init(this);
}
