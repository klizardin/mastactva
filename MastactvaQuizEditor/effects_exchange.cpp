#include "effects_exchange.h"
#include <QFileInfo>
#include "../MastactvaModels/effect.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


static const char *g_defaultSavePath = "./LocalData/export.tar";
static const int g_downloadStepsCount = 1;
static const char *g_effectModelDownloadingStatus = "Effect model downloading...";
static const char *g_allDoneStatus = "All downloading is done.";


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
    delete m_effectModel;
    m_effectModel = nullptr;
}

void EffectsExchange::create()
{
    free();

    m_effectModel = new EffectModel(this);
    m_effectModel->initResponse();
    m_effectModel->setCurrentRef("");
    m_effectModel->setLayoutQMLName("LocalData_EffectModel");
    m_effectModel->setLayoutIdFieldImpl("id");
    m_effectModel->registerListModel();
    m_effectModel->setAutoCreateChildrenModels(true);
}

void EffectsExchange::downloadStep()
{
    if(nullptr != m_effectModel && !m_effectModel->isListLoaded())
    {
        QObject::connect(m_effectModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_effectModel->loadList();
        emit progress(stepProgress(), g_effectModelDownloadingStatus);
        return; // one model at time
    }
    if(nullptr != m_effectModel)
    {
        QObject::disconnect(m_effectModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
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

void EffectsExchange::listReloadedSlot()
{
    downloadStep();
}
