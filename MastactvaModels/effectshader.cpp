#include "effectshader.h"


EffectShader::EffectShader(EffectShaderModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
    m_effectShaderModel = parent_;
}

int EffectShader::id() const
{
    return m_id;
}

void EffectShader::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectShader::effectId() const
{
    return m_effectId;
}

void EffectShader::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

int EffectShader::shaderId() const
{
    return m_shaderId;
}

void EffectShader::setShaderId(const int &shaderId_)
{
    m_shaderId = shaderId_;
    if(nullptr != m_shaderModel)
    {
        m_shaderModel->parentItemChanged();
    }

    emit shaderChanged();
}

QVariant EffectShader::shader() const
{
    if(nullptr == m_shaderModel)
    {
        const_cast<EffectShader *>(this)->m_shaderModel = const_cast<EffectShader *>(this)->createShaderModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ShaderModel *>(m_shaderModel)));
}

void EffectShader::setShader(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_shaderModel)
    {
        delete m_shaderModel;
        m_shaderModel = nullptr;

        emit shaderChanged();
    }
}

ShaderModel *EffectShader::getShader()
{
    return m_shaderModel;
}

ShaderModel *EffectShader::createShaderModel()
{
    ShaderModel *m = new ShaderModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectShaderModel->getQMLLayoutName(), "shader");
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectShaderModel->getQMLLayoutName() + QString("_EffectShader_") + QVariant::fromValue(m_appId).toString() + QString("_ShaderModel_"));
    m->setLayoutIdFieldImpl("id");
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


EffectShaderModel::EffectShaderModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
