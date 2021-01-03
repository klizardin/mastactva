#include "localdataset.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/shadertype.h"
#include "../MastactvaModels/shaderargtype.h"
#include "../MastactvaModels/easingtype.h"


static const QString g_defaultSavePath = "./";


LocalDataSet::LocalDataSet(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
    m_savePath = g_defaultSavePath;
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
    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr == localDataAPI) { return; }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr == sf) { return; }

    localDataAPI->startSave(m_savePath);
    m_serverFilesRootDir = sf->getRootDir();
    sf->setRootDir(m_savePath);
    sf->clean(QDateTime::currentDateTime());

    create();
    m_step = 0;
    downloadStep();
}

qreal LocalDataSet::stepProgress()
{
    int i = m_step++;
    return (qreal)std::min(i, c_downloadedStepsCount) / (qreal)c_downloadedStepsCount;
}

QString LocalDataSet::savePath() const
{
    return m_savePath;
}

void LocalDataSet::setSavePath(const QString &url_)
{
    m_savePath = url_;
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

    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr != localDataAPI) { localDataAPI->endSave(); }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf) { sf->setRootDir(m_serverFilesRootDir); }

    emit progress(stepProgress());
    emit downloaded();
}

void LocalDataSet::listReloadedSlot()
{
    downloadStep();
}
