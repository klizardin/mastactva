#include "effectarg.h"
#include "../MastactvaBase/defines.h"


EffectArg::EffectArg(EffectArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArg::EffectArg()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectArgModel = parent_;
}

int EffectArg::id() const
{
    return m_id;
}

void EffectArg::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArg::effectId() const
{
    return m_effectId;
}

void EffectArg::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

int EffectArg::objectArtefactId() const
{
    return m_objectArtefactId;
}

void EffectArg::setObjectArtefactId(const int &objectArtefactId_)
{
    m_objectArtefactId = objectArtefactId_;

    emit objectArtefactIdChanged();
}

int EffectArg::argTypeId() const
{
    return m_argTypeId;
}

void EffectArg::setArgTypeId(const int &argTypeId_)
{
    if(argTypeId_ == m_argTypeId) { return; }
    m_argTypeId = argTypeId_;
    emit argTypeChanged();
}

int EffectArg::argStorageId() const
{
    return m_argStorageId;
}

void EffectArg::setArgStorageId(const int &argStorageId_)
{
    m_argStorageId = argStorageId_;

    emit argStorageChanged();
}

QString EffectArg::name() const
{
    return m_name;
}

void EffectArg::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString EffectArg::defaultValue() const
{
    return m_defaultValue;
}

void EffectArg::setDefaultValue(const QString &defaultValue_)
{
    m_defaultValue = defaultValue_;

    emit defaultValueChanged();
}

QString EffectArg::description() const
{
    return m_description;
}

void EffectArg::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime EffectArg::created() const
{
    return m_created;
}

void EffectArg::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QVariant EffectArg::objectArtefact() const
{
    if(nullptr == m_objectArtefactModel)
    {
        const_cast<EffectArg *>(this)->m_objectArtefactModel = const_cast<EffectArg *>(this)
                ->createObjectArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ObjectArtefactModel *>(
                                       m_objectArtefactModel)
                                   )
                               );
}

void EffectArg::setObjectArtefact(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_objectArtefactModel)
    {
        delete m_objectArtefactModel;
        m_objectArtefactModel = nullptr;

        emit objectArtefactChanged();
    }
}

void EffectArg::copyFrom(const ArtefactArg *artefactArg_, int effectId, int objectArtefactId_)
{
    m_effectId = effectId;
    m_objectArtefactId = objectArtefactId_;
    m_argTypeId = artefactArg_->argTypeId();
    m_argStorageId = artefactArg_->argStorageId();
    m_name = artefactArg_->name();
    m_defaultValue = artefactArg_->defaultValue();
    m_description = artefactArg_->description();
    m_created = QDateTime::currentDateTime();
}

ObjectArtefactModel *EffectArg::createObjectArtefactModel()
{
    ObjectArtefactModel *m = new ObjectArtefactModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectArgModel->getQMLLayoutName(), "object_artefact", false);
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArgModel->getQMLLayoutName() + QString("_EffectArg_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ObjectArtefactModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setReadonlyImpl(false);
    m->loadList();
    return m;
}


EffectArgModel::EffectArgModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgModel::EffectArgModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
