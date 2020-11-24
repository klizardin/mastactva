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


EffectArgSetModel::EffectArgSetModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
