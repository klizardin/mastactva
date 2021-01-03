#include "localdataset.h"


LocalDataSet::LocalDataSet(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
}

LocalDataSet::~LocalDataSet()
{
    free();
}

void LocalDataSet::create()
{
    free();
    m_galleryModel = new GalleryModel(this);
    m_galleryModel->initResponse();
    m_galleryModel->setCurrentRef("");
    m_galleryModel->setLayoutQMLName("LocalData_GalleryModel");
    m_galleryModel->setLayoutIdFieldImpl("id");
    m_galleryModel->registerListModel();
    m_galleryModel->setAutoCreateChildrenModels(true);

    m_shaderTypeModel = new ShaderTypeModel(this);
    m_shaderTypeModel->initResponse();
    m_shaderTypeModel->setCurrentRef("");
    m_shaderTypeModel->setLayoutQMLName("LocalData_ShaderTypeModel");
    m_shaderTypeModel->setLayoutIdFieldImpl("id");
    m_shaderTypeModel->registerListModel();
    m_shaderTypeModel->setAutoCreateChildrenModels(true);

    m_shaderArgTypeModel = new ShaderArgTypeModel(this);
    m_shaderArgTypeModel->initResponse();
    m_shaderArgTypeModel->setCurrentRef("");
    m_shaderArgTypeModel->setLayoutQMLName("LocalData_ShaderArgTypeModel");
    m_shaderArgTypeModel->setLayoutIdFieldImpl("id");
    m_shaderArgTypeModel->registerListModel();
    m_shaderArgTypeModel->setAutoCreateChildrenModels(true);

    m_easingTypeModel = new EasingTypeModel(this);
    m_easingTypeModel->initResponse();
    m_easingTypeModel->setCurrentRef("");
    m_easingTypeModel->setLayoutQMLName("LocalData_EasingTypeModel");
    m_easingTypeModel->setLayoutIdFieldImpl("id");
    m_easingTypeModel->registerListModel();
    m_easingTypeModel->setAutoCreateChildrenModels(true);
}

void LocalDataSet::free()
{
    delete m_galleryModel;
    m_galleryModel = nullptr;
    delete m_shaderTypeModel;
    m_shaderTypeModel = nullptr;
    delete m_shaderArgTypeModel;
    m_shaderArgTypeModel = nullptr;
    delete m_easingTypeModel;
    m_easingTypeModel = nullptr;
}

void LocalDataSet::download()
{
    create();
    m_step = 0;
    downloadStep();
}

qreal LocalDataSet::stepProgress()
{
    int i = m_step++;
    return (qreal)std::min(i, g_downloadedStepsCount) / (qreal)g_downloadedStepsCount;
}

void LocalDataSet::downloadStep()
{
    if(nullptr != m_galleryModel && !m_galleryModel->isListLoaded())
    {
        QObject::connect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_galleryModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_shaderTypeModel && !m_shaderTypeModel->isListLoaded())
    {
        QObject::connect(m_shaderTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_shaderTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_shaderArgTypeModel && !m_shaderArgTypeModel->isListLoaded())
    {
        QObject::connect(m_shaderArgTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_shaderArgTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_easingTypeModel && !m_easingTypeModel->isListLoaded())
    {
        QObject::connect(m_easingTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_easingTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }

    if(nullptr != m_galleryModel)
    {
        QObject::disconnect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_shaderTypeModel)
    {
        QObject::disconnect(m_shaderTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_shaderArgTypeModel)
    {
        QObject::disconnect(m_shaderArgTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_easingTypeModel)
    {
        QObject::disconnect(m_easingTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    free();
    emit progress(stepProgress());
    emit downloaded();
}

void LocalDataSet::listReloadedSlot()
{
    downloadStep();
}
