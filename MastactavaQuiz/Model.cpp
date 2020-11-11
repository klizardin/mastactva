#include "Model.h"
#include "qmlobjects.h"


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
    IListModel *parentModelPtr = QMLObjects::getInstance().getListModel(parentModel_);
    Q_ASSERT(nullptr != parentModelPtr && nullptr != parentModelPtr->getModel());
    if(nullptr != parentModelPtr && nullptr != parentModelPtr->getModel())
    {
        QObject::connect(parentModelPtr->getModel(), SIGNAL(refreshChildren(QString)), m_modelObj, SLOT(refreshChildrenSlot(QString)));
    }
}

void ListModelBaseData::addLayoutExtraGetFieldsImpl(const QString &modelName_, const QVariant &appId_)
{
    m_extraFields.push_back({modelName_, appId_});
}

void ListModelBaseData::registerListModel()
{
    QMLObjects::getInstance().registerModel(m_model->getQMLLayoutName(), m_model);
    m_autoRegister = true;
}

void ListModelBaseData::unregisterListModel()
{
    QMLObjects::getInstance().unregisterModel(m_model->getQMLLayoutName());
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
    return m_currentIndex >= 0 && m_currentIndex < size_;
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
