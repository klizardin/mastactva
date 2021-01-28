#include "artefact.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


Artefact::Artefact(ArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Shader::Shader()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_shaderModel = parent_;
    m_objectModelInfo = this;
}

int Artefact::id() const
{
    return m_id;
}

void Artefact::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString Artefact::filename() const
{
    return getFilename();
}

ImageSource Artefact::getFilename() const
{
    return m_filename;
}

void Artefact::setFilename(const QString &filename_)
{
    setFilename(ImageSource(filename_));
}

void Artefact::setFilename(const ImageSource &filename_)
{
    m_filename = filename_;

    emit filenameChanged();
}

QString Artefact::hash() const
{
    return m_hash;
}

void Artefact::setHash(const QString &hash_)
{
    m_hash = hash_;

    emit hashChanged();
}

int Artefact::type() const
{
    return m_typeId;
}

void Artefact::setType(const int &type_)
{
    if(m_typeId == type_) { return; }
    m_typeId = type_;

    emit typeChanged();
}

QString Artefact::description() const
{
    return m_description;
}

void Artefact::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

void Artefact::loadChildrenVF()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void Artefact::objectLoadedVF()
{
    IListModelInfoObjectImpl::objectLoadedVF();
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        sf->add(filename(), hash(), g_shadersRelPath);
    }
}

QString Artefact::getObjectName() const
{
    return m_shaderModel->getQMLLayoutName() + QString("_Shader_")
            + QVariant::fromValue(m_appId).toString();
}


ArtefactModel::ArtefactModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ShaderModel::ShaderModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
