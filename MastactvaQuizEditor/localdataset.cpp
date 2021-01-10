#include "localdataset.h"
#include <math.h>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/image.h"
#include "../MastactvaModels/shadertype.h"
#include "../MastactvaModels/shaderargtype.h"
#include "../MastactvaModels/easingtype.h"
#include "../MastactvaModels/userstep.h"


static const char *g_defaultSavePath = "./LocalData/";
static const int g_downloadStepsCount = 5;


LocalDataSet::LocalDataSet(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
    m_savePath = g_defaultSavePath;
    m_galleryIndex = -1;
    c_downloadStepsCount = g_downloadStepsCount;
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

    m_imageModel = new ImageModel(this);
    m_imageModel->initResponse();
    m_imageModel->setLayoutRefImpl("gallery", m_galleryModel->getQMLLayoutName(), "id", false);
    m_imageModel->setCurrentRef("gallery");
    m_imageModel->setLayoutQMLName(
                m_galleryModel->getQMLLayoutName() +
                QString("_Gallery_NNN_") +
                QString("_GalleryImageModel_")
                );
    m_imageModel->addModelParam("use_in_gallery_view", "0");
    m_imageModel->setLayoutIdFieldImpl("id");
    m_imageModel->registerListModel();
    m_imageModel->setAutoCreateChildrenModels(true);           // TODO: find way to upload images of gallery when it is required

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

    m_userStepModel = new UserStepModel(this);
    m_userStepModel->initResponse();
    m_userStepModel->setCurrentRef("");
    m_userStepModel->setLayoutQMLName("LocalData_UserStepModel");
    m_userStepModel->setLayoutIdFieldImpl("id");
    m_userStepModel->registerListModel();
    m_userStepModel->setAutoCreateChildrenModels(false);
    m_userStepModel->setStoreAfterSave(false);
    m_userStepModel->setReadonlyImpl(false);
}

void LocalDataSet::free()
{
    m_galleryIndex = -1;
    c_downloadStepsCount = g_downloadStepsCount;

    delete m_galleryModel;
    m_galleryModel = nullptr;
    delete m_imageModel;
    m_imageModel = nullptr;
    delete m_shaderTypeModel;
    m_shaderTypeModel = nullptr;
    delete m_shaderArgTypeModel;
    m_shaderArgTypeModel = nullptr;
    delete m_easingTypeModel;
    m_easingTypeModel = nullptr;
    delete m_userStepModel;
    m_userStepModel = nullptr;
}

void LocalDataSet::download()
{
    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr == localDataAPI) { return; }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr == sf) { return; }

    localDataAPI->startSave(m_savePath);
    m_serverFilesOldRootDir = sf->getRootDir();
    sf->setRootDir(m_savePath);
    sf->clean(QDateTime::currentDateTime());

    create();
    m_step = 0;
    downloadStep();
}

qreal LocalDataSet::stepProgress()
{
    const int i = m_step++;
    const qreal progress = floor((qreal)std::min(i, c_downloadStepsCount) / (qreal)c_downloadStepsCount * 1000.0) / 10.0;
    qDebug() << "progress =" << progress;
    return progress;
}

QString LocalDataSet::savePath() const
{
    return m_savePath;
}

void LocalDataSet::setSavePath(const QString &url_)
{
    QUrl url(url_);
    QFile f(url.toLocalFile());
    QFileInfo fi(f);
    m_savePath = fi.absoluteDir().absolutePath();
}

void LocalDataSet::downloadStep()
{
    if(nullptr != m_galleryModel && !m_galleryModel->isListLoaded())
    {
        QObject::connect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_galleryModel->procedure("all_available", {{"id", 0},});
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_galleryModel && nullptr != m_imageModel && m_galleryIndex < m_galleryModel->sizeImpl() - 1)
    {
        ++m_galleryIndex;
        m_galleryModel->setCurrentIndex(m_galleryIndex);
        const QVariant appId = nullptr != m_galleryModel->dataItemAtImpl(m_galleryIndex)
                ? QVariant::fromValue(m_galleryModel->dataItemAtImpl(m_galleryIndex)->getAppId())
                : QVariant()
                ;
        m_imageModel->setRefAppId(appId);
        m_imageModel->clearListLoaded();
        m_imageModel->loadList();
        if(0 == m_galleryIndex)
        {
            if(nullptr != m_galleryModel)
            {
                QObject::disconnect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
            }
            c_downloadStepsCount += m_galleryModel->sizeImpl();
            QObject::connect(m_imageModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_shaderTypeModel && !m_shaderTypeModel->isListLoaded())
    {
        if(nullptr != m_imageModel)
        {
            QObject::disconnect(m_imageModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_shaderTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_shaderTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_shaderArgTypeModel && !m_shaderArgTypeModel->isListLoaded())
    {
        if(nullptr != m_shaderTypeModel)
        {
            QObject::disconnect(m_shaderTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_shaderArgTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_shaderArgTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_easingTypeModel && !m_easingTypeModel->isListLoaded())
    {
        if(nullptr != m_shaderArgTypeModel)
        {
            QObject::disconnect(m_shaderArgTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_easingTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_easingTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_userStepModel && !m_userStepModel->isListLoaded())
    {
        if(nullptr != m_easingTypeModel)
        {
            QObject::disconnect(m_easingTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_userStepModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_userStepModel->procedure("empty_list", {{"id", 0}, });
        emit progress(stepProgress());
        return; // one model at time
    }

    if(nullptr != m_galleryModel)
    {
        QObject::disconnect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_imageModel)
    {
        QObject::disconnect(m_imageModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
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
    if(nullptr != m_userStepModel)
    {
        QObject::disconnect(m_userStepModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    //free();

    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr != localDataAPI) { localDataAPI->endSave(); }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf) { sf->setRootDir(m_serverFilesOldRootDir); }

    emit progress(stepProgress());
    emit downloaded();
}

void LocalDataSet::listReloadedSlot()
{
    downloadStep();
}
