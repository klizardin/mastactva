#include "effectartefactargset.h"
#include "../MastactvaBase/defines.h"


EffectArtefactArgSet::EffectArtefactArgSet(EffectArtefactArgSetModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_effectArtefactArgSetModel = parent_;
}

int EffectArtefactArgSet::id() const
{
    return m_id;
}

void EffectArtefactArgSet::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArtefactArgSet::effectArtefactId() const
{
    return m_effectArtefactId;
}

void EffectArtefactArgSet::setEffectArtefactId(const int &effectArtefactId_)
{
    m_effectArtefactId = effectArtefactId_;

    emit effectArtefactIdChanged();
}

QString EffectArtefactArgSet::description() const
{
    return m_description;
}

void EffectArtefactArgSet::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QVariant EffectArtefactArgSet::effectArtefactArg() const
{
    if(nullptr == m_effectArtefactArgModel)
    {
        const_cast<EffectArtefactArgSet *>(this)->m_effectArtefactArgModel = const_cast<EffectArtefactArgSet *>(this)
                ->createEffectArtefactArgModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectArtefactArgModel *>(
                                       m_effectArtefactArgModel)
                                   )
                               );
}

void EffectArtefactArgSet::setEffectArtefactArg(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectArtefactArgModel)
    {
        delete m_effectArtefactArgModel;
        m_effectArtefactArgModel = nullptr;

        emit effectArtefactArgChanged();
    }
}

EffectArtefactArgModel *EffectArtefactArgSet::createEffectArtefactArgModel()
{
    EffectArtefactArgModel *m = new EffectArtefactArgModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect_artefact_arg_set", m_effectArtefactArgSetModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("effect_artefact_arg_set");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArtefactArgSetModel->getQMLLayoutName() + QString("_EffectArtefactArgSet_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArtefactArgModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setReadonlyImpl(true);
    m->loadList();
    return m;
}


EffectArtefactArgSetModel::EffectArtefactArgSetModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefactArgSetModel::EffectArtefactArgSetModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
