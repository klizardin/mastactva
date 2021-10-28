/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Model.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/defines.h"


#if defined(TRACE_MODELS_LOADING)
    static const QStringList g_modelNamesToTrace = TRACE_MODELS_LOADING;
#endif


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

void ListModelBaseData::setLayoutRefImpl(const QString &fieldJsonName_,
                                         const QString &parentModel_,
                                         const QString &parentModelRefJsonName_,
                                         bool notify_ /*= true*/)
{
    m_refs.insert(fieldJsonName_, {fieldJsonName_, parentModel_, parentModelRefJsonName_});
    if(!notify_) { return; }
    IListModel *parentModelPtr = QMLObjectsBase::getInstance().getListModel(parentModel_);
    if(parentModelPtr && parentModelPtr->getModel())
    {
        QObject::connect(parentModelPtr->getModel(), SIGNAL(refreshChildren(QString)),
                         m_modelObj, SLOT(refreshChildrenSlot(QString)));
    }
}

void ListModelBaseData::addLayoutExtraFieldsImpl(const QString &modelName_,
                                                 const QVariant &appId_)
{
    m_extraFields.push_back({modelName_, appId_});
}

void ListModelBaseData::registerListModel()
{
    if(QMLObjectsBase::hasInstance())
    {
        QMLObjectsBase::getInstance().registerModel(m_model->getQMLLayoutName(), m_model);
    }
    m_autoRegister = true;
}

void ListModelBaseData::unregisterListModel()
{
    if(QMLObjectsBase::hasInstance())
    {
        QMLObjectsBase::getInstance().unregisterModel(m_model->getQMLLayoutName());
    }
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
    if(!rd_) { return false; }
    m_requests.push_back(rd_);
    return true;
}

RequestData * ListModelBaseData::findRequest(const QString &requestName_)
{
    const auto fit = std::find_if(std::begin(m_requests),
                                  std::end(m_requests),
                                  [&requestName_](const auto *request_)->bool
    {
        return request_ && request_->getRequestName() == requestName_;
    });
    return std::end(m_requests) == fit ? nullptr : *fit;
}

bool ListModelBaseData::findRequest(RequestData *request_) const
{
    if(!request_) { return false; }
    const auto fit = std::find_if(std::begin(m_requests),
                                  std::end(m_requests),
                                  [request_](const auto *requestItem_)->bool
    {
        return requestItem_ && requestItem_ == request_;
    });
    return std::end(m_requests) != fit;
}

void ListModelBaseData::removeRequest(RequestData *request_)
{
    if(!request_) { return; }
    const auto fit = std::find_if(std::begin(m_requests),
                                  std::end(m_requests),
                                  [request_](const auto *requestItem_)->bool
    {
        return requestItem_ && requestItem_ == request_;
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
    return m_listLoaded
            && !doNeedToReloadList()
            && !listLoading()
            && childrenLoaded()
            ;
}

void ListModelBaseData::listLoadedVF()
{
    if(m_setModelChangeNotify)
    {
        m_setModelChangeNotify->listLoadedVF();
    }
}

void ListModelBaseData::itemAddedVF()
{
    if(m_setModelChangeNotify)
    {
        m_setModelChangeNotify->itemAddedVF();
    }
}

void ListModelBaseData::itemSetVF()
{
    if(m_setModelChangeNotify)
    {
        m_setModelChangeNotify->itemSetVF();
    }
}

void ListModelBaseData::itemDeletedVF()
{
    if(m_setModelChangeNotify)
    {
        m_setModelChangeNotify->itemDeletedVF();
    }
}

void ListModelBaseData::errorVF(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
{
    if(m_setModelChangeNotify)
    {
        m_setModelChangeNotify->errorVF(errorCode_, errorCodeStr_, reply_);
    }
    QMLObjectsBase::getInstance().modelError(errorCode_, errorCodeStr_, reply_);
}

void ListModelBaseData::loadChildrenVF()
{
    startListLoad();
}

void ListModelBaseData::objectLoadedVF()
{
    if(m_setModelChangeNotify)
    {
        m_setModelChangeNotify->objectLoadedVF();
    }
}

void ListModelBaseData::addExtraFieldRenameImpl(const QString &oldName_, const QString &newName_)
{
    m_renames.insert(oldName_, newName_);
}

QHash<QString, QVariant> ListModelBaseData::renameFields(const QHash<QString, QVariant> &src_)
{
    QHash<QString, QVariant> res;
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(src_));
        it != std::cend(qAsConst(src_));
        ++it)
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

bool ListModelBaseData::getReadonlyImpl() const
{
    return m_readonly;
}

void ListModelBaseData::setReadonlyImpl(bool readonly_)
{
    m_readonly = readonly_;
}

bool ListModelBaseData::getStoreAfterSaveBase() const
{
    return m_storeAfterSave;
}

void ListModelBaseData::setStoreAfterSaveBase(bool storeAfterSave_)
{
    m_storeAfterSave = storeAfterSave_;
}

void ListModelBaseData::setModelChangeNotify(IListModelChangeNotify *setModelChangeNotify_)
{
    m_setModelChangeNotify = setModelChangeNotify_;
}

bool ListModelBaseData::doNeedToReloadList() const
{
    return m_needToReloadList;
}

void ListModelBaseData::setNeedToReloadList()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-" << m_QMLLayoutName << "setNeedToReloadList()";
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "setNeedToReloadList()";
    }
#endif

    m_needToReloadList = true;

    if(m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void ListModelBaseData::clearNeedToReloadList()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-" << m_QMLLayoutName << "clearNeedToReloadList()";
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "clearNeedToReloadList()";
    }
#endif

    m_needToReloadList = false;

    if(m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }
}

void ListModelBaseData::startListLoad()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-" << m_QMLLayoutName << "startListLoad()";
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "startListLoad()";
    }
#endif

    m_listLoading = true;

    if(m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void ListModelBaseData::clearListLoaded()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-" << m_QMLLayoutName << "clearListLoaded()";
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "clearListLoaded()";
    }
#endif

    m_listLoaded = false;
}

void ListModelBaseData::reverseListLoading()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-" << m_QMLLayoutName << "reverseListLoading()";
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "reverseListLoading()";
    }
#endif

    m_listLoading = false;

    if(m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }
}

void ListModelBaseData::setListLoaded()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << m_QMLLayoutName << "setListLoaded()";
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "setListLoaded()";
    }
#endif

    m_listLoading = false;
    m_listLoaded = true;

    if(m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }

    if(isListLoadedImpl())
    {
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << m_QMLLayoutName << "setListLoaded(), listLoadedVF()";
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "setListLoaded(), listLoadedVF()";
    }
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
#if defined(TRACE_MODEL_LOADING)
    qDebug() << "-" << m_QMLLayoutName << "startLoadChildModel() " << m_loadingChildenModels << " > " << m_loadingChildenModels + 1;
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "startLoadChildModel() " << m_loadingChildenModels << " > " << m_loadingChildenModels + 1;
    }
#endif

    ++m_loadingChildenModels;
    if(m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void ListModelBaseData::endLoadChildModel()
{
#if defined(TRACE_MODEL_LOADING)
    qDebug() << "-"  << m_QMLLayoutName << "endLoadChildModel() " << m_loadingChildenModels << " > " << m_loadingChildenModels - 1;
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "endLoadChildModel() " << m_loadingChildenModels << " > " << m_loadingChildenModels - 1;
    }
#endif

    --m_loadingChildenModels;

#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    if(m_loadingChildenModels < 0)
    {
        qDebug() << "m_loadingChildenModels < 0" << m_QMLLayoutName << "endLoadChildModel()" << m_loadingChildenModels;
    }
#endif
#if defined(TRACE_MODELS_LOADING)
    if(m_loadingChildenModels < 0 &&
            g_modelNamesToTrace.contains(m_QMLLayoutName)
            )
    {
        qDebug() << "m_loadingChildenModels < 0" << m_QMLLayoutName << "endLoadChildModel()" << m_loadingChildenModels;
    }
#endif

    if(m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }
    Q_ASSERT(m_loadingChildenModels >= 0);
    if(isListLoadedImpl())
    {
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << m_QMLLayoutName << "endLoadChildModel() listLoadedVF()"
            << "m_listLoaded " << m_listLoaded
            << "doNeedToReloadList()" << doNeedToReloadList()
            << "listLoading()" << listLoading()
            << "childrenLoaded()" << childrenLoaded()
            ;
#endif
#if defined(TRACE_MODELS_LOADING)
    if(g_modelNamesToTrace.contains(m_QMLLayoutName))
    {
        qDebug() << "-" << m_QMLLayoutName << "endLoadChildModel() listLoadedVF()"
            << "m_listLoaded " << m_listLoaded
            << "doNeedToReloadList()" << doNeedToReloadList()
            << "listLoading()" << listLoading()
            << "childrenLoaded()" << childrenLoaded()
            ;
    }
#endif
        listLoadedVF();
    }
}

void ListModelBaseData::setParentListModelInfo(IListModelInfo *parentListModelInfo_)
{
    m_parentListModelInfo = parentListModelInfo_;
}
