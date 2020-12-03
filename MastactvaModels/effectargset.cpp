#include "effectargset.h"


EffectArgSet::EffectArgSet(EffectArgSetModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_effectArgSetModel = parent_;
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
    if(nullptr == m_affectArgValueModel)
    {
        const_cast<EffectArgSet *>(this)->m_affectArgValueModel = const_cast<EffectArgSet *>(this)->createAffectArgValueModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgValueModel *>(m_affectArgValueModel)));
}

void EffectArgSet::setValues(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_affectArgValueModel)
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

EffectArgValueModel *EffectArgSet::createAffectArgValueModel()
{
    EffectArgValueModel *m = new EffectArgValueModel(this);
    m->initResponse();
    m->setLayoutRefImpl("arg_set", m_effectArgSetModel->getQMLLayoutName(), "id");
    m->setCurrentRef("arg_set");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArgSetModel->getQMLLayoutName() + QString("_EffectArgValueModel_") + QVariant::fromValue(m_appId).toString());
    m->setLayoutIdFieldImpl("id");
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


EffectArgSetModel::EffectArgSetModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
