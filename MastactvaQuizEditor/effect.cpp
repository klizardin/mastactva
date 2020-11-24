#include "effect.h"

Effect::Effect(EffectModel *parent_)
    : QObject(parent_)
{
    m_effectModel = parent_;
}

int Effect::id() const
{
    return m_id;
}

void Effect::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString Effect::name() const
{
    return m_name;
}

void Effect::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString Effect::description() const
{
    return m_description;
}

void Effect::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QVariant Effect::effectShaders() const
{
    if(nullptr == m_effectShadersModel)
    {
        const_cast<Effect *>(this)->m_effectShadersModel = const_cast<Effect *>(this)->createEffectShadersModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectShaderModel *>(m_effectShadersModel)));
}

void Effect::setEffectShaders(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectShadersModel)
    {
        delete m_effectShadersModel;
        m_effectShadersModel = nullptr;

        emit effectShadersChanged();
    }
}

QVariant Effect::args() const
{
    if(nullptr == m_effectArgModel)
    {
        const_cast<Effect *>(this)->m_effectArgModel = const_cast<Effect *>(this)->createEffectArgModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgModel *>(m_effectArgModel)));
}

void Effect::setArgs(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectArgModel)
    {
        delete m_effectArgModel;
        m_effectArgModel = nullptr;

        emit effectShadersChanged();
    }
}

EffectShaderModel *Effect::createEffectShadersModel()
{
    EffectShaderModel *m = new EffectShaderModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_EffectShaderModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

EffectArgModel *Effect::createEffectArgModel()
{
    EffectArgModel *m = new EffectArgModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_EffectArgModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


EffectModel::EffectModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
