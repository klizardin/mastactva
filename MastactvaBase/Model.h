#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>
#include <QList>
#include <QHash>
#include <QDebug>
#include <QVariant>
#include "IModel.h"
#include "Layout.h"
#include "netapi.h"
#include "qmlobjects.h"


//#define TRACE_LIST_LOAD_DATA
//#define TRACE_LIST_SIZE
//#define TRACE_LIST_DATA_ITEMS_CRUD


class ListModelBaseData : public IListModelInfo
{
protected:
    class RefDescription
    {
    public:
        QString m_ref;
        QString m_parentModel;
        QString m_parentModelJsonFieldName;
    };

    class ExtraFields
    {
    public:
        QString m_modelName;
        QVariant m_appId;
    };

public:
    ListModelBaseData(IListModel *model_);
    ~ListModelBaseData();

    void setLayoutRefImpl(const QString &fieldJsonName_, const QString &parentModel_, const QString &parentModelRefJsonName_, bool notify_ = true);
    void addLayoutExtraFieldsImpl(const QString &modelName_, const QVariant &appId_);
    void registerListModel();
    void setParentListModelInfo(IListModelInfo *parentListModelInfo_);
    bool autoCreateChildrenModelsImpl() const;
    bool autoCreateChildrenModelsOnSelectImpl() const;
    void addExtraFieldRenameImpl(const QString &oldName_, const QString &newName_);

public:
    virtual void startLoadChildModel() override;
    virtual void endLoadChildModel() override;
    virtual bool isListLoadedImpl() const override;
    virtual void listLoadedVF() override;
    virtual void itemAddedVF() override;
    virtual void itemSetVF() override;
    virtual void itemDeletedVF() override;
    virtual void errorVF(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_) override;
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;

    void parentItemRemoved();

protected:
    void setRefAppIdImpl(const QVariant &appId_);
    QVariant getRefAppIdImpl() const;
    QVariant getRefValueImpl() const;
    void setRefValueImpl(const QVariant &value_);
    int getCurrentIndexImpl() const;
    void setCurrentIndexImpl(int index_);
    bool isCurrentIndexValid(int size_) const;
    bool isIndexValid(int index_, int size_) const;
    const QString &currentRefImpl() const;
    void setCurrentRefImpl(const QString &ref_);
    void setLayoutQMLNameImpl(const QString &layoutQMLName_);
    const QString &getLayoutQMLNameImpl() const;
    bool getJsonParamsGetImpl() const;
    void setJsonParamsGetImpl(bool jsonParamsGet_);
    bool addRequest(RequestData *rd_);
    RequestData * findRequest(const QString &requestName_);
    bool findRequest(RequestData *request_) const;
    void removeRequest(RequestData *request_);
    int getNextAppId(void *dta_);
    int getOutOfRangeAppId() const;
    void setNextAppId(int appId_);
    void init(QObject *modelObj_);
    void addModelParamImpl(const QString &name_, const QVariant &value_);
    void setAutoCreateChildrenModelsImpl(bool autoCreateChildrenModels_);
    void setAutoCreateChildrenModelsOnSelectImpl(bool autoCreateChildrenModelsOnSelect_);
    void startListLoad();
    void setListLoaded();
    bool listLoading() const;
    QHash<QString, QVariant> renameFields(const QHash<QString, QVariant> &src_);
    bool getOutputModelImpl() const;
    void setOutputModelImpl(bool outputModel_);

private:
    void unregisterListModel();
    bool childrenLoaded() const;

protected:
    QHash<QString, RefDescription> m_refs;
    QList<ExtraFields> m_extraFields;
    QHash<QString, QString> m_renames;
    QVector<RequestData *> m_requests;
    QHash<QString, QVariant> m_modelParams;

private:
    QVariant m_refAppId;
    QVariant m_refValue;
    int m_currentIndex = -1;
    QString m_currentRef;
    QString m_QMLLayoutName;
    bool m_jsonParamsGet = false;
    int m_nextAppId = 0;
    QObject *m_modelObj = nullptr;
    bool m_autoRegister = false;
    IListModel *m_model = nullptr;
    bool m_autoCreateChildrenModels = false;
    bool m_autoCreateChildrenModelsOnSelect = false;
    bool m_listLoaded = false;
    bool m_listLoading = false;
    int m_loadingChildenModels = 0;
    bool m_outputModel = false;
    IListModelInfo *m_parentListModelInfo = nullptr;
};


template<class DataType_, class ModelType_>
class ListModelBaseOfData : public QAbstractListModel, public IListModel, public ListModelBaseData
{
public:
    explicit ListModelBaseOfData(QObject *parent_)
        : QAbstractListModel(parent_),
          ListModelBaseData(this)
    {
        getDataLayout<DataType_>().initQMLModelRoleNames(m_roleNames);
    }

    virtual ~ListModelBaseOfData() override
    {
        clearData();
    }

    virtual int rowCount(const QModelIndex &parent_) const override
    {
        Q_UNUSED(parent_);
#if defined(TRACE_LIST_SIZE)
        qDebug() << "m_data.size = " << m_data.size();
#endif
        return m_data.size();
    }

    virtual QVariant data(const QModelIndex &index_, int role_) const override
    {
        const int row = index_.row();
        if(row < 0 || row >= m_data.size()) { return QVariant(); }
        const auto *item = m_data[row];
        return getDataLayout<DataType_>().getModelValue(item, role_);
    }

    virtual bool setData(const QModelIndex &index_, const QVariant &value_, int role_ = Qt::EditRole) override
    {
        const int row = index_.row();
        if(row < 0 || row >= m_data.size()) { return false; }
        auto *item = m_data[row];
        return getDataLayout<DataType_>().setModelValue(item, role_, value_);
    }

    virtual QObject* getModel() const override
    {
        return m_model;
    }

    virtual void initResponse() override
    {
        NetAPI *netAPI = QMLObjectsBase::getInstance().getNetAPI();
        Q_ASSERT(nullptr != netAPI);
        QObject::connect(netAPI, SIGNAL(response(int, const QString &, RequestData *, const QJsonDocument &)), m_model, SLOT(jsonResponseSlot(int, const QString &, RequestData *, const QJsonDocument &)));
    }

    virtual const QString &getQMLLayoutName() const override
    {
        return getLayoutQMLNameImpl();
    }

    virtual const QString &getJsonLayoutName() const override
    {
        return getDataLayout<DataType_>().getLayoutJsonName();
    }

    virtual QString getLayoutIdFiledName() const override
    {
        return getDataLayout<DataType_>().getIdFieldJsonName();
    }

    virtual bool containsAppId(const QVariant &appId_) const override
    {
        return nullptr != findDataItemByAppIdImpl(appId_);
    }

    virtual QVariant getCurrentIndexAppId() const override
    {
        if(!isCurrentIndexValid(m_data.size())) { return QVariant::fromValue(-1); }
        return getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, m_data[getCurrentIndexImpl()]);
    }

    virtual bool getValuesForAppId(const QVariant &appId_, QHash<QString, QVariant> &values_) const override
    {
        const DataType_ *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(nullptr == item)
        {
            if(!isCurrentIndexValid(m_data.size())) { return false; }
            item = m_data[getCurrentIndexImpl()];
        }
        return getDataLayout<DataType_>().getJsonValues(item, values_);
    }

    virtual QVariant getIdFieldValueForAppId(const QVariant &appId_) const override
    {
        const DataType_ *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(nullptr == item)
        {
            if(!isCurrentIndexValid(m_data.size())) { return QVariant(); }
            item = m_data[getCurrentIndexImpl()];
        }
        return getDataLayout<DataType_>().getIdJsonValue(item);
    }

    virtual QVariant getFieldValueForAppId(const QVariant &appId_, const QString &jsonFieldName) const override
    {
        const DataType_ *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(nullptr == item)
        {
            if(!isCurrentIndexValid(m_data.size())) { return QVariant(); }
            item = m_data[getCurrentIndexImpl()];
        }
        return getDataLayout<DataType_>().getJsonValue(item, jsonFieldName);
    }

    const DataType_ *findDataItemByIdImpl(const QVariant &id_) const
    {
        const auto fit = std::find_if(std::begin(m_data),
                                      std::end(m_data),
                                      [&id_](const DataType_ *item)->bool
        {
            if(nullptr == item) { return false; }
            const QVariant id = getDataLayout<DataType_>().getIdJsonValue(item);
            return id == id_;
        });
        return std::end(m_data) == fit ? nullptr : *fit;
    }

    DataType_ *findDataItemByIdImpl(const QVariant &id_)
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_, ModelType_>*>(this)->findDataItemByIdImpl(id_));
    }

    QVariant findItemByIdImpl(const QVariant &id_)
    {
        return QVariant::fromValue(static_cast<QObject *>(findDataItemByIdImpl(id_)));
    }

    const DataType_ *findDataItemByAppIdImpl(const QVariant &appId_) const
    {
        QVariant appId1 = appId_;
        if(!appId1.isValid())
        {
            appId1 = getCurrentIndexAppId();
        }
        const auto fit = std::find_if(std::begin(m_data),
                                      std::end(m_data),
                                      [&appId1](const DataType_ *item)->bool
        {
            if(nullptr == item) { return false; }
            const QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
            return appId1.isValid() && appId1 == appId;
        });
        return std::end(m_data) == fit ? nullptr : *fit;
    }

    bool selectDataItemByAppIdImpl(const QVariant &appId_)
    {
        if(!appId_.isValid() || appId_.isNull()) { return false; }
        const auto fit = std::find_if(std::begin(m_data),
                                      std::end(m_data),
                                      [&appId_](const DataType_ *item)->bool
        {
            if(nullptr == item) { return false; }
            const QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
            return appId_.isValid() && appId_ == appId;
        });
        if(std::end(m_data) == fit) { return false; }
        const int ni = std::distance(std::begin(m_data), fit);
        if(ni == getCurrentIndexImpl()) { return false; }
        setCurrentIndexImpl(ni);
        autoLoadDataItem();
        return true;
    }

    bool selectDataItemByIdImpl(const QVariant &id_)
    {
        if(!id_.isValid() || id_.isNull()) { return false; }
        const auto fit = std::find_if(std::begin(m_data),
                                      std::end(m_data),
                                      [&id_](const DataType_ *item)->bool
        {
            if(nullptr == item) { return false; }
            const QVariant id = getDataLayout<DataType_>().getIdJsonValue(item);
            return id_.isValid() && id_ == id;
        });
        if(std::end(m_data) == fit) { return false; }
        const int ni = std::distance(std::begin(m_data), fit);
        if(ni == getCurrentIndexImpl()) { return false; }
        setCurrentIndexImpl(ni);
        autoLoadDataItem();
        return true;
    }

    const DataType_ *dataItemAtImpl(int index_) const
    {
        if(!isIndexValid(index_, m_data.size())) { return nullptr; }
        return m_data[index_];
    }

    DataType_ *dataItemAtImpl(int index_)
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_, ModelType_> *>(this)->dataItemAtImpl(index_));
    }

    DataType_ *findDataItemByAppIdImpl(const QVariant &appId_)
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_, ModelType_> *>(this)->findDataItemByAppIdImpl(appId_));
    }

    QVariant findItemByAppIdImpl(const QVariant &appId_)
    {
        return QVariant::fromValue(static_cast<QObject *>(findDataItemByAppIdImpl(appId_)));
    }

    bool selectItemByAppIdImpl(const QVariant &appId_)
    {
        return selectDataItemByAppIdImpl(appId_);
    }

    bool selectItemByIdImpl(const QVariant &id_)
    {
        return selectDataItemByIdImpl(id_);
    }

    const DataType_ *getCurrentDataItem() const
    {
        return findDataItemByAppIdImpl(QVariant());
    }

    DataType_ *getCurrentDataItem()
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_, ModelType_> *>(this)->getCurrentDataItem());
    }

    void parentCurrentIndexChanged()
    {
        loadListImpl();
    }

    void parentItemChanged()
    {
        loadListImpl();
    }

    QVariant getItemIdImpl(int index_) const
    {
        if(!isIndexValid(index_, m_data.size())) { return QVariant(); }
        const DataType_* item = m_data.at(index_);
        return getDataLayout<DataType_>().getIdJsonValue(item);
    }

    QVariant getItemAppIdImpl(int index_) const
    {
        if(!isIndexValid(index_, m_data.size())) { return QVariant(); }
        const DataType_* item = m_data.at(index_);
        return getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
    }

    QVariant itemAtImpl(int index_) const
    {
        return QVariant::fromValue(
                    static_cast<QObject *>(
                        const_cast<ListModelBaseOfData<DataType_, ModelType_> *>(this)
                        ->dataItemAtImpl(index_)
                        )
                    );
    }

    int sizeImpl() const
    {
        return m_data.size();
    }

    template<typename Op_>
    DataType_ *dataItemFindIf(Op_ op_)
    {
        const int count = sizeImpl();
        for(int i = 0; i < count; i++)
        {
            if(op_(m_data[i])) { return m_data[i]; }
        }
        return nullptr;
    }

    DataType_ *findDataItemByFieldValueImpl(const QString &qmlFieldName_, const QVariant &value_)
    {
        return dataItemFindIf([&qmlFieldName_, &value_](const DataType_ *item_)->bool
        {
            return getDataLayout<DataType_>().getQMLValue(item_, qmlFieldName_) == value_;
        });
    }

    QVariant findItemByFieldValueImpl(const QString &qmlFieldName_, const QVariant &value_) const
    {
        return QVariant::fromValue(
                    static_cast<QObject *>(
                        const_cast<ListModelBaseOfData<DataType_, ModelType_> *>(this)
                        ->findDataItemByFieldValueImpl(qmlFieldName_,value_)
                        )
                    );
    }

    template<typename Op_>
    int indexFindIf(Op_ op_) const
    {
        const int count = sizeImpl();
        for(int i = 0; i < count; i++)
        {
            if(op_(m_data[i])) { return i; }
        }
        return -1;
    }

    int indexOfDataItemImpl(const DataType_ *item_) const
    {
        const QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
        return indexFindIf([&appId](const DataType_ *i)->bool
        {
            const QVariant iAppId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, i);
            return appId.isValid() && iAppId.isValid() && appId == iAppId;
        });
    }

    DataType_ *createDataItemImpl()
    {
        DataType_ *dta = new DataType_(m_model);
        getDataLayout<DataType_>().setSpecialFieldValue(
                    layout::SpecialFieldEn::appId,
                    QVariant::fromValue(getNextAppId(dta)),
                    dta);
        getDataLayout<DataType_>().setSpecialFieldValue(
                    layout::SpecialFieldEn::modelInfo,
                    QVariant::fromValue(static_cast<IListModelInfo *>(this)),
                    dta);
        return dta;
    }

    bool setDataItemImpl(int index_, DataType_ *item_)
    {
        if(index_ < 0 || index_ >= m_data.size()) { return false; }

        NetAPI *netAPI = QMLObjectsBase::getInstance().getNetAPI();
        if(nullptr == netAPI) { return false; }

        if(getJsonLayoutName().isEmpty())
        {
            QVariant itemAppId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
            QVariant itemId = getDataLayout<DataType_>().getIdJsonValue(item_);
            RequestData *request = netAPI->emptyRequest(netAPI->setItemRequestName<DataType_>(), itemAppId, itemId);
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, QString(), request, QJsonDocument());
            }
            netAPI->freeRequest(request);
            return true;
        }
        else
        {
            QHash<QString, QVariant> extraFields(m_modelParams);
            for(const ExtraFields &f: m_extraFields)
            {
                IListModel *m = QMLObjectsBase::getInstance().getListModel(f.m_modelName);
                if(nullptr != m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            RequestData *request = netAPI->setItem(getJsonLayoutName(), item_, extraFields);
            return addRequest(request);
        }
    }

    bool addDataItemImpl(DataType_ *item_, bool setCurrentIndex_ = false)
    {
        NetAPI *netAPI = QMLObjectsBase::getInstance().getNetAPI();
        if(nullptr == netAPI) { return false; }

        if(getJsonLayoutName().isEmpty())
        {
            QVariant itemAppId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
            QVariant itemId = getDataLayout<DataType_>().getIdJsonValue(item_);
            RequestData *request = netAPI->emptyRequest(netAPI->addItemRequestName<DataType_>(), itemAppId, itemId);
            if(nullptr != request)
            {
                request->setItemData(reinterpret_cast<void *>(item_));
                request->setSetCurrentItemIndex(setCurrentIndex_);
            }
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, QString(), request, QJsonDocument());
            }
            netAPI->freeRequest(request);
            return true;
        }
        else
        {
            QHash<QString, QVariant> extraFields(m_modelParams);
            for(const ExtraFields &f: m_extraFields)
            {
                IListModel *m = QMLObjectsBase::getInstance().getListModel(f.m_modelName);
                if(nullptr != m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            RequestData *request = netAPI->addItem(getJsonLayoutName(), item_, extraFields);
            if(nullptr != request)
            {
                request->setItemData(reinterpret_cast<void *>(item_));
                request->setSetCurrentItemIndex(setCurrentIndex_);
            }
            return addRequest(request);
        }
    }

    bool delDataItemImpl(int index_)
    {
        if(!isIndexValid(index_, m_data.size())) { return false; }
        return delDataItemImpl(m_data.at(index_));

    }

    bool delDataItemImpl(DataType_ *item_)
    {
        NetAPI *netAPI = QMLObjectsBase::getInstance().getNetAPI();
        if(nullptr == netAPI) { return false; }

        QVariant itemId = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!itemId.isValid() || itemId.isNull()) { return false; }

        RequestData *request = netAPI->delItem(getJsonLayoutName(), item_);
        return addRequest(request);
    }

    void setLayoutIdFieldImpl(const QString &fieldJsonName_)
    {
        setDataLayout<DataType_>().setIdField(fieldJsonName_);
    }

    void autoLoadDataItemImpl(const DataType_ *item_)
    {
        if(nullptr == item_) { return; }
        const QVariant modelInfoVar = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::objectModelInfo, item_);
        IListModelInfo *modelInfo = nullptr;
        layout::setValue(modelInfoVar, modelInfo);
        if(nullptr != modelInfo)
        {
            modelInfo->loadChildrenVF();
        }
        getDataLayout<DataType_>().createQMLValues(item_);
        if(nullptr != modelInfo)
        {
            modelInfo->objectLoadedVF();
        }
    }

protected:
    bool storeAfterSaveImpl() const
    {
        return getDataLayout<DataType_>().storeAfterSave();
    }

    void setStoreAfterSaveImpl(bool storeAfterSave_)
    {
        setDataLayout<DataType_>().setStoreAfterSave(storeAfterSave_);
    }

    void loadListImpl()
    {
        NetAPI *netAPI = QMLObjectsBase::getInstance().getNetAPI();
        if(nullptr == netAPI) { return; }
        RequestData *request = findRequest(netAPI->getListRequestName<DataType_>());
        if(nullptr != request)
        {
            reloadList = true;
            return;
        }
        startListLoad();
        if(getJsonLayoutName().isEmpty())
        {
            request = netAPI->emptyRequest(netAPI->getListRequestName<DataType_>(), QVariant() , QVariant());
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, QString(), request, QJsonDocument());
            }
            netAPI->freeRequest(request);
        }
        else
        {
            QString parentModel;
            QString parentModelJsonFieldName;
            if(m_refs.contains(currentRefImpl()))
            {
                parentModel = m_refs.value(currentRefImpl()).m_parentModel;
                parentModelJsonFieldName = m_refs.value(currentRefImpl()).m_parentModelJsonFieldName;
            }
            QHash<QString, QVariant> extraFields(m_modelParams);
            for(const ExtraFields &f: m_extraFields)
            {
                IListModel *m = QMLObjectsBase::getInstance().getListModel(f.m_modelName);
                if(nullptr != m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            request = netAPI->getList<DataType_>(
                        getJsonLayoutName(),
                        currentRefImpl(),
                        parentModel,
                        parentModelJsonFieldName,
                        getRefAppIdImpl(),
                        getRefValueImpl(),
                        getJsonParamsGetImpl(),
                        extraFields
                        );
            if(!addRequest(request))
            {
                setListLoaded();
            }
        }
    }

    QVariant createItemImpl()
    {
        return QVariant::fromValue(createDataItemImpl());
    }

    bool setItemImpl(int index_, const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataType_ *dataItem = qobject_cast<DataType_ *>(obj);
        if(nullptr == dataItem) { return false; }
        return setDataItemImpl(index_, dataItem);
    }

    bool addItemImpl(const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataType_ *dataItem = qobject_cast<DataType_ *>(obj);
        if(nullptr == dataItem) { return false; }
        return addDataItemImpl(dataItem);
    }

    bool delItemImpl(int index_)
    {
        return delDataItemImpl(index_);
    }

    bool delItemImpl(const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataType_ *dataItem = qobject_cast<DataType_ *>(obj);
        if(nullptr == dataItem) { return false; }
        return delDataItemImpl(dataItem);
    }

    int indexOfItemImpl(const QVariant &item_) const
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataType_ *dataItem = qobject_cast<DataType_ *>(obj);
        if(nullptr == dataItem) { return -1; }
        return indexOfDataItemImpl(dataItem);
    }

    void setLayoutJsonNameImpl(const QString &layoutJsonName_)
    {
        setDataLayout<DataType_>().setLayoutJsonName(layoutJsonName_);
    }

    const QString &getLayoutJsonNameImpl()
    {
        return getDataLayout<DataType_>().getLayoutJsonName();
    }

    QString getLayoutIdFieldImpl()
    {
        return getDataLayout<DataType_>().getIdFieldJsonName();
    }

    void jsonResponseSlotImpl(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_)
    {
        if(!findRequest(request_)) { return; }
        NetAPI *netAPI = QMLObjectsBase::getInstance().getNetAPI();
        if(nullptr == netAPI)
        {
            removeRequest(request_);
            return;
        }
        if(0 != errorCode_ && (200 > errorCode_ || 300 <= errorCode_))
        {
            modelError(errorCode_, errorCodeStr_, reply_);
            DataType_ *itemToDel = reinterpret_cast<DataType_ *>(request_->getItemData());
            delete itemToDel;
            itemToDel = nullptr;
            request_->setItemData(nullptr);
        }
        else if(request_->getRequestName() == netAPI->getListRequestName<DataType_>())
        {
            modelListLoaded(reply_);
            if(reloadList)
            {
                reloadList = false;
                loadListImpl();
            }
        }
        else if(request_->getRequestName() == netAPI->addItemRequestName<DataType_>())
        {
            modelItemAdded(request_, reply_);
        }
        else if(request_->getRequestName() == netAPI->setItemRequestName<DataType_>())
        {
            modelItemSet(request_, reply_);
        }
        else if(request_->getRequestName() == netAPI->delItemRequestName<DataType_>())
        {
            modelItemDeleted(request_, reply_);
        }
        removeRequest(request_);
        clearTempData();
        if(nullptr != m_model)
        {
            m_model->startRefreshChildren(getQMLLayoutName());
        }
    }

    bool init(ModelType_ *model_)
    {
        m_model = model_;
        ListModelBaseData::init(model_);
        return true;
    }

    void setCurrentIndexByAppIdImpl(const QVariant &appId_)
    {
        const auto fit = std::find_if(std::begin(m_data),
                                      std::end(m_data),
                                      [&appId_](const DataType_ *item)->bool
        {
            if(nullptr == item) { return false; }
            const QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
            return appId_.isValid() && appId_ == appId;
        });
        if(std::end(m_data) == fit)
        {
            setCurrentIndexImpl(m_data.isEmpty()? -1: 0);
            autoLoadDataItem();
        }
        else
        {
            setCurrentIndexImpl(std::distance(std::begin(m_data), fit));
            autoLoadDataItem();
        }
    }

    template<typename Op_>
    void filterItemsIf(Op_ op_)
    {
        QVector<const DataType_ *> toRemove;
        toRemove.reserve(m_data.size());
        for(const DataType_ * item: m_data)
        {
            if(!op_(item)) { toRemove.push_back(item); }
        }
        for(const DataType_ * item: toRemove)
        {
            removeItem(item);
        }
    }

    bool filterItemsLeftIDsImpl(const QVariantList &ids_)
    {
        filterItemsIf([&ids_](const DataType_ *i)->bool
        {
            return nullptr != i &&
                    std::end(ids_) != std::find_if(std::begin(ids_), std::end(ids_),
                                                [&i](const QVariant &id_)
            {
                const QVariant id = getDataLayout<DataType_>().getIdJsonValue(i);
                return id_.isValid() && id.isValid() && id_ == id;
            });
        });
        return !m_data.isEmpty();
    }

    bool filterItemsLeftIDsIntImpl(const QVector<int> &ids_)
    {
        QVariantList ids;
        convertIds(ids_, ids);
        return filterItemsLeftIDsImpl(ids);
    }

    bool filterItemsLeftExceptIDsImpl(const QVariantList &ids_)
    {
        filterItemsIf([&ids_](const DataType_ *i)->bool
        {
            return nullptr != i &&
                    std::end(ids_) == std::find_if(std::begin(ids_), std::end(ids_),
                                                [&i](const QVariant &id_)
            {
                const QVariant id = getDataLayout<DataType_>().getIdJsonValue(i);
                return id_.isValid() && id.isValid() && id_ == id;
            });
        });
        return !m_data.isEmpty();
    }

    bool filterItemsLeftExceptIDsIntImpl(const QVector<int> &ids_)
    {
        QVariantList ids;
        convertIds(ids_, ids);
        return filterItemsLeftExceptIDsImpl(ids);
    }

protected:
    void convertIds(const QVector<int> &idsInt_, QVariantList &idsVariant_)
    {
        idsVariant_.clear();
        idsVariant_.reserve(idsInt_.size());
        for(int id : idsInt_)
        {
            idsVariant_.push_back(id);
        }
    }

    virtual void modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
    {
        errorVF(errorCode_, errorCodeStr_, reply_);
    }

    virtual void modelListLoaded(const QJsonDocument &reply_)
    {
        const QVariant oldCurrentAppId = getCurrentIndexAppId();
        const QVariant oldCurrentId = getIdFieldValueForAppId(oldCurrentAppId);
        QList<DataType_ *> loaded;
        if(!reply_.isEmpty())
        {
            for(int i = 0; ; i++)
            {
                QJsonValue itemJV = reply_[i];
                if(itemJV.isUndefined()) { break; }
                DataType_ *item = createDataItemImpl();
                const bool ok = getDataLayout<DataType_>().setJsonValues(item, itemJV);
                if(!ok)
                {
                    delete item;
                    continue;
                }
                const QVariant id = getDataLayout<DataType_>().getIdJsonValue(item);
                DataType_ *oldItem = findDataItemByIdImpl(id);
                if(nullptr != oldItem)
                {
                    const QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
                    Q_ASSERT(appId.isValid());
                    getDataLayout<DataType_>().setSpecialFieldValue(layout::SpecialFieldEn::appId, appId, item);
                }
                loaded.push_back(item);
            }
        }
        clearData();
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "modelListLoaded() beginInsertRows(" << m_data.size() << "," << std::max(0, m_data.size() + loaded.size() - 1) << ")";
#endif
        beginInsertRows(QModelIndex(), m_data.size(), std::max(0, m_data.size() + loaded.size() - 1));
        std::copy(std::begin(loaded), std::end(loaded),
                  std::inserter(m_data, std::end(m_data)));
        endInsertRows();
        DataType_ *oldIdItem = findDataItemByIdImpl(oldCurrentId);
        if(nullptr == oldIdItem)
        {
            setCurrentIndexImpl(0);
            autoLoadDataItem();
        }
        else
        {
            setCurrentIndexByAppIdImpl(oldCurrentAppId);
            autoLoadDataItem();
        }
        loaded.clear();
        for(DataType_ *item : m_data)
        {
            autoLoadDataItem(item);
        }
        setListLoaded();
#if defined(TRACE_LIST_LOAD_DATA)
        for(const DataType_ *i: m_data)
        {
            QHash<QString, QVariant> values;
            getDataLayout<DataType_>().getJsonValues(i, values);
            qDebug() << values;
        }
#endif
    }

    virtual void modelItemAdded(RequestData *request_, const QJsonDocument &reply_)
    {
        if(nullptr == request_->getItemData()) { return; }
        const QVariant appId = request_->getItemAppId();

#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "modelItemAdded() beginInsertRows(" << m_data.size() << "," << m_data.size() << ")";
#endif
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.push_back(reinterpret_cast<DataType_ *>(request_->getItemData()));
        endInsertRows();

        request_->setItemData(nullptr);

        DataType_ *item = findDataItemByAppIdImpl(appId);
        if(nullptr == item) { return; }

        getDataLayout<DataType_>().setJsonValues(item, reply_);
        autoLoadDataItem(item);
        if(request_->getSetCurrentItemIndex())
        {
            setCurrentIndexImpl(m_data.size() - 1);
        }
        itemAddedVF();
    }

    virtual void modelItemSet(RequestData *request_, const QJsonDocument &reply_)
    {
        const QVariant id = request_->getItemId();
        DataType_ *item = findDataItemByIdImpl(id);
        if(nullptr == item) { return; }
        getDataLayout<DataType_>().setJsonValues(item, reply_);
        autoLoadDataItem(item);
        const int itemIndex = indexOfDataItemImpl(item);
        QModelIndex changedIndex0 = createIndex(itemIndex, 0);
        QModelIndex changedIndex1 = createIndex(itemIndex, m_roleNames.size());
        emit dataChanged(changedIndex0, changedIndex1);
        itemSetVF();
    }

    virtual void modelItemDeleted(RequestData *request_, const QJsonDocument &reply_)
    {
        Q_UNUSED(reply_);
        const QVariant id = request_->getItemId();
        DataType_ *item = findDataItemByIdImpl(id);
        if(nullptr == item) { return; }
        removeItem(item);
        itemDeletedVF();
    }

    void refreshChildrenSlotImpl(const QString &modelName_)
    {
        Q_ASSERT(m_refs.contains(currentRefImpl()));
        if(!m_refs.contains(currentRefImpl())) { return; }
        const RefDescription &ref = m_refs.value(currentRefImpl());
        if(modelName_ != ref.m_parentModel) { return; }
        IListModel* parentModel = QMLObjectsBase::getInstance().getListModel(ref.m_parentModel);
        const QVariant &refAppId = getRefAppIdImpl();
        if(refAppId.isValid())
        {
            if(parentModel->containsAppId(refAppId))
            {
                parentItemChanged();
            }
            else
            {
                parentItemRemoved();
            }
        }
        else
        {
            parentCurrentIndexChanged();
        }
    }

protected:
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

    void autoLoadDataItem(const DataType_ *item_ = nullptr)
    {
        if(
            (
                nullptr != item_ &&
                autoCreateChildrenModelsImpl()
            ) ||
            (
                autoCreateChildrenModelsOnSelectImpl() &&
                getCurrentIndexImpl() >= 0 &&
                isIndexValid(getCurrentIndexImpl(), m_data.size())
             ))
        {
            if(nullptr == item_)
            {
                item_ = m_data[getCurrentIndexImpl()];
            }
            autoLoadDataItemImpl(item_);
        }
    }

    void autoLoadItemImpl(const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataType_ *dataItem = qobject_cast<DataType_ *>(obj);
        if(nullptr == dataItem) { return; }
        autoLoadDataItemImpl(dataItem);
    }

    void clearData()
    {
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "clearData() beginRemoveRows(" << 0 << "," << std::max(0, m_data.size() - 1) << ")";
#endif
        beginRemoveRows(QModelIndex(), 0, std::max(0, m_data.size() - 1));
        for(auto *&p: m_data)
        {
            delete p;
            p = nullptr;
        }
        m_data.clear();
        endRemoveRows();
    }

    void removeItem(const DataType_ *item_)
    {
        QVariant appId0 = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
        if(!appId0.isValid() || appId0.isNull()) { return; }
        const auto fit = std::find_if(std::begin(m_data), std::end(m_data),
                                      [&appId0](const DataType_ *i_) -> bool
        {
            QVariant appId1 = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, i_);
            return appId0 == appId1;
        });
        if(std::end(m_data) == fit) { return; }
        const int index = std::distance(std::begin(m_data), fit);
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "removeItem() beginRemoveRows(" << index << "," << index << ")";
#endif
        beginRemoveRows(QModelIndex(), index, index);
        delete *fit;
        *fit = nullptr;
        m_data.erase(fit);
        endRemoveRows();
    }

    void clearTempData()
    {
        if(getDataLayout<DataType_>().storeAfterSave()) { return; }

        NetAPI *netAPI = QMLObjectsBase::getInstance().getNetAPI();
        if(nullptr == netAPI) { return; }

        QVector<DataType_ *> waitingToUpdate;
        for(const RequestData *r : m_requests)
        {
            DataType_ *item = nullptr;
            if(r->getRequestName() == netAPI->addItemRequestName<DataType_>())
            {
                const QVariant appId = r->getItemAppId();
                item = findDataItemByAppIdImpl(appId);
            }
            else if(r->getRequestName() == netAPI->setItemRequestName<DataType_>())
            {
                const QVariant id = r->getItemId();
                item = findDataItemByIdImpl(id);
            }
            if(nullptr != item) { waitingToUpdate.push_back(item); }
        }
        if(waitingToUpdate.size() == m_data.size()) { return; }
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "clearTempData() beginRemoveRows(" << 0 << "," << std::max(0, m_data.size() - 1) << ")";
#endif
        beginRemoveRows(QModelIndex(), 0, std::max(0, m_data.size() - 1));
        for(auto *&p: m_data)
        {
            const auto fit = std::find(std::begin(waitingToUpdate), std::end(waitingToUpdate), p);
            if(std::end(waitingToUpdate) == fit)
            {
                delete p;
                p = nullptr;
            }
        }
        m_data.clear();
        endRemoveRows();
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "clearTempData() beginInsertRows(" << 0 << "," << std::max(0, waitingToUpdate.size() - 1) << ")";
#endif
        beginInsertRows(QModelIndex(), 0, std::max(0, waitingToUpdate.size() - 1));
        std::copy(std::begin(waitingToUpdate), std::end(waitingToUpdate),
                  std::inserter(m_data, std::end(m_data)));
        endInsertRows();
    }

protected:
    QVector<DataType_ *> m_data;

private:
    QHash<int, QByteArray> m_roleNames;
    bool reloadList = false;
    ModelType_ *m_model = nullptr;
};


#define LAYOUT_MODEL_IMPL()                                                                                     \
public:                                                                                                         \
    /*properties*/                                                                                              \
    Q_PROPERTY(QString layoutQMLName READ getLayoutQMLName WRITE setLayoutQMLName NOTIFY layoutQMLNameChanged)  \
    Q_PROPERTY(QString layoutIdField READ getLayoutIdField WRITE setLayoutIdField NOTIFY layoutIdFieldChanged)  \
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)             \
    Q_PROPERTY(QVariant currentItem READ getCurrentItem NOTIFY currentItemChanged)                              \
    Q_PROPERTY(QString currentRef READ currentRef WRITE setCurrentRef NOTIFY currentRefChanged)                 \
    Q_PROPERTY(QVariant refAppId READ getRefAppId WRITE setRefAppId NOTIFY refAppIdChanged)                     \
    Q_PROPERTY(QVariant refValue READ getRefValue WRITE setRefValue NOTIFY refValueChanged)                     \
    Q_PROPERTY(bool storeAfterSave READ storeAfterSave WRITE setStoreAfterSave NOTIFY storeAfterSaveChanged)    \
    Q_PROPERTY(bool jsonParamsGet READ jsonParamsGet WRITE setJsonParamsGet NOTIFY jsonParamsGetChanged)        \
    Q_PROPERTY(bool autoCreateChildrenModels READ autoCreateChildrenModels WRITE setAutoCreateChildrenModels NOTIFY autoCreateChildrenModelsChanged)    \
    Q_PROPERTY(bool autoCreateChildrenModelsOnSelect READ autoCreateChildrenModelsOnSelect WRITE setAutoCreateChildrenModelsOnSelect NOTIFY autoCreateChildrenModelsOnSelectChanged)    \
    Q_PROPERTY(bool outputModel READ outputModel WRITE setOutputModel NOTIFY outputModelChanged)                \
    /*Q_INVOKABLEs*/                                                                                            \
    Q_INVOKABLE void setLayoutRef(const QString &fieldJsonName_, const QString &parentModel_, const QString &parentModelRefJsonName_)   \
    {                                                                                                           \
        setLayoutRefImpl(fieldJsonName_, parentModel_, parentModelRefJsonName_);                                \
    }                                                                                                           \
    Q_INVOKABLE QVariant createItem()                                                                           \
    {                                                                                                           \
        return createItemImpl();                                                                                \
    }                                                                                                           \
    Q_INVOKABLE QVariant findItemById(const QVariant &id_)                                                      \
    {                                                                                                           \
        return findItemByIdImpl(id_);                                                                           \
    }                                                                                                           \
    Q_INVOKABLE QVariant findItemByAppId(const QVariant &appId_)                                                \
    {                                                                                                           \
        return findItemByAppIdImpl(appId_);                                                                     \
    }                                                                                                           \
    Q_INVOKABLE QVariant getCurrentItem()                                                                       \
    {                                                                                                           \
        return findItemByAppIdImpl(QVariant());                                                                 \
    }                                                                                                           \
    Q_INVOKABLE void loadList()                                                                                 \
    {                                                                                                           \
        loadListImpl();                                                                                         \
    }                                                                                                           \
    Q_INVOKABLE QVariant getItemId(int index_)                                                                  \
    {                                                                                                           \
        return getItemIdImpl(index_);                                                                           \
    }                                                                                                           \
    Q_INVOKABLE QVariant getItemAppId(int index_)                                                               \
    {                                                                                                           \
        return getItemAppIdImpl(index_);                                                                        \
    }                                                                                                           \
    Q_INVOKABLE QVariant itemAt(int index_)                                                                     \
    {                                                                                                           \
        return itemAtImpl(index_);                                                                              \
    }                                                                                                           \
    Q_INVOKABLE bool selectItemByAppId(const QVariant &appId_)                                                  \
    {                                                                                                           \
        const bool ret = selectItemByAppIdImpl(appId_);                                                         \
        if(ret) { emit currentIndexChanged(); emit currentItemChanged(); }                                      \
        return ret;                                                                                             \
    }                                                                                                           \
    Q_INVOKABLE bool selectItemById(const QVariant &id_)                                                        \
    {                                                                                                           \
        const bool ret = selectItemByIdImpl(id_);                                                               \
        if(ret) { emit currentIndexChanged(); emit currentItemChanged(); }                                      \
        return ret;                                                                                             \
    }                                                                                                           \
    Q_INVOKABLE bool setItem(int index_, const QVariant &item_)                                                 \
    {                                                                                                           \
        return setItemImpl(index_, item_);                                                                      \
    }                                                                                                           \
    Q_INVOKABLE bool addItem(const QVariant &item_)                                                             \
    {                                                                                                           \
        return addItemImpl(item_);                                                                              \
    }                                                                                                           \
    Q_INVOKABLE bool delItem(const QVariant &item_)                                                             \
    {                                                                                                           \
        return delItemImpl(item_);                                                                              \
    }                                                                                                           \
    Q_INVOKABLE bool delItem(const int &index_)                                                                 \
    {                                                                                                           \
        return delItemImpl(index_);                                                                             \
    }                                                                                                           \
    Q_INVOKABLE void addModelParam(const QString &name_, const QVariant &value_)                                \
    {                                                                                                           \
        addModelParamImpl(name_, value_);                                                                       \
    }                                                                                                           \
    Q_INVOKABLE bool isListLoaded() const                                                                       \
    {                                                                                                           \
        return isListLoadedImpl();                                                                              \
    }                                                                                                           \
    Q_INVOKABLE bool isEmpty() const                                                                            \
    {                                                                                                           \
        return m_data.isEmpty();                                                                                \
    }                                                                                                           \
    Q_INVOKABLE void addLayoutExtraFields(const QString &modelName_, const QVariant &appId_)                    \
    {                                                                                                           \
        addLayoutExtraFieldsImpl(modelName_, appId_);                                                           \
    }                                                                                                           \
    Q_INVOKABLE void addExtraFieldRename(const QString &oldName_, const QString &newName_)                      \
    {                                                                                                           \
        addExtraFieldRenameImpl(oldName_, newName_);                                                            \
    }                                                                                                           \
    Q_INVOKABLE int size()                                                                                      \
    {                                                                                                           \
        return sizeImpl();                                                                                      \
    }                                                                                                           \
    Q_INVOKABLE QVariant findItemByFieldValue(const QString &qmlFieldName_, const QVariant &value_) const       \
    {                                                                                                           \
        return findItemByFieldValueImpl(qmlFieldName_, value_);                                                 \
    }                                                                                                           \
    Q_INVOKABLE int indexOfItem(const QVariant &item_)                                                          \
    {                                                                                                           \
        return indexOfItemImpl(item_);                                                                          \
    }                                                                                                           \
    Q_INVOKABLE bool filterItemsLeftIDs(const QVariantList &ids_)                                               \
    {                                                                                                           \
        return filterItemsLeftIDsImpl(ids_);                                                                    \
    }                                                                                                           \
    Q_INVOKABLE bool filterItemsLeftExceptIDs(const QVariantList &ids_)                                         \
    {                                                                                                           \
        return filterItemsLeftExceptIDsImpl(ids_);                                                              \
    }                                                                                                           \
    Q_INVOKABLE void autoLoadItem(const QVariant &item_)                                                        \
    {                                                                                                           \
        autoLoadItemImpl(item_);                                                                                \
    }                                                                                                           \
    /*property's functions*/                                                                                    \
    const QString &getLayoutQMLName()                                                                           \
    {                                                                                                           \
        return getLayoutQMLNameImpl();                                                                          \
    }                                                                                                           \
    void setLayoutQMLName(const QString &layoutQMLName_)                                                        \
    {                                                                                                           \
        setLayoutQMLNameImpl(layoutQMLName_);                                                                   \
        emit layoutQMLNameChanged();                                                                            \
    }                                                                                                           \
    QString getLayoutIdField()                                                                                  \
    {                                                                                                           \
        return getLayoutIdFieldImpl();                                                                          \
    }                                                                                                           \
    void setLayoutIdField(const QString &fieldJsonName_)                                                        \
    {                                                                                                           \
        setLayoutIdFieldImpl(fieldJsonName_);                                                                   \
        emit layoutIdFieldChanged();                                                                            \
    }                                                                                                           \
    int currentIndex() const                                                                                    \
    {                                                                                                           \
        return getCurrentIndexImpl();                                                                           \
    }                                                                                                           \
    void setCurrentIndex(int index_)                                                                            \
    {                                                                                                           \
        setCurrentIndexImpl(index_);                                                                            \
        autoLoadDataItem();                                                                                     \
        emit currentIndexChanged();                                                                             \
        emit currentItemChanged();                                                                              \
    }                                                                                                           \
    QString currentRef() const                                                                                  \
    {                                                                                                           \
        return currentRefImpl();                                                                                \
    }                                                                                                           \
    void setCurrentRef(const QString &ref_)                                                                     \
    {                                                                                                           \
        setCurrentRefImpl(ref_);                                                                                \
        emit currentRefChanged();                                                                               \
    }                                                                                                           \
    QVariant getRefAppId() const                                                                                \
    {                                                                                                           \
        return getRefAppIdImpl();                                                                               \
    }                                                                                                           \
    void setRefAppId(const QVariant &appId_)                                                                    \
    {                                                                                                           \
        setRefAppIdImpl(appId_);                                                                                \
        emit refAppIdChanged();                                                                                 \
    }                                                                                                           \
    QVariant getRefValue() const                                                                                \
    {                                                                                                           \
        return getRefValueImpl();                                                                               \
    }                                                                                                           \
    void setRefValue(const QVariant &value_)                                                                    \
    {                                                                                                           \
        setRefValueImpl(value_);                                                                                \
        emit refValueChanged();                                                                                 \
    }                                                                                                           \
    bool storeAfterSave() const                                                                                 \
    {                                                                                                           \
        return storeAfterSaveImpl();                                                                            \
    }                                                                                                           \
    void setStoreAfterSave(bool storeAfterSave_)                                                                \
    {                                                                                                           \
        setStoreAfterSaveImpl(storeAfterSave_);                                                                 \
        emit storeAfterSaveChanged();                                                                           \
    }                                                                                                           \
    bool jsonParamsGet() const                                                                                  \
    {                                                                                                           \
        return getJsonParamsGetImpl();                                                                          \
    }                                                                                                           \
    void setJsonParamsGet(bool jsonParamsGet_)                                                                  \
    {                                                                                                           \
        setJsonParamsGetImpl(jsonParamsGet_);                                                                   \
        emit jsonParamsGetChanged();                                                                            \
    }                                                                                                           \
    void startRefreshChildren(QString modelName_)                                                               \
    {                                                                                                           \
        emit refreshChildren(modelName_);                                                                       \
    }                                                                                                           \
    bool autoCreateChildrenModels() const                                                                       \
    {                                                                                                           \
        return autoCreateChildrenModelsImpl();                                                                  \
    }                                                                                                           \
    void setAutoCreateChildrenModels(bool autoCreateChildrenModels_)                                            \
    {                                                                                                           \
        setAutoCreateChildrenModelsImpl(autoCreateChildrenModels_);                                             \
        emit autoCreateChildrenModelsChanged();                                                                 \
    }                                                                                                           \
    bool autoCreateChildrenModelsOnSelect() const                                                               \
    {                                                                                                           \
        return autoCreateChildrenModelsOnSelectImpl();                                                          \
    }                                                                                                           \
    void setAutoCreateChildrenModelsOnSelect(bool autoCreateChildrenModelsOnSelect_)                            \
    {                                                                                                           \
        setAutoCreateChildrenModelsOnSelectImpl(autoCreateChildrenModelsOnSelect_);                             \
        emit autoCreateChildrenModelsOnSelectChanged();                                                         \
    }                                                                                                           \
    bool outputModel() const                                                                                    \
    {                                                                                                           \
        return getOutputModelImpl();                                                                            \
    }                                                                                                           \
    void setOutputModel(bool outputModel_)                                                                      \
    {                                                                                                           \
        setOutputModelImpl(outputModel_);                                                                       \
        emit outputModelChanged();                                                                              \
    }                                                                                                           \
    virtual void listLoadedVF() override                                                                        \
    {                                                                                                           \
        ListModelBaseData::listLoadedVF();                                                                      \
        emit listReloaded();                                                                                    \
    }                                                                                                           \
    virtual void itemAddedVF() override                                                                         \
    {                                                                                                           \
        ListModelBaseData::itemAddedVF();                                                                       \
        emit itemAdded();                                                                                       \
    }                                                                                                           \
    virtual void itemSetVF() override                                                                           \
    {                                                                                                           \
        ListModelBaseData::itemSetVF();                                                                         \
        emit itemSet();                                                                                         \
    }                                                                                                           \
    virtual void itemDeletedVF() override                                                                       \
    {                                                                                                           \
        ListModelBaseData::itemDeletedVF();                                                                     \
        emit itemDeleted();                                                                                     \
    }                                                                                                           \
    virtual void errorVF(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_) override    \
    {                                                                                                           \
        ListModelBaseData::errorVF(errorCode_, errorCodeStr_, reply_);                                          \
        emit error(errorCodeStr_, reply_.toJson(QJsonDocument::Indented));                                      \
    }                                                                                                           \
/* end macro LAYOUTMODEL() */


#endif // MODEL_H
