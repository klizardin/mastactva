#include "effectartefact.h"
#include "../MastactvaBase/defines.h"


EffectArtefact::EffectArtefact(EffectArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectShader::EffectShader()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_effectShaderModel = parent_;
}

int EffectArtefact::id() const
{
    return m_id;
}

void EffectArtefact::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArtefact::effectId() const
{
    return m_effectId;
}

void EffectArtefact::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

int EffectArtefact::artefactId() const
{
    return m_artefactId;
}

void EffectArtefact::setArtefactId(const int &artefactId_)
{
    m_artefactId = artefactId_;
    if(nullptr != m_shaderModel)
    {
        m_shaderModel->parentItemChanged();
    }

    emit shaderChanged();
}

QVariant EffectArtefact::shader() const
{
    if(nullptr == m_shaderModel)
    {
        const_cast<EffectArtefact *>(this)->m_shaderModel = const_cast<EffectArtefact *>(this)
                ->createShaderModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ArtefactModel *>(
                                       m_shaderModel)
                                   )
                               );
}

void EffectArtefact::setShader(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_shaderModel)
    {
        delete m_shaderModel;
        m_shaderModel = nullptr;

        emit shaderChanged();
    }
}

ArtefactModel *EffectArtefact::getShader()
{
    return m_shaderModel;
}

const ArtefactModel *EffectArtefact::getShader() const
{
    return m_shaderModel;
}

ArtefactModel *EffectArtefact::createShaderModel()
{
    ArtefactModel *m = new ArtefactModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectShaderModel->getQMLLayoutName(), "shader");
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectShaderModel->getQMLLayoutName() + QString("_EffectShader_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ShaderModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


EffectArtefactModel::EffectArtefactModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectShaderModel::EffectShaderModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
