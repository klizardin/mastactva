#include "shader.h"

Shader::Shader(ShaderModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
    m_shaderModel = parent_;
}

int Shader::id() const
{
    return m_id;
}

void Shader::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString Shader::filename() const
{
    return getFilename();
}

ImageSource Shader::getFilename() const
{
    return m_filename;
}

void Shader::setFilename(const QString &filename_)
{
    setFilename(ImageSource(filename_));
}

void Shader::setFilename(const ImageSource &filename_)
{
    m_filename = filename_;

    emit filenameChanged();
}

QString Shader::hash() const
{
    return m_hash;
}

void Shader::setHash(const QString &hash_)
{
    m_hash = hash_;

    emit hashChanged();
}

int Shader::type() const
{
    return m_typeId;
}

void Shader::setType(const int &type_)
{
    if(m_typeId == type_) { return; }
    m_typeId = type_;

    emit typeChanged();
}

QString Shader::description() const
{
    return m_description;
}

void Shader::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}


ShaderModel::ShaderModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
