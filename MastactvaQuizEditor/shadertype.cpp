#include "shadertype.h"

ShaderType::ShaderType(QObject *parent_)
    : QObject(parent_)
{
}

int ShaderType::id() const
{
    return m_id;
}

void ShaderType::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString ShaderType::type() const
{
    return m_type;
}

void ShaderType::setType(const QString &type_)
{
    m_type = type_;

    emit typeChanged();
}

ShaderTypeModel::ShaderTypeModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
