#include "effects_exchange.h"
#include <QFileInfo>
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


static const char *g_defaultSavePath = "./LocalData/export.tar";
static const int g_downloadStepsCount = 5;
static const char *g_effectModelDownloadingStatus = "Effect model downloading...";
static const char *g_artefactTypeModelDownloadingStatus = "Artefact type model downloading...";
static const char *g_artefactArgTypeModelDownloadingStatus = "Artefact arg type model downloading...";
static const char *g_artefactArgStorageModelDownloadingStatus = "Artefact arg storage model downloading...";
static const char *g_easingTypeModelDownloadingStatus = "Easing type model downloading...";
static const char *g_allDoneStatus = "All downloading is done.";
static const char *g_inputExchangeNamespace = "exchangeInput";


EffectsExchange::EffectsExchange(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    initSavePath(g_defaultSavePath);
    c_downloadStepsCount = g_downloadStepsCount;
}

EffectsExchange::~EffectsExchange()
{
    free();
}

void EffectsExchange::download()
{
    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr == localDataAPI) { return; }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr == sf) { return; }

    localDataAPI->startSave(m_path);
    m_oldPathServerFiles = sf->getRootDir();
    sf->setRootDir(m_path);
    sf->clean(QDateTime::currentDateTime());
    sf->reset();

    create();
    m_step = 0;
    downloadStep();
}

void EffectsExchange::upload()
{
}

void EffectsExchange::merge()
{
}

QString EffectsExchange::savePath() const
{
    const QUrl url = QUrl::fromLocalFile(m_path);
    return url.toString();
}

void EffectsExchange::setSavePath(const QString &url_)
{
    QUrl url(url_);
    initSavePath(url.toLocalFile());

    emit savePathChanged();
    emit saveArchiveChanged();
}

QString EffectsExchange::saveArchive() const
{
    const QUrl url = QUrl::fromLocalFile(m_archiveName);
    return url.toString();
}

void EffectsExchange::setSaveArchive(const QString &url_)
{
    QUrl url(url_);
    initSavePath(url.toLocalFile());

    emit savePathChanged();
    emit saveArchiveChanged();
}

void EffectsExchange::initSavePath(const QString &path_)
{
    QFileInfo fi(path_);
    m_path = fi.absolutePath();
    m_archiveName = fi.absoluteFilePath();
    if(!m_archiveName.endsWith(".tar"))
    {
        m_archiveName += ".tar";
    }
}

void EffectsExchange::free()
{
    if(m_effectModel)
    {
        m_effectModel->clearResponse();
    }
    if(m_artefactTypeModel)
    {
        m_artefactTypeModel->clearResponse();
    }
    if(m_artefactArgTypeModel)
    {
        m_artefactArgTypeModel->clearResponse();
    }
    if(m_artefactArgStorageModel)
    {
        m_artefactArgStorageModel->clearResponse();
    }
    if(m_easingTypeModel)
    {
        m_easingTypeModel->clearResponse();
    }
    m_effectModel.reset(nullptr);
    m_artefactTypeModel.reset(nullptr);;
    m_artefactArgTypeModel.reset(nullptr);
    m_artefactArgStorageModel.reset(nullptr);
    m_easingTypeModel.reset(nullptr);
}

void EffectsExchange::create()
{
    free();

    m_effectModel = std::make_unique<EffectModel>(this);
    m_effectModel->initResponse();
    m_effectModel->setCurrentRef("");
    m_effectModel->setLayoutQMLName("LocalData_EffectModel");
    m_effectModel->setLayoutIdFieldImpl("id");
    m_effectModel->registerListModel();
    m_effectModel->setAutoCreateChildrenModels(true);

    m_artefactTypeModel = std::make_unique<ArtefactTypeModel>(this);
    m_artefactTypeModel->initResponse();
    m_artefactTypeModel->setCurrentRef("");
    m_artefactTypeModel->setLayoutQMLName("LocalData_ArtefactTypeModel");
    m_artefactTypeModel->setLayoutIdFieldImpl("id");
    m_artefactTypeModel->registerListModel();
    m_artefactTypeModel->setAutoCreateChildrenModels(true);

    m_artefactArgTypeModel = std::make_unique<ArtefactArgTypeModel>(this);
    m_artefactArgTypeModel->initResponse();
    m_artefactArgTypeModel->setCurrentRef("");
    m_artefactArgTypeModel->setLayoutQMLName("LocalData_ArtefactArgTypeModel");
    m_artefactArgTypeModel->setLayoutIdFieldImpl("id");
    m_artefactArgTypeModel->registerListModel();
    m_artefactArgTypeModel->setAutoCreateChildrenModels(true);

    m_artefactArgStorageModel = std::make_unique<ArtefactArgStorageModel>(this);
    m_artefactArgStorageModel->initResponse();
    m_artefactArgStorageModel->setCurrentRef("");
    m_artefactArgStorageModel->setLayoutQMLName("LocalData_ArtefactArgStorageModel");
    m_artefactArgStorageModel->setLayoutIdFieldImpl("id");
    m_artefactArgStorageModel->registerListModel();
    m_artefactArgStorageModel->setAutoCreateChildrenModels(true);

    m_easingTypeModel = std::make_unique<EasingTypeModel>(this);
    m_easingTypeModel->initResponse();
    m_easingTypeModel->setCurrentRef("");
    m_easingTypeModel->setLayoutQMLName("LocalData_EasingTypeModel");
    m_easingTypeModel->setLayoutIdFieldImpl("id");
    m_easingTypeModel->registerListModel();
    m_easingTypeModel->setAutoCreateChildrenModels(true);
}

void EffectsExchange::downloadStep()
{
    if(m_effectModel && !m_effectModel->isListLoaded())
    {
        QObject::connect(m_effectModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_effectModel->loadList();
        emit progress(stepProgress(), g_effectModelDownloadingStatus);
        return; // one model at time
    }
    if(m_artefactTypeModel && !m_artefactTypeModel->isListLoaded())
    {
        QObject::connect(m_artefactTypeModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_artefactTypeModel->loadList();
        emit progress(stepProgress(), g_artefactTypeModelDownloadingStatus);
        return; // one model at time
    }
    if(m_artefactArgTypeModel && !m_artefactArgTypeModel->isListLoaded())
    {
        QObject::connect(m_artefactArgTypeModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_artefactArgTypeModel->loadList();
        emit progress(stepProgress(), g_artefactArgTypeModelDownloadingStatus);
        return; // one model at time
    }
    if(m_artefactArgStorageModel && !m_artefactArgStorageModel->isListLoaded())
    {
        QObject::connect(m_artefactArgStorageModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_artefactArgStorageModel->loadList();
        emit progress(stepProgress(), g_artefactArgStorageModelDownloadingStatus);
        return; // one model at time
    }
    if(m_easingTypeModel && !m_easingTypeModel->isListLoaded())
    {
        QObject::connect(m_easingTypeModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_easingTypeModel->loadList();
        emit progress(stepProgress(), g_easingTypeModelDownloadingStatus);
        return; // one model at time
    }

    if(m_effectModel)
    {
        QObject::disconnect(m_effectModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(m_artefactTypeModel)
    {
        QObject::disconnect(m_artefactTypeModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(m_artefactArgTypeModel)
    {
        QObject::disconnect(m_artefactArgTypeModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(m_artefactArgStorageModel)
    {
        QObject::disconnect(m_artefactArgStorageModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(m_easingTypeModel)
    {
        QObject::disconnect(m_easingTypeModel.get(), SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }

    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr != localDataAPI)
    {
        localDataAPI->endSave();
    }

    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        sf->setRootDir(m_oldPathServerFiles);
    }

    archiveResults();

    emit progress(stepProgress(), g_allDoneStatus);
    emit downloaded();
}

void EffectsExchange::archiveResults()
{
    QDir dir(m_path);
    QStringList args;
    QDirIterator fit(
                dir.absolutePath(),
                QStringList() << "*.*",
                QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files,
                QDirIterator::Subdirectories
                );
    while(fit.hasNext())
    {
        QFileInfo fi(fit.next());
        if(fi.isDir() || fi.isFile())
        {
            args.push_back(fi.absoluteFilePath());
        }
    }
    QProcess tar;
    tar.start("tar", QStringList({"-cf", m_archiveName}) + args);
    if(!tar.waitForStarted())
    {
        return;
    }
    if(!tar.waitForFinished())
    {
        return;
    }
}

qreal EffectsExchange::stepProgress()
{
    const int i = m_step++;
    const qreal progress = floor
            (
                (qreal)std::min
                (
                    i,
                    c_downloadStepsCount
                ) / (qreal)c_downloadStepsCount * 1000.0
            ) / 10.0
            ;
    qDebug() << "progress =" << progress;
    return progress;
}

void EffectsExchange::freeInput()
{
    if(m_inputEffectModel)
    {
        m_inputEffectModel->clearResponse();
    }
    if(m_inputArtefactTypeModel)
    {
        m_inputArtefactTypeModel->clearResponse();
    }
    if(m_inputArtefactArgTypeModel)
    {
        m_inputArtefactArgTypeModel->clearResponse();
    }
    if(m_inputArtefactArgStorageModel)
    {
        m_inputArtefactArgStorageModel->clearResponse();
    }
    if(m_inputEasingTypeModel)
    {
        m_inputEasingTypeModel->clearResponse();
    }
    m_inputEffectModel.reset(nullptr);
    m_inputArtefactTypeModel.reset(nullptr);;
    m_inputArtefactArgTypeModel.reset(nullptr);
    m_inputArtefactArgStorageModel.reset(nullptr);
    m_inputEasingTypeModel.reset(nullptr);
    //m_inputModelConfig.reset(nullptr);
}

void EffectsExchange::createInput()
{
    free();

/*    m_inputModelConfig = std::make_unique<ChooseModelConfig>(true, g_inputExchangeNamespace);

    m_inputEffectModel = std::make_unique<EffectModel>(
                this,
                std::shared_ptr<QVector<Effect *>>{nullptr},
                m_inputModelConfig.get()
                );
    m_inputEffectModel->initResponse();
    m_inputEffectModel->setCurrentRef("");
    m_inputEffectModel->setLayoutQMLName("LocalDataInput_EffectModel");
    m_inputEffectModel->setLayoutIdFieldImpl("id");
    m_inputEffectModel->registerListModel();
    m_inputEffectModel->setAutoCreateChildrenModels(true);

    m_inputArtefactTypeModel = std::make_unique<ArtefactTypeModel>(
                this,
                std::shared_ptr<QVector<ArtefactType *>>{nullptr},
                m_inputModelConfig.get()
                );
    m_inputArtefactTypeModel->initResponse();
    m_inputArtefactTypeModel->setCurrentRef("");
    m_inputArtefactTypeModel->setLayoutQMLName("LocalDataInput_ArtefactTypeModel");
    m_inputArtefactTypeModel->setLayoutIdFieldImpl("id");
    m_inputArtefactTypeModel->registerListModel();
    m_inputArtefactTypeModel->setAutoCreateChildrenModels(true);

    m_inputArtefactArgTypeModel = std::make_unique<ArtefactArgTypeModel>(this);
    m_inputArtefactArgTypeModel->initResponse();
    m_inputArtefactArgTypeModel->setCurrentRef("");
    m_inputArtefactArgTypeModel->setLayoutQMLName("LocalDataInput_ArtefactArgTypeModel");
    m_inputArtefactArgTypeModel->setLayoutIdFieldImpl("id");
    m_inputArtefactArgTypeModel->registerListModel();
    m_inputArtefactArgTypeModel->setAutoCreateChildrenModels(true);

    m_inputArtefactArgStorageModel = std::make_unique<ArtefactArgStorageModel>(this);
    m_inputArtefactArgStorageModel->initResponse();
    m_inputArtefactArgStorageModel->setCurrentRef("");
    m_inputArtefactArgStorageModel->setLayoutQMLName("LocalDataInput_ArtefactArgStorageModel");
    m_inputArtefactArgStorageModel->setLayoutIdFieldImpl("id");
    m_inputArtefactArgStorageModel->registerListModel();
    m_inputArtefactArgStorageModel->setAutoCreateChildrenModels(true);

    m_inputEasingTypeModel = std::make_unique<EasingTypeModel>(this);
    m_inputEasingTypeModel->initResponse();
    m_inputEasingTypeModel->setCurrentRef("");
    m_inputEasingTypeModel->setLayoutQMLName("LocalDataInput_EasingTypeModel");
    m_inputEasingTypeModel->setLayoutIdFieldImpl("id");
    m_inputEasingTypeModel->registerListModel();
    m_inputEasingTypeModel->setAutoCreateChildrenModels(true);*/
}

void EffectsExchange::listReloadedSlot()
{
    downloadStep();
}
