#include "effects_exchange.h"
#include <algorithm>
#include <QFileInfo>
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


static const char *g_defaultSavePath = "./LocalData/export.tar";
static const int g_downloadStepsCount = 10;
static const char *g_effectModelDownloadingStatus = "Effect model is downloading %1...";
static const char *g_artefactTypeModelDownloadingStatus = "Artefact type model is downloading %1...";
static const char *g_artefactArgTypeModelDownloadingStatus = "Artefact arg type model is downloading %1...";
static const char *g_artefactArgStorageModelDownloadingStatus = "Artefact arg storage model is downloading %1...";
static const char *g_easingTypeModelDownloadingStatus = "Easing type model is downloading %1...";
static const char *g_artefactModelDownloadingStatus = "Artefact model is downloading %1...";
static const char *g_effectObjectsModelDownloadingStatus = "Effect objects model is downloading %1...";
static const char *g_objectArtefactModelDownloadingStatus = "Object artefact model is downloading %1...";
static const char *g_objectInfoModelDownloadingStatus = "Object info model is downloading %1...";
static const char *g_effectModelUploadingStatus = "Effect model is uploading %1...";
static const char *g_artefactTypeModelUploadingStatus = "Artefact type model is uploading %1...";
static const char *g_artefactArgTypeModelUploadingStatus = "Artefact arg type model is uploading %1...";
static const char *g_artefactArgStorageModelUploadingStatus = "Artefact arg storage model is uploading %1...";
static const char *g_easingTypeModelUploadingStatus = "Easing type model is uploading %1...";
static const char *g_artefactModelUploadingStatus = "Artefact model is uploading %1...";
static const char *g_effectObjectsModelUploadingStatus = "Effect objects model is uploading %1...";
static const char *g_objectArtefactModelUploadingStatus = "Object artefact model is uploading %1...";
static const char *g_objectInfoModelUploadingStatus = "Object info model is uploading %1...";
static const char *g_archiveResultsStatus = "Archiving results...";
static const char *g_exctractArchiveStatus = "Extract archive...";
static const char *g_mergingStatus = "Merging ...";
static const char *g_allDoneStatus = "All downloading is done.";
static const char *g_importExchangeNamespace = "exchangeImport";
static const char *g_archExt = ".tar.gz";
static const char *g_archiveName = "effects.tar.gz";
static const char *g_forImport = "(for import)";
static const char *g_forExport = "(for export)";



void MergeData::clear()
{
    m_idsMap.clear();
    m_onlyInNew.clear();
    m_onlyInOld.clear();
    m_differents.clear();
}

void MergeData::clearIds(int newSize_, int oldSize_)
{
    m_onlyInNew.clear();
    m_onlyInOld.clear();
    m_differents.clear();
    m_onlyInNew.reserve(newSize_);
    m_onlyInOld.reserve(oldSize_);
    m_differents.reserve(std::min(newSize_, oldSize_));
}

void MergeData::pushNewId(int id_, const QString &mergeid_)
{
    m_onlyInNew.push_back({id_, mergeid_});
}

void MergeData::pushOldId(int id_, const QString &mergeid_)
{
    m_onlyInOld.push_back({id_, mergeid_});
}

void MergeData::pushDifferentId(int aId_, int bId_)
{
    m_differents.push_back({aId_, bId_});
}

void MergeData::sort()
{
    const auto cmp1 = [](
            const std::pair<int, QString> &v1_,
            const std::pair<int, QString> &v2_)->bool
    {
        return v1_.first < v2_.first;
    };
    const auto cmp2 = [](
            const std::pair<int, int> &v1_,
            const std::pair<int, int> &v2_)->bool
    {
        return v1_.first < v2_.first;
    };
    std::sort(std::begin(m_onlyInNew), std::end(m_onlyInNew), cmp1);
    std::sort(std::begin(m_onlyInOld), std::end(m_onlyInOld), cmp1);
    std::sort(std::begin(m_differents), std::end(m_differents), cmp2);
}

int MergeData::count() const
{
    return m_onlyInNew.size() + m_differents.size();
}

bool MergeData::hasDifferent() const
{
    return !m_differents.isEmpty();
}

std::pair<int,int> MergeData::popDifferentId()
{
    if(!hasDifferent())
    {
        return {-1, -1};
    }
    std::pair<int,int> abIds = m_differents.back();
    m_differents.pop_back();
    return abIds;
}

bool MergeData::hasNewId() const
{
    return !m_onlyInNew.isEmpty();
}

int MergeData::popNewId()
{
    if(!hasNewId())
    {
        return -1;
    }
    int id = m_onlyInNew.back().first;
    m_onlyInNew.pop_back();
    return id;
}

void MergeData::setIdMapping(const QString &layoutName_, int oldId_, int newId_)
{
    if(m_idsMap.contains(layoutName_)
            && m_idsMap[layoutName_].contains(oldId_)
            )
    {
        m_idsMap[layoutName_][oldId_] = newId_;
    }
}

void MergeData::addIdMapping(const QString &layoutName_, int oldId_)
{
    if(!m_idsMap.contains(layoutName_))
    {
        m_idsMap.insert(layoutName_, QHash<int, int>{});
    }
    if(!m_idsMap[layoutName_].contains(oldId_))
    {
        m_idsMap[layoutName_].insert(oldId_, -1);
    }
}


template<typename ModelType_> inline
bool isEqualModelItems(
        const typename ModelType_::DataObjectType *a_,
        const typename ModelType_::DataObjectType *b_
        )
{
    return !a_ && !b_ && ModelType_::isEqual(a_, b_);
}

template<typename ModelType_> inline
bool compare(
        ModelType_ *a_,
        ModelType_ *b_,
        MergeData &data_
        )
{
    Q_ASSERT(a_);
    Q_ASSERT(b_);
    if(!a_ || !b_)
    {
        return false;
    }

    data_.clearIds(a_->sizeImpl(), b_->sizeImpl());
    for(int i = 0; i < a_->sizeImpl(); i++)
    {
        const typename ModelType_::DataObjectType *ai = a_->dataItemAtImpl(i);
        if(!ai)
        {
            continue;
        }
        const int aId = ModelType_::getIntId(ai);
        const QString aMergeId = ModelType_::getMergeId(ai);
        bool found = false;
        for(int i = 0; i < b_->sizeImpl(); i++)
        {
            const typename ModelType_::DataObjectType *bi = b_->dataItemAtImpl(i);
            if(!bi)
            {
                continue;
            }
            const QString bMergeId = ModelType_::getMergeId(bi);
            found |= aMergeId == bMergeId;
            if(found)
            {
                break;
            }
        }
        if(!found)
        {
            data_.pushNewId(aId, aMergeId);
        }
    }
    for(int i = 0; i < b_->sizeImpl(); i++)
    {
        const typename ModelType_::DataObjectType *bi = b_->dataItemAtImpl(i);
        if(!bi)
        {
            continue;
        }
        const int bId = ModelType_::getIntId(bi);
        const QString bMergeId = ModelType_::getMergeId(bi);
        bool found = false;
        for(int i = 0; i < a_->sizeImpl(); i++)
        {
            const typename ModelType_::DataObjectType *ai = a_->dataItemAtImpl(i);
            if(!ai)
            {
                continue;
            }
            const QString aMergeId = ModelType_::getMergeId(ai);
            found |= aMergeId == bMergeId;
            if(found)
            {
                break;
            }
        }
        if(!found)
        {
            data_.pushOldId(bId, bMergeId);
        }
    }
    for(int i = 0; i < a_->sizeImpl(); i++)
    {
        const typename ModelType_::DataObjectType *ai = a_->dataItemAtImpl(i);
        if(!ai)
        {
            continue;
        }
        const int aId = ModelType_::getIntId(ai);
        const QString aMergeId = ModelType_::getMergeId(ai);
        const typename ModelType_::DataObjectType *itemA = ai;
        const typename ModelType_::DataObjectType *itemB = nullptr;
        bool found = false;
        for(int i = 0; i < b_->sizeImpl(); i++)
        {
            const typename ModelType_::DataObjectType *bi = b_->dataItemAtImpl(i);
            if(!bi)
            {
                continue;
            }
            const QString bMergeId = ModelType_::getMergeId(bi);
            found |= aMergeId == bMergeId;
            if(found)
            {
                itemB = bi;
                break;
            }
        }
        if(!found)
        {
            continue;
        }
        if(!isEqualModelItems<ModelType_>(itemA, itemB))
        {
            const int bId = ModelType_::getIntId(itemB);
            data_.pushDifferentId(aId, bId);
        }
    }
    return true;
}

template<typename ModelType_> inline
QHash<QString, QVariant> filterItem(
        const typename ModelType_::DataObjectType *item_,
        const MergeData &data_
        )
{
    Q_UNUSED(item_);
    Q_UNUSED(data_);
    return QHash<QString, QVariant>{};
}


template<typename ModelType_>
int MergeItem<ModelType_>::countSteps(
        ModelType_ *model_,
        ModelType_ *inputModel_
        )
{
    MergeData data;
    return
        compare(
            inputModel_,
            model_,
            data
            )
        ? data.count()
        : 0
        ;
}

template<typename ModelType_>
void MergeItem<ModelType_>::clear()
{
    m_modelTypePrepered = false;
    m_modelTypeMerged = false;
    m_modelItemNew = nullptr;
    m_oldInsertItemId = -1;
}

template<typename ModelType_>
bool MergeItem<ModelType_>::mergeStepImpl(
        MergeData &data_,
        EffectsExchange *effectExchange_,
        ModelType_ *model_,
        ModelType_ *inputModel_
        )
{
    if(!m_modelTypeMerged
            && !m_modelTypePrepered)
    {
        if(compare(
            inputModel_,
            model_,
            data_
            ))
        {
            data_.sort();

            QObject::connect(
                model_,
                SIGNAL(itemSet()),
                effectExchange_,
                SLOT(itemSetSlotForImport())
                );
            QObject::connect(
                model_,
                SIGNAL(itemAdded()),
                effectExchange_,
                SLOT(itemAddedSlotForImport())
                );
        }
        else
        {
            m_modelTypeMerged = true;
        }
        m_modelTypePrepered = true;
    }
    if(!m_modelTypeMerged
            && m_modelTypePrepered)
    {
        if(data_.hasDifferent())
        {
            const std::pair<int,int> abIds = data_.popDifferentId();
            const typename ModelType_::DataObjectType *itemOld =
                    model_->findDataItemByIdImpl(
                        QVariant::fromValue(abIds.second)
                        );
            typename ModelType_::DataObjectType *itemNew =
                    inputModel_->findDataItemByIdImpl(
                        QVariant::fromValue(abIds.first)
                        );
            const int index = model_->indexOfDataItemImpl(itemOld);
            if(index >= 0 && itemOld)
            {
                model_->setDataItemImpl(
                    index,
                    itemNew,
                    filterItem<ModelType_>(itemNew, data_)
                    );
            }
            return true;
        }

        const QString layoutName = model_->getJsonLayoutName();

        if(m_oldInsertItemId >= 0
                && m_modelItemNew
                )
        {
            data_.setIdMapping(
                        layoutName,
                        m_oldInsertItemId,
                        ModelType_::getIntId(m_modelItemNew)
                        );
        }
        m_oldInsertItemId = -1;
        m_modelItemNew = nullptr;

        if(data_.hasNewId())
        {
            m_oldInsertItemId = data_.popNewId();
            data_.addIdMapping(layoutName, m_oldInsertItemId);
            const typename ModelType_::DataObjectType *itemOld =
                    inputModel_->findDataItemByIdImpl(
                        QVariant::fromValue(m_oldInsertItemId)
                        );
            if(itemOld)
            {
                m_modelItemNew = model_->createDataItemImpl();
                if(m_modelItemNew)
                {
                    ModelType_::copyFromTo(itemOld, m_modelItemNew);
                    model_->addDataItemImpl(m_modelItemNew);
                }
            }
            return true;
        }

        QObject::disconnect(
            model_,
            SIGNAL(itemSet()),
            effectExchange_,
            SLOT(itemSetSlotForImport())
            );
        QObject::disconnect(
            model_,
            SIGNAL(itemAdded()),
            effectExchange_,
            SLOT(itemAddedSlotForImport())
            );
        m_modelTypeMerged = true;
    }
    return false;
}


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

bool EffectsExchange::download()
{
    m_uploading = false;
    m_downloading = false;
    return downloadImpl();
}

bool EffectsExchange::downloadImpl()
{
    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr == localDataAPI) { return false; }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr == sf) { return false; }

    localDataAPI->startSave(m_path);
    m_oldPathServerFiles = sf->getRootDir();
    sf->setRootDir(m_path);
    sf->clean(QDateTime::currentDateTime());
    sf->reset();

    create();
    m_step = 0;
    m_downloading = true;
    downloadStep();
    return true;
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
    if(!m_archiveName.endsWith(g_archExt))
    {
        m_archiveName += g_archExt;
    }
}

void EffectsExchange::free()
{
    if(m_effectModel)
    {
        m_effectModel->clearResponse();
    }
    if(m_artefactModel)
    {
        m_artefactModel->clearResponse();
    }
    if(m_effectObjectsModel)
    {
        m_effectObjectsModel->clearResponse();
    }
    if(m_objectArtefactModel)
    {
        m_objectArtefactModel->clearResponse();
    }
    if(m_objectInfoModel)
    {
        m_objectInfoModel->clearResponse();
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
    m_artefactModel.reset(nullptr);
    m_effectObjectsModel.reset(nullptr);
    m_objectArtefactModel.reset(nullptr);
    m_objectInfoModel.reset(nullptr);
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
    m_effectModel->setLayoutQMLName("LocalData_Export_EffectModel");
    m_effectModel->setLayoutIdFieldImpl("id");
    m_effectModel->registerListModel();
    m_effectModel->setAutoCreateChildrenModels(true);

    m_artefactModel = std::make_unique<ArtefactModel>(this);
    m_artefactModel->initResponse();
    m_artefactModel->setCurrentRef("");
    m_artefactModel->setLayoutQMLName("LocalData_Export_ArtefactModel");
    m_artefactModel->setLayoutIdFieldImpl("id");
    m_artefactModel->registerListModel();
    m_artefactModel->setAutoCreateChildrenModels(true);

    m_effectObjectsModel = std::make_unique<EffectObjectsModel>(this);
    m_effectObjectsModel->initResponse();
    m_effectObjectsModel->setCurrentRef("");
    m_effectObjectsModel->setLayoutQMLName("LocalData_Export_EffectObjectsModel");
    m_effectObjectsModel->setLayoutIdFieldImpl("id");
    m_effectObjectsModel->registerListModel();
    m_effectObjectsModel->setAutoCreateChildrenModels(true);

    m_objectArtefactModel = std::make_unique<ObjectArtefactModel>(this);
    m_objectArtefactModel->initResponse();
    m_objectArtefactModel->setCurrentRef("");
    m_objectArtefactModel->setLayoutQMLName("LocalData_Export_ObjectArtefactModel");
    m_objectArtefactModel->setLayoutIdFieldImpl("id");
    m_objectArtefactModel->registerListModel();
    m_objectArtefactModel->setAutoCreateChildrenModels(true);

    m_objectInfoModel = std::make_unique<ObjectInfoModel>(this);
    m_objectInfoModel->initResponse();
    m_objectInfoModel->setCurrentRef("");
    m_objectInfoModel->setLayoutQMLName("LocalData_Export_ObjectInfoModel");
    m_objectInfoModel->setLayoutIdFieldImpl("id");
    m_objectInfoModel->registerListModel();
    m_objectInfoModel->setAutoCreateChildrenModels(true);

    m_artefactTypeModel = std::make_unique<ArtefactTypeModel>(this);
    m_artefactTypeModel->initResponse();
    m_artefactTypeModel->setCurrentRef("");
    m_artefactTypeModel->setLayoutQMLName("LocalData_Export_ArtefactTypeModel");
    m_artefactTypeModel->setLayoutIdFieldImpl("id");
    m_artefactTypeModel->registerListModel();
    m_artefactTypeModel->setAutoCreateChildrenModels(true);

    m_artefactArgTypeModel = std::make_unique<ArtefactArgTypeModel>(this);
    m_artefactArgTypeModel->initResponse();
    m_artefactArgTypeModel->setCurrentRef("");
    m_artefactArgTypeModel->setLayoutQMLName("LocalData_Export_ArtefactArgTypeModel");
    m_artefactArgTypeModel->setLayoutIdFieldImpl("id");
    m_artefactArgTypeModel->registerListModel();
    m_artefactArgTypeModel->setAutoCreateChildrenModels(true);

    m_artefactArgStorageModel = std::make_unique<ArtefactArgStorageModel>(this);
    m_artefactArgStorageModel->initResponse();
    m_artefactArgStorageModel->setCurrentRef("");
    m_artefactArgStorageModel->setLayoutQMLName("LocalData_Export_ArtefactArgStorageModel");
    m_artefactArgStorageModel->setLayoutIdFieldImpl("id");
    m_artefactArgStorageModel->registerListModel();
    m_artefactArgStorageModel->setAutoCreateChildrenModels(true);

    m_easingTypeModel = std::make_unique<EasingTypeModel>(this);
    m_easingTypeModel->initResponse();
    m_easingTypeModel->setCurrentRef("");
    m_easingTypeModel->setLayoutQMLName("LocalData_Export_EasingTypeModel");
    m_easingTypeModel->setLayoutIdFieldImpl("id");
    m_easingTypeModel->registerListModel();
    m_easingTypeModel->setAutoCreateChildrenModels(true);
}

inline
QString msg(const char *messageBase_, bool uploading_)
{
    return QString(messageBase_)
    .arg(
        uploading_
        ? g_forImport
        : g_forExport
        );
}

void EffectsExchange::downloadStep()
{
    if(m_effectModel && !m_effectModel->isListLoaded())
    {
        QObject::connect(
                    m_effectModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_effectModel->loadList();
        emit progress(stepProgress(), msg(g_effectModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_artefactModel && !m_artefactModel->isListLoaded())
    {
        QObject::connect(
                    m_artefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_artefactModel->loadList();
        emit progress(stepProgress(), msg(g_artefactModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_effectObjectsModel && !m_effectObjectsModel->isListLoaded())
    {
        QObject::connect(
                    m_effectObjectsModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_effectObjectsModel->loadList();
        emit progress(stepProgress(), msg(g_effectObjectsModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_objectArtefactModel && !m_objectArtefactModel->isListLoaded())
    {
        QObject::connect(
                    m_objectArtefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_objectArtefactModel->loadList();
        emit progress(stepProgress(), msg(g_objectArtefactModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_objectInfoModel && !m_objectInfoModel->isListLoaded())
    {
        QObject::connect(
                    m_objectInfoModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_objectInfoModel->loadList();
        emit progress(stepProgress(), msg(g_objectInfoModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_artefactTypeModel && !m_artefactTypeModel->isListLoaded())
    {
        QObject::connect(
                    m_artefactTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_artefactTypeModel->loadList();
        emit progress(stepProgress(), msg(g_artefactTypeModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_artefactArgTypeModel && !m_artefactArgTypeModel->isListLoaded())
    {
        QObject::connect(
                    m_artefactArgTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_artefactArgTypeModel->loadList();
        emit progress(stepProgress(), msg(g_artefactArgTypeModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_artefactArgStorageModel && !m_artefactArgStorageModel->isListLoaded())
    {
        QObject::connect(
                    m_artefactArgStorageModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_artefactArgStorageModel->loadList();
        emit progress(stepProgress(), msg(g_artefactArgStorageModelDownloadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_easingTypeModel && !m_easingTypeModel->isListLoaded())
    {
        QObject::connect(
                    m_easingTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
        m_easingTypeModel->loadList();
        emit progress(stepProgress(), msg(g_easingTypeModelDownloadingStatus, m_uploading));
        return; // one model at time
    }

    disconnectDownload();

    emit progress(stepProgress(), g_archiveResultsStatus);
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
    tar.start("tar", QStringList({"-czf", m_archiveName}) + args);
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

void EffectsExchange::freeInput()
{
    if(m_inputEffectModel)
    {
        m_inputEffectModel->clearResponse();
    }
    if(m_inputArtefactModel)
    {
        m_inputArtefactModel->clearResponse();
    }
    if(m_inputEffectObjectsModel)
    {
        m_inputEffectObjectsModel->clearResponse();
    }
    if(m_inputObjectArtefactModel)
    {
        m_inputObjectArtefactModel->clearResponse();
    }
    if(m_inputObjectInfoModel)
    {
        m_inputObjectInfoModel->clearResponse();
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
    m_inputModelConfig.reset(nullptr);
}

void EffectsExchange::createInput()
{
    free();

    m_inputModelConfig = std::make_unique<ChooseModelConfig>(
                true,
                g_importExchangeNamespace
                );

    m_inputEffectModel = std::make_unique<EffectModel>(
                this,
                m_inputModelConfig.get()
                );
    m_inputEffectModel->initResponse();
    m_inputEffectModel->setCurrentRef("");
    m_inputEffectModel->setLayoutQMLName("LocalData_Import_EffectModel");
    m_inputEffectModel->setLayoutIdFieldImpl("id");
    m_inputEffectModel->registerListModel();
    m_inputEffectModel->setAutoCreateChildrenModels(true);

    m_inputArtefactModel = std::make_unique<ArtefactModel>(
                this,
                std::shared_ptr<QVector<ArtefactData *>>{nullptr},
                m_inputModelConfig.get()
                );
    m_inputArtefactModel->initResponse();
    m_inputArtefactModel->setCurrentRef("");
    m_inputArtefactModel->setLayoutQMLName("LocalData_Import_ArtefactModel");
    m_inputArtefactModel->setLayoutIdFieldImpl("id");
    m_inputArtefactModel->registerListModel();
    m_inputArtefactModel->setAutoCreateChildrenModels(true);

    m_inputEffectObjectsModel = std::make_unique<EffectObjectsModel>(
                this,
                std::shared_ptr<QVector<EffectObjectsData *>>{nullptr},
                m_inputModelConfig.get()
                );
    m_inputEffectObjectsModel->initResponse();
    m_inputEffectObjectsModel->setCurrentRef("");
    m_inputEffectObjectsModel->setLayoutQMLName("LocalData_Import_EffectObjectsModel");
    m_inputEffectObjectsModel->setLayoutIdFieldImpl("id");
    m_inputEffectObjectsModel->registerListModel();
    m_inputEffectObjectsModel->setAutoCreateChildrenModels(true);

    m_inputObjectArtefactModel = std::make_unique<ObjectArtefactModel>(
                this,
                std::shared_ptr<QVector<ObjectArtefactData *>>{nullptr},
                m_inputModelConfig.get()
                );
    m_inputObjectArtefactModel->initResponse();
    m_inputObjectArtefactModel->setCurrentRef("");
    m_inputObjectArtefactModel->setLayoutQMLName("LocalData_Import_ObjectArtefactModel");
    m_inputObjectArtefactModel->setLayoutIdFieldImpl("id");
    m_inputObjectArtefactModel->registerListModel();
    m_inputObjectArtefactModel->setAutoCreateChildrenModels(true);

    m_inputObjectInfoModel = std::make_unique<ObjectInfoModel>(
                this,
                std::shared_ptr<QVector<ObjectInfoData *>>{nullptr},
                m_inputModelConfig.get()
                );
    m_inputObjectInfoModel->initResponse();
    m_inputObjectInfoModel->setCurrentRef("");
    m_inputObjectInfoModel->setLayoutQMLName("LocalData_Import_ObjectInfoModel");
    m_inputObjectInfoModel->setLayoutIdFieldImpl("id");
    m_inputObjectInfoModel->registerListModel();
    m_inputObjectInfoModel->setAutoCreateChildrenModels(true);

    m_inputArtefactTypeModel = std::make_unique<ArtefactTypeModel>(
                this,
                m_inputModelConfig.get()
                );
    m_inputArtefactTypeModel->initResponse();
    m_inputArtefactTypeModel->setCurrentRef("");
    m_inputArtefactTypeModel->setLayoutQMLName("LocalData_Import_ArtefactTypeModel");
    m_inputArtefactTypeModel->setLayoutIdFieldImpl("id");
    m_inputArtefactTypeModel->registerListModel();
    m_inputArtefactTypeModel->setAutoCreateChildrenModels(true);

    m_inputArtefactArgTypeModel = std::make_unique<ArtefactArgTypeModel>(
                this,
                m_inputModelConfig.get()
                );
    m_inputArtefactArgTypeModel->initResponse();
    m_inputArtefactArgTypeModel->setCurrentRef("");
    m_inputArtefactArgTypeModel->setLayoutQMLName("LocalData_Import_ArtefactArgTypeModel");
    m_inputArtefactArgTypeModel->setLayoutIdFieldImpl("id");
    m_inputArtefactArgTypeModel->registerListModel();
    m_inputArtefactArgTypeModel->setAutoCreateChildrenModels(true);

    m_inputArtefactArgStorageModel = std::make_unique<ArtefactArgStorageModel>(
                this,
                m_inputModelConfig.get()
                );
    m_inputArtefactArgStorageModel->initResponse();
    m_inputArtefactArgStorageModel->setCurrentRef("");
    m_inputArtefactArgStorageModel->setLayoutQMLName("LocalData_Import_ArtefactArgStorageModel");
    m_inputArtefactArgStorageModel->setLayoutIdFieldImpl("id");
    m_inputArtefactArgStorageModel->registerListModel();
    m_inputArtefactArgStorageModel->setAutoCreateChildrenModels(true);

    m_inputEasingTypeModel = std::make_unique<EasingTypeModel>(
                this,
                m_inputModelConfig.get()
                );
    m_inputEasingTypeModel->initResponse();
    m_inputEasingTypeModel->setCurrentRef("");
    m_inputEasingTypeModel->setLayoutQMLName("LocalData_Import_EasingTypeModel");
    m_inputEasingTypeModel->setLayoutIdFieldImpl("id");
    m_inputEasingTypeModel->registerListModel();
    m_inputEasingTypeModel->setAutoCreateChildrenModels(true);
}

bool EffectsExchange::upload()
{
    m_downloading = false;
    m_uploading = false;
    return uploadImpl();
}

bool EffectsExchange::mergeDownload()
{
    QTemporaryDir tmpDir;
    if(!tmpDir.isValid())
    {
        return false;
    }
    QFileInfo fi(QDir(tmpDir.path()), g_archiveName);
    initSavePath(fi.absoluteFilePath());
    downloadImpl();
    return true;
}

bool EffectsExchange::merge()
{
    return mergeImpl();
}

bool EffectsExchange::uploadImpl()
{
    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr == localDataAPI) { return false; }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr == sf) { return false; }

    QTemporaryDir tmpDir;
    if(!tmpDir.isValid())
    {
        return false;
    }

    createInput();
    if(!m_inputModelConfig)
    {
        return false;
    }
    LocalDataAPICache *localDataAPICache = m_inputModelConfig->getDataAPIFile();
    if(!localDataAPICache)
    {
        return false;
    }
    m_oldPathServerFiles = sf->getRootDir();
    sf->setRootDir(tmpDir.path());
    sf->clean(QDateTime::currentDateTime());
    sf->reset();

    m_step = 0;
    m_uploading = true;

    emit progress(stepProgress(), g_exctractArchiveStatus);
    extractArchive(tmpDir.path());
    localDataAPICache->loadDBFrom(tmpDir.path());

    uploadStep();
    return true;
}

void EffectsExchange::extractArchive(const QString &path_)
{
    QProcess tar;
    tar.setWorkingDirectory(path_);
    tar.start("tar", QStringList({"-xzf", m_archiveName}));
    if(!tar.waitForStarted())
    {
        return;
    }
    if(!tar.waitForFinished())
    {
        return;
    }
}

void EffectsExchange::uploadStep()
{
    if(m_inputEffectModel && !m_inputEffectModel->isListLoaded())
    {
        QObject::connect(
                    m_inputEffectModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputEffectModel->loadList();
        emit progress(stepProgress(), msg(g_effectModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputArtefactModel && !m_inputArtefactModel->isListLoaded())
    {
        QObject::connect(
                    m_inputArtefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputArtefactModel->loadList();
        emit progress(stepProgress(), msg(g_artefactModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputEffectObjectsModel && !m_inputEffectObjectsModel->isListLoaded())
    {
        QObject::connect(
                    m_inputEffectObjectsModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputEffectObjectsModel->loadList();
        emit progress(stepProgress(), msg(g_effectObjectsModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputObjectArtefactModel && !m_inputObjectArtefactModel->isListLoaded())
    {
        QObject::connect(
                    m_inputObjectArtefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputObjectArtefactModel->loadList();
        emit progress(stepProgress(), msg(g_objectArtefactModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputObjectInfoModel && !m_inputObjectInfoModel->isListLoaded())
    {
        QObject::connect(
                    m_inputObjectInfoModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputObjectInfoModel->loadList();
        emit progress(stepProgress(), msg(g_objectInfoModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputArtefactTypeModel && !m_inputArtefactTypeModel->isListLoaded())
    {
        QObject::connect(
                    m_inputArtefactTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputArtefactTypeModel->loadList();
        emit progress(stepProgress(), msg(g_artefactTypeModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputArtefactArgTypeModel && !m_inputArtefactArgTypeModel->isListLoaded())
    {
        QObject::connect(
                    m_inputArtefactArgTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputArtefactArgTypeModel->loadList();
        emit progress(stepProgress(), msg(g_artefactArgTypeModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputArtefactArgStorageModel && !m_inputArtefactArgStorageModel->isListLoaded())
    {
        QObject::connect(
                    m_inputArtefactArgStorageModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputArtefactArgStorageModel->loadList();
        emit progress(stepProgress(), msg(g_artefactArgStorageModelUploadingStatus, m_uploading));
        return; // one model at time
    }
    if(m_inputEasingTypeModel && !m_inputEasingTypeModel->isListLoaded())
    {
        QObject::connect(
                    m_inputEasingTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
        m_inputEasingTypeModel->loadList();
        emit progress(stepProgress(), msg(g_easingTypeModelUploadingStatus, m_uploading));
        return; // one model at time
    }

    disconnectUpload();

    emit progress(stepProgress(), g_allDoneStatus);
    emit uploaded();
}

void EffectsExchange::listReloadedSlotForImport()
{
    uploadStep();
}

void EffectsExchange::cancel()
{
    if(m_downloading)
    {
        disconnectDownload();
        free();
    }
    if(m_uploading)
    {
        disconnectUpload();
        freeInput();
    }
}

void EffectsExchange::disconnectDownload()
{
    if(m_effectModel)
    {
        QObject::disconnect(
                    m_effectModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_artefactModel)
    {
        QObject::disconnect(
                    m_artefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_effectObjectsModel)
    {
        QObject::disconnect(
                    m_effectObjectsModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_objectArtefactModel)
    {
        QObject::disconnect(
                    m_objectArtefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_objectInfoModel)
    {
        QObject::disconnect(
                    m_objectInfoModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_artefactTypeModel)
    {
        QObject::disconnect(
                    m_artefactTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_artefactArgTypeModel)
    {
        QObject::disconnect(
                    m_artefactArgTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_artefactArgStorageModel)
    {
        QObject::disconnect(
                    m_artefactArgStorageModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
    }
    if(m_easingTypeModel)
    {
        QObject::disconnect(
                    m_easingTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlot())
                    );
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
}

void EffectsExchange::disconnectUpload()
{
    if(m_inputEffectModel)
    {
        QObject::disconnect(
                    m_inputEffectModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputArtefactModel)
    {
        QObject::disconnect(
                    m_inputArtefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputEffectObjectsModel)
    {
        QObject::disconnect(
                    m_inputEffectObjectsModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputObjectArtefactModel)
    {
        QObject::disconnect(
                    m_inputObjectArtefactModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputObjectInfoModel)
    {
        QObject::disconnect(
                    m_inputObjectInfoModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputArtefactTypeModel)
    {
        QObject::disconnect(
                    m_inputArtefactTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputArtefactArgTypeModel)
    {
        QObject::disconnect(
                    m_inputArtefactArgTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputArtefactArgStorageModel)
    {
        QObject::disconnect(
                    m_inputArtefactArgStorageModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }
    if(m_inputEasingTypeModel)
    {
        QObject::disconnect(
                    m_inputEasingTypeModel.get(),
                    SIGNAL(listReloaded()),
                    this,
                    SLOT(listReloadedSlotForImport())
                    );
    }

    LocalDataAPICache *localDataAPICache = m_inputModelConfig->getDataAPIFile();
    if(localDataAPICache)
    {
        localDataAPICache->loadFromDefault();
    }

    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        sf->setRootDir(m_oldPathServerFiles);
    }
}

bool EffectsExchange::mergeImpl()
{
    c_downloadStepsCount = m_merge.countSteps(
        m_easingTypeModel.get(), m_inputEasingTypeModel.get(),
        m_artefactArgStorageModel.get(), m_inputArtefactArgStorageModel.get(),
        m_artefactArgTypeModel.get(), m_inputArtefactArgTypeModel.get(),
        m_artefactTypeModel.get(), m_inputArtefactTypeModel.get()
        );
    m_mergeData.clear();
    m_merge.clear();
    mergeStep();
    return true;
}

void EffectsExchange::mergeStep()
{
    if(m_merge.mergeStep(
        m_mergeData,
        this,
        m_easingTypeModel.get(), m_inputEasingTypeModel.get(),
        m_artefactArgStorageModel.get(), m_inputArtefactArgStorageModel.get(),
        m_artefactArgTypeModel.get(), m_inputArtefactArgTypeModel.get(),
        m_artefactTypeModel.get(), m_inputArtefactTypeModel.get()
        ))
    {
        emit progress(stepProgress(), g_mergingStatus);
        return;
    }

    emit progress(stepProgress(), g_allDoneStatus);
    emit merged();
}

void EffectsExchange::itemSetSlotForImport()
{
    mergeStep();
}

void EffectsExchange::itemAddedSlotForImport()
{
    mergeStep();
}
