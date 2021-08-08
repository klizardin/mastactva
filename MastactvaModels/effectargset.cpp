#include "effectargset.h"
#include "../MastactvaBase/defines.h"


EffectArgSet::EffectArgSet(EffectArgSetModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgSet::EffectArgSet()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgSet::EffectArgSet()" << this;
#endif

    m_effectArgSetModel = parent_;
    m_objectModelInfo = this;
}

EffectArgSet::~EffectArgSet()
{
    if(m_affectArgValueModel)
    {
        m_affectArgValueModel->deleteLater();
    }
    m_affectArgValueModel = nullptr;
}

QVariantList EffectArgSet::argValuesOfSetIdList()
{
    if(!m_affectArgValueModel
            || !m_affectArgValueModel->isListLoaded()
            ) { return QVariantList(); }
    for(int i = 0; i < m_affectArgValueModel->size(); i++)
    {
        EffectArgValue *effectArgValue = m_affectArgValueModel->dataItemAtImpl(i);
        if(!effectArgValue ||
                !effectArgValue->getArg() ||
                !effectArgValue->getArg()->isListLoaded()
                ) { return QVariantList(); }
    }

    QVariantList res;
    for(int i = 0; i < m_affectArgValueModel->size(); i++)
    {
        EffectArgValue *effectArgValue = m_affectArgValueModel->dataItemAtImpl(i);
        if(!effectArgValue) { continue; }
        EffectArg *effectArg = effectArgValue->getArg()->getCurrentDataItem();
        if(!effectArg) { continue; }
        res.push_back(QVariant::fromValue(effectArg->id()));
    }

    qDebug() << "argValuesOfSetIdList() res = " << res;
    return res;
}

bool EffectArgSet::isChildrenLoaded()
{
    return IListModelInfoObjectImpl::isListLoadedImpl();
}

EffectArgValueModel *EffectArgSet::getArgumentValues()
{
    return m_affectArgValueModel;
}

const EffectArgValueModel *EffectArgSet::getArgumentValues() const
{
    return m_affectArgValueModel;
}

int EffectArgSet::id() const
{
    return m_id;
}

void EffectArgSet::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArgSet::effectId() const
{
    return m_effectId;
}

void EffectArgSet::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

QString EffectArgSet::description() const
{
    return m_description;
}

void EffectArgSet::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QVariant EffectArgSet::values() const
{
    if(!m_affectArgValueModel)
    {
        const_cast<EffectArgSet *>(this)->m_affectArgValueModel = const_cast<EffectArgSet *>(this)
                ->createAffectArgValueModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectArgValueModel *>(
                                       m_affectArgValueModel)
                                   )
                               );
}

void EffectArgSet::setValues(const QVariant &obj_)
{
    if(obj_.isNull() && m_affectArgValueModel)
    {
        delete m_affectArgValueModel;
        m_affectArgValueModel = nullptr;

        emit valuesChanged();
    }
}

int EffectArgSet::easingId() const
{
    return m_easingTypeId;
}

void EffectArgSet::setEasingId(const int &easingId_)
{
    if(easingId_ == m_easingTypeId) { return; }

    m_easingTypeId = easingId_;

    emit easingChanged();
}

QDateTime EffectArgSet::created() const
{
    return m_created;
}

void EffectArgSet::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

EffectArgValueModel *EffectArgSet::createAffectArgValueModel()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    EffectArgValueModel *m = new EffectArgValueModel(this);
    m->initResponse();
    m->setLayoutRefImpl("arg_set", m_effectArgSetModel->getQMLLayoutName(), "id");
    m->setCurrentRef("arg_set");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArgSetModel->getQMLLayoutName() + QString("_EffectArgSet_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArgValueModel_"));
    m->registerListModel();
    m->setParentListModelInfo(this);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

QString EffectArgSet::getObjectName() const
{
    return m_effectArgSetModel->getQMLLayoutName() + QString("_EffectArgSet_") +
            QVariant::fromValue(m_appId).toString();
}

void EffectArgSet::loadChildrenVF()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void EffectArgSet::listLoadedVF()
{
    emit childrenLoaded();
}


EffectArgSetModel::EffectArgSetModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgSetModel::EffectArgSetModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgSetModel::EffectArgSetModel()" << this;
#endif

    init(this);
}
