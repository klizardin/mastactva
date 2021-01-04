#include "Model.h"
#include "../MastactvaBase/qmlobjects.h"


//#define TRACE_MODEL_LOADING
//#define TRACE_MODEL_LOADED


ListModelBaseData::ListModelBaseData(IListModel *model_)
    :m_model(model_)
{
}

ListModelBaseData::~ListModelBaseData()
{
    if(m_autoRegister)
    {
        unregisterListModel();
    }
}

void ListModelBaseData::setLayoutRefImpl(const QString &fieldJsonName_, const QString &parentModel_, const QString &parentModelRefJsonName_, bool notify_ /*= true*/)
{
    m_refs.insert(fieldJsonName_, {fieldJsonName_, parentModel_, parentModelRefJsonName_});
    if(!notify_) { return; }
    IListModel *parentModelPtr = QMLObjectsBase::getInstance().getListModel(parentModel_);
    if(nullptr != parentModelPtr && nullptr != parentModelPtr->getModel())
    {
        QObject::connect(parentModelPtr->getModel(), SIGNAL(refreshChildren(QString)), m_modelObj, SLOT(refreshChildrenSlot(QString)));
    }
}

void ListModelBaseData::addLayoutExtraFieldsImpl(const QString &modelName_, const QVariant &appId_)
{
    m_extraFields.push_back({modelName_, appId_});
}

void ListModelBaseData::registerListModel()
{
    QMLObjectsBase::getInstance().registerModel(m_model->getQMLLayoutName(), m_model);
    m_autoRegister = true;
}

void ListModelBaseData::unregisterListModel()
{
    QMLObjectsBase::getInstance().unregisterModel(m_model->getQMLLayoutName());
    m_autoRegister = false;
}

void ListModelBaseData::setRefAppIdImpl(const QVariant &appId_)
{
    m_refAppId = appId_;
}

QVariant ListModelBaseData::getRefAppIdImpl() const
{
    return m_refAppId;
}

QVariant ListModelBaseData::getRefValueImpl() const
{
    return m_refValue;
}

void ListModelBaseData::setRefValueImpl(const QVariant &value_)
{
    m_refValue = value_;
}

int ListModelBaseData::getCurrentIndexImpl() const
{
    return m_currentIndex;
}

void ListModelBaseData::setCurrentIndexImpl(int index_)
{
    m_currentIndex = index_;
}

bool ListModelBaseData::isCurrentIndexValid(int size_) const
{
    return isIndexValid(m_currentIndex, size_);
}

bool ListModelBaseData::isIndexValid(int index_, int size_) const
{
    return index_ >= 0 && index_ < size_;
}

const QString &ListModelBaseData::currentRefImpl() const
{
    return m_currentRef;
}

void ListModelBaseData::setCurrentRefImpl(const QString &ref_)
{
    m_currentRef = ref_;
}

void ListModelBaseData::setLayoutQMLNameImpl(const QString &layoutQMLName_)
{
    m_QMLLayoutName = layoutQMLName_;
}

const QString &ListModelBaseData::getLayoutQMLNameImpl() const
{
    return m_QMLLayoutName;
}

bool ListModelBaseData::getJsonParamsGetImpl() const
{
    return m_jsonParamsGet;
}

void ListModelBaseData::setJsonParamsGetImpl(bool jsonParamsGet_)
{
    m_jsonParamsGet = jsonParamsGet_;
}

bool ListModelBaseData::addRequest(RequestData *rd_)
{
    if(nullptr == rd_) { return false; }
    m_requests.push_back(rd_);
    return true;
}

RequestData * ListModelBaseData::findRequest(const QString &requestName_)
{
    const auto fit = std::find_if(std::begin(m_requests),
                                  std::end(m_requests),
                                  [&requestName_](RequestData *request_)->bool
    {
        return nullptr != request_ && request_->getRequestName() == requestName_;
    });
    return std::end(m_requests) == fit ? nullptr : *fit;
}

bool ListModelBaseData::findRequest(RequestData *request_) const
{
    if(nullptr == request_) { return false; }
    const auto fit = std::find_if(std::begin(m_requests),
                                  std::end(m_requests),
                                  [request_](RequestData *requestItem_)->bool
    {
        return nullptr != requestItem_ && requestItem_ == request_;
    });
    return std::end(m_requests) != fit;
}

void ListModelBaseData::removeRequest(RequestData *request_)
{
    if(nullptr == request_) { return; }
    const auto fit = std::find_if(std::begin(m_requests),
                                  std::end(m_requests),
                                  [request_](RequestData *requestItem_)->bool
    {
        return nullptr != requestItem_ && requestItem_ == request_;
    });
    if(std::end(m_requests) != fit)
    {
        m_requests.erase(fit);
    }
}

int ListModelBaseData::getNextAppId(void *dta_)
{
    Q_UNUSED(dta_);
    if(++m_nextAppId < 0)
    {
        m_nextAppId = 0;
    }
    return m_nextAppId;
}

int ListModelBaseData::getOutOfRangeAppId() const
{
    return -1;
}

void ListModelBaseData::setNextAppId(int appId_)
{
    m_nextAppId = appId_;
}

void ListModelBaseData::parentItemRemoved()
{
    m_refAppId = QVariant::fromValue(getOutOfRangeAppId());
}

void ListModelBaseData::init(QObject *modelObj_)
{
    m_modelObj = modelObj_;
}

void ListModelBaseData::addModelParamImpl(const QString &name_, const QVariant &value_)
{
    m_modelParams.insert(name_, value_);
}

bool ListModelBaseData::autoCreateChildrenModelsImpl() const
{
    return m_autoCreateChildrenModels;
}

bool ListModelBaseData::autoCreateChildrenModelsOnSelectImpl() const
{
    return m_autoCreateChildrenModelsOnSelect;
}

void ListModelBaseData::setAutoCreateChildrenModelsImpl(bool autoCreateChildrenModels_)
{
    m_autoCreateChildrenModels = autoCreateChildrenModels_;
}

void ListModelBaseData::setAutoCreateChildrenModelsOnSelectImpl(bool autoCreateChildrenModelsOnSelect_)
{
    m_autoCreateChildrenModelsOnSelect = autoCreateChildrenModelsOnSelect_;
}

bool ListModelBaseData::isListLoadedImpl() const
{
    if(getOutputModelImpl()) { return true; }
    return m_listLoaded && !listLoading() && childrenLoaded();
}

void ListModelBaseData::listLoadedVF()
{
}

void ListModelBaseData::itemAddedVF()
{
}

void ListModelBaseData::itemSetVF()
{
}

void ListModelBaseData::itemDeletedVF()
{
}

void ListModelBaseData::errorVF(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
{
    QMLObjectsBase::getInstance().modelError(errorCode_, errorCodeStr_, reply_);
}

void ListModelBaseData::loadChildrenVF()
{
    startListLoad();
}

void ListModelBaseData::objectLoadedVF()
{
    Q_ASSERT(false); // should not be called
}

void ListModelBaseData::addExtraFieldRenameImpl(const QString &oldName_, const QString &newName_)
{
    m_renames.insert(oldName_, newName_);
}

QHash<QString, QVariant> ListModelBaseData::renameFields(const QHash<QString, QVariant> &src_)
{
    QHash<QString, QVariant> res;
    for(QHash<QString, QVariant>::const_iterator it = src_.begin(); it != src_.end(); ++it)
    {
        if(m_renames.contains(it.key()))
        {
            res.insert(m_renames.value(it.key()), it.value());
        }
        else
        {
            res.insert(it.key(), it.value());
        }
    }
    return res;
}

bool ListModelBaseData::getOutputModelImpl() const
{
    return m_outputModel;
}

void ListModelBaseData::setOutputModelImpl(bool outputModel_)
{
    m_outputModel = outputModel_;
}

QStringList ListModelBaseData::getSortFieldsImpl() const
{
    return m_sortFields;
}

void ListModelBaseData::setSortFieldsImpl(const QStringList &sortFields_)
{
    m_sortFields = sortFields_;
}

void ListModelBaseData::startListLoad()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << m_QMLLayoutName << "startListLoad()";
#endif

    m_listLoading = true;
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void ListModelBaseData::setListLoaded()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << m_QMLLayoutName << "setListLoaded()";
#endif

    m_listLoading = false;
    m_listLoaded = true;
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }
    if(isListLoadedImpl())
    {
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << m_QMLLayoutName << "listLoadedVF() ";
#endif
        listLoadedVF();
    }
}

bool ListModelBaseData::listLoading() const
{
    return m_listLoading;
}

bool ListModelBaseData::childrenLoaded() const
{
    return 0 == m_loadingChildenModels;
}

void ListModelBaseData::startLoadChildModel()
{
    ++m_loadingChildenModels;
#if defined(TRACE_MODEL_LOADING)
    qDebug() << m_QMLLayoutName << "startLoadChildModel() " << m_loadingChildenModels;
#endif
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void ListModelBaseData::endLoadChildModel()
{
    --m_loadingChildenModels;
#if defined(TRACE_MODEL_LOADING)
    qDebug() << m_QMLLayoutName << "endLoadChildModel() " << m_loadingChildenModels;
#endif
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }
    Q_ASSERT(m_loadingChildenModels >= 0);
    if(isListLoadedImpl())
    {
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << m_QMLLayoutName << "listLoadedVF() ";
#endif
        listLoadedVF();
    }
}

void ListModelBaseData::setParentListModelInfo(IListModelInfo *parentListModelInfo_)
{
    m_parentListModelInfo = parentListModelInfo_;
}
