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
    void addExtraFieldRenameImpl(const QString &oldName_, const QString &newName_);

public:
    virtual void startLoadChildModel() override;
    virtual void endLoadChildModel() override;
    virtual bool isListLoadedImpl() const override;
    virtual void listLoadedVF() override;

protected:
    void setRefAppIdImpl(const QVariant &appId_);
    QVariant getRefAppIdImpl() const;
    int getCurrentIndexImpl() const;
    void setCurrentIndexImpl(int index_);
    bool isCurrentIndexValid(int size_) const;
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
    void parentItemRemoved();
    void init(QObject *modelObj_);
    void addModelParamImpl(const QString &name_, const QVariant &value_);
    void setAutoCreateChildrenModelsImpl(bool autoCreateChildrenModels_);
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
    int m_currentIndex = -1;
    QString m_currentRef;
    QString m_QMLLayoutName;
    bool m_jsonParamsGet = false;
    int m_nextAppId = 0;
    QObject *m_modelObj = nullptr;
    bool m_autoRegister = false;
    IListModel *m_model = nullptr;
    bool m_autoCreateChildrenModels = false;
    bool m_listLoaded = false;
    bool m_listLoading = false;
    int m_loadingChildenModels = 0;
    IListModelInfo *m_parentListModelInfo = nullptr;
    bool m_outputModel = false;
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
        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
        Q_ASSERT(nullptr != netAPI);
        QObject::connect(netAPI, SIGNAL(response(int, RequestData *, const QJsonDocument &)), m_model, SLOT(jsonResponseSlot(int, RequestData *, const QJsonDocument &)));
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
            if(!isCurrentIndexValid(m_data.size())) { return false; }
            item = m_data[getCurrentIndexImpl()];
        }
        return getDataLayout<DataType_>().getIdJsonValue(item);
    }

    virtual QVariant getFieldValueForAppId(const QVariant &appId_, const QString &jsonFieldName) const override
    {
        const DataType_ *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(nullptr == item)
        {
            if(!isCurrentIndexValid(m_data.size())) { return false; }
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

    DataType_ *findDataItemByAppIdImpl(const QVariant &appId_)
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_, ModelType_> *>(this)->findDataItemByAppIdImpl(appId_));
    }

    QVariant findItemByAppIdImpl(const QVariant &appId_)
    {
        return QVariant::fromValue(static_cast<QObject *>(findDataItemByAppIdImpl(appId_)));
    }

    const DataType_ *getCurrentDataItem() const
    {
        return findDataItemByAppIdImpl(QVariant());
    }

    DataType_ *getCurrentDataItem()
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_, ModelType_> *>(this)->getCurrentDataItem());
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
        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
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
                jsonResponseSlotImpl(0, request, QJsonDocument());
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
                IListModel *m = QMLObjects::getInstance().getListModel(f.m_modelName);
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
                        getJsonParamsGetImpl(),
                        extraFields
                        );
            if(!addRequest(request))
            {
                setListLoaded();
            }
        }
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

    QVariant createItemImpl()
    {
        return QVariant::fromValue(createDataItemImpl());
    }

    bool setDataItemImpl(int index_, DataType_ *item_)
    {
        if(index_ < 0 || index_ >= m_data.size()) { return false; }

        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
        if(nullptr == netAPI) { return false; }

        if(getJsonLayoutName().isEmpty())
        {
            QVariant itemAppId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
            QVariant itemId = getDataLayout<DataType_>().getIdJsonValue(item_);
            RequestData *request = netAPI->emptyRequest(netAPI->setItemRequestName<DataType_>(), itemAppId, itemId);
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, request, QJsonDocument());
            }
            netAPI->freeRequest(request);
            return true;
        }
        else
        {
            bool ret = getDataLayout<DataType_>().copyQMLFields(item_, m_data[index_]);
            if(!ret) { return ret; }

            QHash<QString, QVariant> extraFields(m_modelParams);
            for(const ExtraFields &f: m_extraFields)
            {
                IListModel *m = QMLObjects::getInstance().getListModel(f.m_modelName);
                if(nullptr != m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            RequestData *request = netAPI->setItem(getJsonLayoutName(), m_data[index_], extraFields);
            return addRequest(request);
        }
    }

    bool addDataItemImpl(DataType_ *item_, bool setCurrentIndex_ = false)
    {
        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
        if(nullptr == netAPI) { return false; }

        m_data.push_back(item_);
        if(setCurrentIndex_)
        {
            setCurrentIndexImpl(m_data.size() - 1);
        }

        if(getJsonLayoutName().isEmpty())
        {
            QVariant itemAppId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
            QVariant itemId = getDataLayout<DataType_>().getIdJsonValue(item_);
            RequestData *request = netAPI->emptyRequest(netAPI->addItemRequestName<DataType_>(), itemAppId, itemId);
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, request, QJsonDocument());
            }
            netAPI->freeRequest(request);
            return true;
        }
        else
        {
            QHash<QString, QVariant> extraFields(m_modelParams);
            for(const ExtraFields &f: m_extraFields)
            {
                IListModel *m = QMLObjects::getInstance().getListModel(f.m_modelName);
                if(nullptr != m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            RequestData *request = netAPI->addItem(getJsonLayoutName(), m_data.back(), extraFields);
            return addRequest(request);
        }
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

    void setLayoutJsonNameImpl(const QString &layoutJsonName_)
    {
        setDataLayout<DataType_>().setLayoutJsonName(layoutJsonName_);
    }

    const QString &getLayoutJsonNameImpl()
    {
        return getDataLayout<DataType_>().getLayoutJsonName();
    }

    void setLayoutIdFieldImpl(const QString &fieldJsonName_)
    {
        setDataLayout<DataType_>().setIdField(fieldJsonName_);
    }

    QString getLayoutIdFieldImpl()
    {
        return getDataLayout<DataType_>().getIdFieldJsonName();
    }

    void jsonResponseSlotImpl(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        if(!findRequest(request_)) { return; }
        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
        if(nullptr == netAPI)
        {
            removeRequest(request_);
            return;
        }
        if(errorCode_ != 0 && errorCode_ >= 300)
        {
            handleError(errorCode_, reply_);
        }
        else if(request_->getRequestName() == netAPI->getListRequestName<DataType_>())
        {
            listLoaded(reply_);
            if(reloadList)
            {
                reloadList = false;
                loadListImpl();
            }
        }
        else if(request_->getRequestName() == netAPI->addItemRequestName<DataType_>())
        {
            itemAdded(request_, reply_);
        }
        else if(request_->getRequestName() == netAPI->setItemRequestName<DataType_>())
        {
            itemSet(request_, reply_);
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
        }
        else
        {
            setCurrentIndexImpl(std::distance(std::begin(m_data), fit));
        }
    }

protected:
    virtual void handleError(int errorCode_, const QJsonDocument &reply_)
    {
        Q_UNUSED(errorCode_);
        Q_UNUSED(reply_);
    }

    virtual void listLoaded(const QJsonDocument &reply_)
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
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + loaded.size() - 1);
        std::copy(std::begin(loaded), std::end(loaded),
                  std::inserter(m_data, std::end(m_data)));
        endInsertRows();
        DataType_ *oldIdItem = findDataItemByIdImpl(oldCurrentId);
        if(nullptr == oldIdItem)
        {
            setCurrentIndexImpl(0);
        }
        else
        {
            setCurrentIndexByAppIdImpl(oldCurrentAppId);
        }
        loaded.clear();
        if(autoCreateChildrenModelsImpl())
        {
            for(DataType_ *item : m_data)
            {
                getDataLayout<DataType_>().createQMLValues(item);
            }
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

    virtual void itemAdded(RequestData *request_, const QJsonDocument &reply_)
    {
        const QVariant appId = request_->getItemAppId();
        DataType_ *item = findDataItemByAppIdImpl(appId);
        if(nullptr == item) { return; }
        getDataLayout<DataType_>().setJsonValues(item, reply_);
    }

    virtual void itemSet(RequestData *request_, const QJsonDocument &reply_)
    {
        const QVariant id = request_->getItemId();
        DataType_ *item = findDataItemByIdImpl(id);
        if(nullptr == item) { return; }
        getDataLayout<DataType_>().setJsonValues(item, reply_);
    }

    void refreshChildrenSlotImpl(const QString &modelName_)
    {
        Q_ASSERT(m_refs.contains(currentRefImpl()));
        if(!m_refs.contains(currentRefImpl())) { return; }
        const RefDescription &ref = m_refs.value(currentRefImpl());
        if(modelName_ != ref.m_parentModel) { return; }
        IListModel* parentModel = QMLObjects::getInstance().getListModel(ref.m_parentModel);
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

    void parentCurrentIndexChanged()
    {
        loadListImpl();
    }

    void parentItemChanged()
    {
        loadListImpl();
    }

protected:
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

    void clearData()
    {
        beginRemoveRows(QModelIndex(), 0, m_data.size());
        for(auto *&p: m_data)
        {
            delete p;
            p = nullptr;
        }
        m_data.clear();
        endRemoveRows();
    }

    void clearTempData()
    {
        if(getDataLayout<DataType_>().storeAfterSave()) { return; }

        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
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
        beginRemoveRows(QModelIndex(), 0, m_data.size());
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
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + waitingToUpdate.size() - 1);
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

// TODO: may be refactor define

#define LAYOUT_MODEL_IMPL()                                                                                     \
public:                                                                                                         \
    /*properties*/                                                                                              \
    Q_PROPERTY(QString layoutQMLName READ getLayoutQMLName WRITE setLayoutQMLName NOTIFY layoutQMLNameChanged)  \
    Q_PROPERTY(QString layoutIdField READ getLayoutIdField WRITE setLayoutIdField NOTIFY layoutIdFieldChanged)  \
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)             \
    Q_PROPERTY(QString currentRef READ currentRef WRITE setCurrentRef NOTIFY currentRefChanged)                 \
    Q_PROPERTY(QVariant refAppId READ getRefAppId WRITE setRefAppId NOTIFY refAppIdChanged)                     \
    Q_PROPERTY(bool storeAfterSave READ storeAfterSave WRITE setStoreAfterSave NOTIFY storeAfterSaveChanged)    \
    Q_PROPERTY(bool jsonParamsGet READ jsonParamsGet WRITE setJsonParamsGet NOTIFY jsonParamsGetChanged)        \
    Q_PROPERTY(bool autoCreateChildrenModels READ autoCreateChildrenModels WRITE setAutoCreateChildrenModels NOTIFY autoCreateChildrenModelsChanged)    \
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
    Q_INVOKABLE bool setItem(int index_, const QVariant &item_)                                                 \
    {                                                                                                           \
        return setItemImpl(index_, item_);                                                                      \
    }                                                                                                           \
    Q_INVOKABLE bool addItem(const QVariant &item_)                                                             \
    {                                                                                                           \
        return addItemImpl(item_);                                                                              \
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
        emit currentIndexChanged();                                                                             \
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
/* end macro LAYOUTMODEL() */


#endif // MODEL_H
