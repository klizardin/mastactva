#include "shader.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


Shader::Shader(ShaderModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Shader::Shader()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_shaderModel = parent_;
    m_objectModelInfo = this;
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

void Shader::loadChildrenVF()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void Shader::objectLoadedVF()
{
    IListModelInfoObjectImpl::objectLoadedVF();
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        sf->add(filename(), hash(), g_shadersRelPath);
    }
}

QString Shader::getObjectName() const
{
    return m_shaderModel->getQMLLayoutName() + QString("_Shader_") + QVariant::fromValue(m_appId).toString();
}


ShaderModel::ShaderModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ShaderModel::ShaderModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
