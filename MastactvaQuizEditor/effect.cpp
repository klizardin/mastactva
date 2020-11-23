#include "effect.h"

Effect::Effect(QObject *parent_)
    : QObject(parent_)
{
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

EffectModel::EffectModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
