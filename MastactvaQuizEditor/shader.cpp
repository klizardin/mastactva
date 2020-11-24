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

int Shader::getType() const
{
    return m_typeId;
}

void Shader::setType(const int &type_)
{
    if(m_typeId == type_) { return; }

    m_typeId = type_;
    if(nullptr == m_shaderTypeModel)
    {
        m_shaderTypeModel->parentItemChanged();
    }

    emit typeChanged();
}

QVariant Shader::type() const
{
    if(nullptr == m_shaderTypeModel)
    {
        const_cast<Shader *>(this)->m_shaderTypeModel = const_cast<Shader *>(this)->createShaderTypeModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ShaderTypeModel *>(m_shaderTypeModel)));
}

void Shader::setType(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_shaderTypeModel)
    {
        delete m_shaderTypeModel;
        m_shaderTypeModel = nullptr;

        emit typeChanged();
    }
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

ShaderTypeModel *Shader::createShaderTypeModel()
{
    ShaderTypeModel *m = new ShaderTypeModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_shaderModel->getQMLLayoutName(), "type", false);
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_shaderModel->getQMLLayoutName() + QString("_ShaderTypeModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


ShaderModel::ShaderModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
