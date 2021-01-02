#include "easingtype.h"


EasingType::EasingType(EasingTypeModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_easingTypeModel = parent_;
}

int EasingType::id() const
{
    return m_id;
}

void EasingType::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString EasingType::type() const
{
    return m_type;
}

void EasingType::setType(const QString &type_)
{
    m_type = type_;

    emit typeChanged();
}


EasingTypeModel::EasingTypeModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
