#ifndef MODEL_H
#define MODEL_H


#include <random>
#include <algorithm>
#include <QAbstractListModel>
#include <QString>
#include <QVector>
#include <QList>
#include <QHash>
#include <QDebug>
#include <QVariant>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


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

    void setLayoutRefImpl(const QString &fieldJsonName_,
                          const QString &parentModel_,
                          const QString &parentModelRefJsonName_,
                          bool notify_ = true);
    void addLayoutExtraFieldsImpl(const QString &modelName_,
                                  const QVariant &appId_);
    void registerListModel();
    void setParentListModelInfo(IListModelInfo *parentListModelInfo_);
    bool autoCreateChildrenModelsImpl() const;
    bool autoCreateChildrenModelsOnSelectImpl() const;
    void addExtraFieldRenameImpl(const QString &oldName_,
                                 const QString &newName_);
    void setSortFieldsImpl(const QStringList &sortFields_);
    void clearListLoaded();
    void setReadonlyImpl(bool readonly_);
    void setStoreAfterSaveBase(bool storeAfterSave_);
    void setModelChangeNotify(IListModelChangeNotify *setModelChangeNotify_);

public:
    virtual void startLoadChildModel() override;
    virtual void endLoadChildModel() override;
    virtual bool isListLoadedImpl() const override;
    virtual void listLoadedVF() override;
    virtual void itemAddedVF() override;
    virtual void itemSetVF() override;
    virtual void itemDeletedVF() override;
    virtual void errorVF(int errorCode_,
                         const QString &errorCodeStr_,
                         const QJsonDocument &reply_) override;
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;

    void parentItemRemoved();
    void setCurrentRefImpl(const QString &ref_);
    void setLayoutQMLNameImpl(const QString &layoutQMLName_);
    void setAutoCreateChildrenModelsImpl(bool autoCreateChildrenModels_);
    void setAutoCreateChildrenModelsOnSelectImpl(bool autoCreateChildrenModelsOnSelect_);
    void addModelParamImpl(const QString &name_, const QVariant &value_);
    void setRefAppIdImpl(const QVariant &appId_);
    void setRefValueImpl(const QVariant &value_);
    void setCurrentIndexImpl(int index_);
    void setJsonParamsGetImpl(bool jsonParamsGet_);
    void setOutputModelImpl(bool outputModel_);
    int getCurrentIndexImpl() const;

protected:
    QVariant getRefAppIdImpl() const;
    QVariant getRefValueImpl() const;
    bool isCurrentIndexValid(int size_) const;
    bool isIndexValid(int index_, int size_) const;
    const QString &currentRefImpl() const;
    const QString &getLayoutQMLNameImpl() const;
    bool getJsonParamsGetImpl() const;
    bool addRequest(RequestData *rd_);
    RequestData * findRequest(const QString &requestName_);
    bool findRequest(RequestData *request_) const;
    void removeRequest(RequestData *request_);
    int getNextAppId(void *dta_);
    int getOutOfRangeAppId() const;
    void setNextAppId(int appId_);
    void init(QObject *modelObj_);
    void startListLoad();
    void reverseListLoading();
    void setListLoaded();
    bool listLoading() const;
    QHash<QString, QVariant> renameFields(const QHash<QString, QVariant> &src_);
    bool getOutputModelImpl() const;
    QStringList getSortFieldsImpl() const;
    bool getReadonlyImpl() const;
    bool getStoreAfterSaveBase() const;

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
    QStringList m_sortFields;
    bool m_readonly = true;
    bool m_listLoaded = false;
    bool m_listLoading = false;
    int m_loadingChildenModels = 0;
    bool m_outputModel = false;
    IListModelInfo *m_parentListModelInfo = nullptr;
    bool m_storeAfterSave = true;
    IListModelChangeNotify *m_setModelChangeNotify = nullptr;
};


template<class DataType_, class ModelType_, class DataObjectType_ = DataType_>
class ListModelBaseOfData :
        public QAbstractListModel,
        public IListModel,
        public ListModelBaseData
{
public:
    using DataType = DataType_;
    using ModelType = ModelType_;
    using DataObjectType = DataObjectType_;

    explicit ListModelBaseOfData(
            QObject *parent_,
            std::shared_ptr<QVector<DataType_ *>> data_ = std::shared_ptr<QVector<DataType_ *>>{nullptr}
            )
        : QAbstractListModel(parent_),
          ListModelBaseData(this)
    {
#if defined(TRACE_THREADS)
    qDebug() << "ListModelBaseOfData::ListModelBaseOfData<" << getDataLayout<DataObjectType>().getLayoutJsonName() << ">()" << QThread::currentThread() << QThread::currentThreadId();
#endif
        m_data = data_;
        if(!m_data)
        {
            m_data = std::make_shared<QVector<DataType_ *>>();
        }
        updateDataType();
        getDataLayout<DataObjectType>().initQMLModelRoleNames(m_roleNames);
        setStoreAfterSaveBase(getDataLayout<DataObjectType_>().storeAfterSave());
    }

    virtual ~ListModelBaseOfData() override
    {
        clearData();
    }

    virtual int rowCount(const QModelIndex &parent_) const override
    {
        Q_UNUSED(parent_);
        if(!m_data)
        {
            return 0;
        }
#if defined(TRACE_LIST_SIZE)
        qDebug() << getLayoutJsonNameImpl() << "m_data->size() = " << m_data->size();
#endif
        return m_data->size();
    }

    virtual QVariant data(const QModelIndex &index_, int role_) const override
    {
        if(!m_data)
        {
            return QVariant();
        }
        const int row = index_.row();
        if(row < 0 || row >= m_data->size()) { return QVariant(); }
        const DataObjectType *item = dynamic_cast<const DataObjectType *>((*m_data)[row]);
        return getDataLayout<DataObjectType>().getModelValue(item, role_);
    }

    virtual bool setData(const QModelIndex &index_, const QVariant &value_, int role_ = Qt::EditRole) override
    {
        if(!m_data)
        {
            return false;
        }
        const int row = index_.row();
        if(row < 0 || row >= m_data->size()) { return false; }
        DataObjectType *item = dynamic_cast<DataObjectType *>((*m_data)[row]);
        return getDataLayout<DataObjectType>().setModelValue(item, role_, value_);
    }

    virtual QObject* getModel() const override
    {
        return m_model;
    }

    virtual void initResponse() override
    {
        LocalDataAPI *dataAPI = QMLObjectsBase::getInstance().getDataAPI();
        Q_ASSERT(dataAPI);
        QObject::connect(dataAPI, SIGNAL(response(int, const QString &, RequestData *, const QJsonDocument &)),
                         m_model, SLOT(jsonResponseSlot(int, const QString &, RequestData *, const QJsonDocument &)));
    }

    virtual const QString &getQMLLayoutName() const override
    {
        return getLayoutQMLNameImpl();
    }

    virtual const QString &getJsonLayoutName() const override
    {
        return getDataLayout<DataObjectType>().getLayoutJsonName();
    }

    virtual QString getLayoutIdFiledName() const override
    {
        return getDataLayout<DataObjectType>().getIdFieldJsonName();
    }

    virtual bool containsAppId(const QVariant &appId_) const override
    {
        return findDataItemByAppIdImpl(appId_);
    }

    virtual QVariant getCurrentIndexAppId() const override
    {
        if(!m_data)
        {
            return QVariant();
        }
        if(!isCurrentIndexValid(m_data->size())) { return QVariant::fromValue(-1); }
        return getDataLayout<DataObjectType>().getSpecialFieldValue(
                    layout::SpecialFieldEn::appId,
                    dynamic_cast<const DataObjectType *>((*m_data)[getCurrentIndexImpl()]));
    }

    virtual bool getValuesForAppId(const QVariant &appId_, QHash<QString, QVariant> &values_) const override
    {
        if(!m_data)
        {
            return false;
        }
        const DataObjectType *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(!item)
        {
            if(!isCurrentIndexValid(m_data->size())) { return false; }
            item = dynamic_cast<const DataObjectType *>((*m_data)[getCurrentIndexImpl()]);
        }
        if(!item)
        {
            return false;
        }
        return getDataLayout<DataObjectType>().getJsonValues(item, values_);
    }

    virtual QVariant getIdFieldValueForAppId(const QVariant &appId_) const override
    {
        if(!m_data)
        {
            return QVariant();
        }
        const DataObjectType *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(!item)
        {
            if(!isCurrentIndexValid(m_data->size())) { return QVariant(); }
            item = dynamic_cast<const DataObjectType *>((*m_data)[getCurrentIndexImpl()]);
        }
        if(!item)
        {
            return false;
        }
        return getDataLayout<DataObjectType>().getIdJsonValue(item);
    }

    virtual QVariant getFieldValueForAppId(const QVariant &appId_, const QString &jsonFieldName) const override
    {
        if(!m_data)
        {
            return QVariant();
        }
        const DataObjectType *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(!item)
        {
            if(!isCurrentIndexValid(m_data->size())) { return QVariant(); }
            item = dynamic_cast<const DataObjectType *>((*m_data)[getCurrentIndexImpl()]);
        }
        if(!item)
        {
            return false;
        }
        return getDataLayout<DataObjectType>().getJsonValue(item, jsonFieldName);
    }

    const DataObjectType *findDataItemByIdImpl(const QVariant &id_) const
    {
        if(!m_data)
        {
            return nullptr;
        }
        const auto fit = std::find_if(std::cbegin(*m_data),
                                      std::cend(*m_data),
                                      [&id_](const DataType_ *item)->bool
        {
            if(!item) { return false; }
            const QVariant id = getDataLayout<DataObjectType>().getIdJsonValue(
                        dynamic_cast<const DataObjectType *>(item)
                        );
            return id.isValid() && id == id_;
        });
        return std::cend(*m_data) == fit ? nullptr : dynamic_cast<const DataObjectType *>(*fit);
    }

    DataObjectType *findDataItemByIdImpl(const QVariant &id_)
    {
        return const_cast<DataObjectType *>(
                    const_cast<const ListModelBaseOfData<DataType_, ModelType_, DataObjectType_>*>(this)
                    ->findDataItemByIdImpl(id_));
    }

    QVariant findItemByIdImpl(const QVariant &id_)
    {
        return QVariant::fromValue(dynamic_cast<QObject *>(findDataItemByIdImpl(id_)));
    }

    const DataObjectType *findDataItemByAppIdImpl(const QVariant &appId_) const
    {
        if(!m_data)
        {
            return nullptr;
        }
        QVariant appId1 = appId_;
        if(!appId1.isValid())
        {
            appId1 = getCurrentIndexAppId();
        }
        const auto fit = std::find_if(std::cbegin(*m_data),
                                      std::cend(*m_data),
                                      [&appId1](const DataType_ *item)->bool
        {
            if(!item) { return false; }
            const QVariant appId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        dynamic_cast<const DataObjectType *>(item));
            return appId.isValid() && appId1.isValid() && appId1 == appId;
        });
        return std::cend(*m_data) == fit ? nullptr : dynamic_cast<const DataObjectType*>(*fit);
    }

    bool selectDataItemByAppIdImpl(const QVariant &appId_)
    {
        if(!m_data)
        {
            return false;
        }
        if(!appId_.isValid() || appId_.isNull()) { return false; }
        const auto fit = std::find_if(std::cbegin(*m_data),
                                      std::cend(*m_data),
                                      [&appId_](const DataType_ *item)->bool
        {
            if(!item) { return false; }
            const QVariant appId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        dynamic_cast<const DataObjectType *>(item));
            return appId.isValid() && appId_.isValid() && appId_ == appId;
        });
        if(std::cend(*m_data) == fit) { return false; }
        const int ni = std::distance(std::cbegin(*m_data), fit);
        if(ni == getCurrentIndexImpl()) { return false; }
        setCurrentIndexImpl(ni);
        autoLoadDataItem();
        return true;
    }

    bool selectDataItemByIdImpl(const QVariant &id_)
    {
        if(!m_data)
        {
            return false;
        }
        if(!id_.isValid() || id_.isNull()) { return false; }
        const auto fit = std::find_if(std::cbegin(*m_data),
                                      std::cend(*m_data),
                                      [&id_](const DataType_ *item)->bool
        {
            if(!item) { return false; }
            const QVariant id = getDataLayout<DataObjectType>().getIdJsonValue(
                        dynamic_cast<const DataObjectType *>(item)
                        );
            return id.isValid() && id_.isValid() && id_ == id;
        });
        if(std::cend(*m_data) == fit) { return false; }
        const int ni = std::distance(std::cbegin(*m_data), fit);
        if(ni == getCurrentIndexImpl()) { return false; }
        setCurrentIndexImpl(ni);
        autoLoadDataItem();
        return true;
    }

    const DataObjectType *dataItemAtImpl(int index_) const
    {
        if(!m_data)
        {
            return nullptr;
        }
        if(!isIndexValid(index_, m_data->size())) { return nullptr; }
        return dynamic_cast<const DataObjectType *>((*m_data)[index_]);
    }

    DataObjectType *dataItemAtImpl(int index_)
    {
        return const_cast<DataObjectType *>(
                    const_cast<const ListModelBaseOfData<DataType_, ModelType_, DataObjectType_> *>(this)
                    ->dataItemAtImpl(index_));
    }

    DataObjectType *findDataItemByAppIdImpl(const QVariant &appId_)
    {
        return const_cast<DataObjectType *>(
                    const_cast<const ListModelBaseOfData<DataType_, ModelType_, DataObjectType_> *>(this)
                    ->findDataItemByAppIdImpl(appId_));
    }

    QVariant findItemByAppIdImpl(const QVariant &appId_)
    {
        return QVariant::fromValue(dynamic_cast<QObject *>(
                                       findDataItemByAppIdImpl(appId_))
                                   );
    }

    bool selectItemByAppIdImpl(const QVariant &appId_)
    {
        return selectDataItemByAppIdImpl(appId_);
    }

    bool selectItemByIdImpl(const QVariant &id_)
    {
        return selectDataItemByIdImpl(id_);
    }

    const DataObjectType *getCurrentDataItem() const
    {
        return findDataItemByAppIdImpl(QVariant());
    }

    DataObjectType *getCurrentDataItem()
    {
        return const_cast<DataObjectType *>(
                    const_cast<const ListModelBaseOfData<DataType_, ModelType_, DataObjectType_> *>(this)
                    ->getCurrentDataItem()
                    );
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
        if(!m_data)
        {
            return QVariant();
        }
        if(!isIndexValid(index_, m_data->size())) { return QVariant(); }
        const DataObjectType* item = dynamic_cast<const DataObjectType* >(m_data->at(index_));
        return getDataLayout<DataObjectType>().getIdJsonValue(item);
    }

    QVariant getItemAppIdImpl(int index_) const
    {
        if(!m_data)
        {
            return QVariant();
        }
        if(!isIndexValid(index_, m_data->size())) { return QVariant(); }
        const DataObjectType* item = dynamic_cast<const DataObjectType* >(m_data->at(index_));
        return getDataLayout<DataObjectType>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
    }

    QVariant itemAtImpl(int index_) const
    {
        return QVariant::fromValue(
                    dynamic_cast<QObject *>(
                        const_cast<ListModelBaseOfData<DataType_, ModelType_, DataObjectType_> *>(this)
                        ->dataItemAtImpl(index_)
                        )
                    );
    }

    int sizeImpl() const
    {
        if(!m_data)
        {
            return 0;
        }
        return m_data->size();
    }

    template<typename Op_>
    DataObjectType *dataItemFindIf(Op_ op_)
    {
        if(!m_data)
        {
            return nullptr;
        }
        const int count = sizeImpl();
        for(int i = 0; i < count; i++)
        {
            if(nullptr == dynamic_cast<DataObjectType *>((*m_data)[i]))
            {
                continue;
            }
            if(op_(dynamic_cast<DataObjectType *>((*m_data)[i])))
            {
                return dynamic_cast<DataObjectType *>((*m_data)[i]);
            }
        }
        return nullptr;
    }

    DataObjectType *findDataItemByFieldValueImpl(const QString &qmlFieldName_, const QVariant &value_)
    {
        return dataItemFindIf([&qmlFieldName_, &value_](const DataType_ *item_)->bool
        {
            return getDataLayout<DataObjectType>().getQMLValue(
                        dynamic_cast<const DataObjectType *>(item_),
                        qmlFieldName_
                        ) == value_;
        });
    }

    QVariant findItemByFieldValueImpl(const QString &qmlFieldName_, const QVariant &value_) const
    {
        return QVariant::fromValue(
                    dynamic_cast<QObject *>(
                        const_cast<ListModelBaseOfData<DataType_, ModelType_, DataObjectType_> *>(this)
                        ->findDataItemByFieldValueImpl(qmlFieldName_,value_)
                        )
                    );
    }

    template<typename Op_>
    int indexFindIf(Op_ op_) const
    {
        if(!m_data)
        {
            return -1;
        }
        const int count = sizeImpl();
        for(int i = 0; i < count; i++)
        {
            if(nullptr == dynamic_cast<DataObjectType *>((*m_data)[i]))
            {
                continue;
            }
            if(op_(dynamic_cast<DataObjectType *>((*m_data)[i]))) { return i; }
        }
        return -1;
    }

    int indexOfDataItemImpl(const DataObjectType *item_) const
    {
        const QVariant appId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                    layout::SpecialFieldEn::appId,
                    item_
                    );
        return indexFindIf([&appId](const DataType_ *i)->bool
        {
            const QVariant iAppId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        dynamic_cast<const DataObjectType *>(i)
                        );
            return appId.isValid() && iAppId.isValid() && appId == iAppId;
        });
    }

    DataObjectType *createDataItemImpl()
    {
        DataObjectType *dta = new DataObjectType(m_model);
        getDataLayout<DataObjectType>().setSpecialFieldValue(
                    layout::SpecialFieldEn::appId,
                    QVariant::fromValue(getNextAppId(dta)),
                    dta);
        getDataLayout<DataObjectType>().setSpecialFieldValue(
                    layout::SpecialFieldEn::modelInfo,
                    QVariant::fromValue(dynamic_cast<IListModelInfo *>(this)),
                    dta);
        return dta;
    }

    bool setDataItemImpl(int index_,
                         DataObjectType *item_,
                         const QHash<QString, QVariant> &extraFields_ = QHash<QString, QVariant>()
                         )
    {
        if(!m_data)
        {
            return false;
        }
        if(index_ < 0 || index_ >= m_data->size()) { return false; }

        LocalDataAPI *dataAPI = QMLObjectsBase::getInstance().getDataAPI();
        if(!dataAPI) { return false; }

        if(getJsonLayoutName().isEmpty())
        {
            QVariant itemAppId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        item_);
            QVariant itemId = getDataLayout<DataObjectType>().getIdJsonValue(item_);
            RequestData *request = dataAPI->emptyRequest(
                        RequestData::setItemRequestName<DataObjectType>(),
                        itemAppId,
                        itemId
                        );
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, QString(), request, QJsonDocument());
            }
            dataAPI->freeRequest(request);
            return true;
        }
        else
        {
            QHash<QString, QVariant> extraFields(m_modelParams);
            extraFields.insert(extraFields_);
            for(const ExtraFields &f: qAsConst(m_extraFields))
            {
                IListModel *m = QMLObjectsBase::getInstance().getListModel(f.m_modelName);
                if(m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            RequestData *request = dataAPI->setItem(getJsonLayoutName(), item_, extraFields);
            return addRequest(request);
        }
    }

    bool addDataItemImpl(DataObjectType *item_,
                         bool setCurrentIndex_ = false,
                         const QHash<QString, QVariant> &extraFields_ = QHash<QString, QVariant>()
                         )
    {
        LocalDataAPI *dataAPI = QMLObjectsBase::getInstance().getDataAPI();
        if(!dataAPI) { return false; }

        if(getJsonLayoutName().isEmpty())
        {
            QVariant itemAppId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        item_
                        );
            QVariant itemId = getDataLayout<DataObjectType>().getIdJsonValue(item_);
            RequestData *request = dataAPI->emptyRequest(
                        RequestData::addItemRequestName<DataObjectType>(),
                        itemAppId,
                        itemId);
            if(request)
            {
                Q_ASSERT(nullptr != dynamic_cast<IListModelItem *>(item_));
                request->setItemData(dynamic_cast<IListModelItem *>(item_));
                request->setSetCurrentItemIndex(setCurrentIndex_);
            }
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, QString(), request, QJsonDocument());
            }
            dataAPI->freeRequest(request);
            return true;
        }
        else
        {
            QHash<QString, QVariant> extraFields(m_modelParams);
            extraFields.insert(extraFields_);
            for(const ExtraFields &f: qAsConst(m_extraFields))
            {
                IListModel *m = QMLObjectsBase::getInstance().getListModel(f.m_modelName);
                if(m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            RequestData *request = dataAPI->addItem(getJsonLayoutName(), item_, extraFields);
            if(request)
            {
                Q_ASSERT(nullptr != dynamic_cast<IListModelItem *>(item_));
                request->setItemData(dynamic_cast<IListModelItem *>(item_));
                request->setSetCurrentItemIndex(setCurrentIndex_);
            }
            return addRequest(request);
        }
    }

    bool delDataItemImpl(int index_,
                         const QHash<QString, QVariant> &extraFields_ = QHash<QString, QVariant>()
                         )
    {
        if(!m_data)
        {
            return false;
        }
        if(!isIndexValid(index_, m_data->size())) { return false; }
        if(nullptr == dynamic_cast<DataObjectType *>(m_data->at(index_)))
        {
            return false;
        }
        return delDataItemImpl(dynamic_cast<DataObjectType *>(m_data->at(index_)), extraFields_);

    }

    bool delDataItemImpl(DataObjectType *item_,
                         const QHash<QString, QVariant> &extraFields_ = QHash<QString, QVariant>()
                         )
    {
        LocalDataAPI *dataAPI = QMLObjectsBase::getInstance().getDataAPI();
        if(!dataAPI) { return false; }

        QVariant itemId = getDataLayout<DataObjectType>().getIdJsonValue(item_);
        if(!itemId.isValid() || itemId.isNull()) { return false; }

        RequestData *request = dataAPI->delItem(getJsonLayoutName(), item_, extraFields_);
        return addRequest(request);
    }

    void setLayoutIdFieldImpl(const QString &fieldJsonName_)
    {
        setDataLayout<DataObjectType>().setIdField(fieldJsonName_);
    }

    void autoLoadDataItemImpl(const DataObjectType *item_)
    {
        if(!item_) { return; }
#if defined(TRACE_MODEL_LOADING)
        qDebug() << "-start autoLoadDataItemImpl() item =" << item_ << getQMLLayoutName();
#endif
        const QVariant modelInfoVar = getDataLayout<DataObjectType>().getSpecialFieldValue(
                    layout::SpecialFieldEn::objectModelInfo,
                    item_);
        IListModelInfo *modelInfo = nullptr;
        layout::setValue(modelInfoVar, modelInfo);
        if(modelInfo)
        {
            modelInfo->loadChildrenVF();
        }
        getDataLayout<DataObjectType>().createQMLValues(item_);
        if(modelInfo)
        {
            modelInfo->objectLoadedVF();
        }
#if defined(TRACE_MODEL_LOADING)
        qDebug() << "-end autoLoadDataItemImpl() item =" << item_ << getQMLLayoutName();
#endif
    }

    template<typename Op_>
    bool sortIf(Op_ op_, bool saveSelection_, bool stableSort_ = false)
    {
        if(!m_data)
        {
            return false;
        }
        const DataObjectType *item = saveSelection_ ? getCurrentDataItem() : nullptr;
        const QVariant currentAppId = saveSelection_ && item
                ? getDataLayout<DataObjectType>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item)
                : QVariant();
        if(stableSort_) { std::stable_sort(std::begin(*m_data), std::end(*m_data), op_); }
        else { std::sort(std::begin(*m_data), std::end(*m_data), op_); }
        QModelIndex changedIndex0 = createIndex(0, 0);
        QModelIndex changedIndex1 = createIndex(std::max(0, sizeImpl() - 1), m_roleNames.size());
        emit dataChanged(changedIndex0, changedIndex1);
        if(saveSelection_) { return selectDataItemByAppIdImpl(currentAppId); }
        return false;
    }

    bool randOrderImpl(bool saveSelection_)
    {
        if(!m_data)
        {
            return false;
        }
        const DataObjectType *item = saveSelection_ ? getCurrentDataItem() : nullptr;
        const QVariant currentAppId = saveSelection_ && item
                ? getDataLayout<DataObjectType>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item)
                : QVariant();
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(std::begin(*m_data), std::end(*m_data), g);
        QModelIndex changedIndex0 = createIndex(0, 0);
        QModelIndex changedIndex1 = createIndex(std::max(0, sizeImpl() - 1), m_roleNames.size());
        emit dataChanged(changedIndex0, changedIndex1);
        if(saveSelection_) { return selectDataItemByAppIdImpl(currentAppId); }
        return false;
    }

    bool sortByFieldsImpl(const QStringList &fields_, bool saveSelection_)
    {
        bool res = false;
        for(const QString &field : qAsConst(fields_))
        {
            if(g_randSort == field)
            {
                res |= randOrderImpl(saveSelection_);
            }
            else
            {
                if(g_reverseSortOrder == field.at(0))
                {
                    QString fld;
                    fld = field.mid(1);
                    res |= sortIf([&fld](const DataType_ *i1_, const DataType_ *i2_)->bool
                    {
                        const DataObjectType * i1 = dynamic_cast<const DataObjectType *>(i1_);
                        const DataObjectType * i2 = dynamic_cast<const DataObjectType *>(i2_);
                        return getDataLayout<DataObjectType>().compareJsonValues(i1, i2, fld) > 0;
                    }, saveSelection_, true);
                }
                else
                {
                    res |= sortIf([&field](const DataType_ *i1_, const DataType_ *i2_)->bool
                    {
                        const DataObjectType * i1 = dynamic_cast<const DataObjectType *>(i1_);
                        const DataObjectType * i2 = dynamic_cast<const DataObjectType *>(i2_);
                        return getDataLayout<DataObjectType>().compareJsonValues(i1, i2, field) < 0;
                    }, saveSelection_, true);
                }
            }
        }
        return res;
    }

    void clearImpl()
    {
        clearData();
        setCurrentIndexImpl(-1);
        clearListLoaded();
    }

    void procedureImpl(const QString &procedureName_, const QHash<QString, QVariant> &extraFields_)
    {
        loadListImpl(procedureName_, extraFields_);
    }

    void dataItemProcedureImpl(const DataObjectType *item_,
                               const QString &procedureName_,
                               const QHash<QString, QVariant> &extraFields_)
    {
        Q_UNUSED(item_);
        Q_UNUSED(procedureName_);
        Q_UNUSED(extraFields_);
        Q_ASSERT(false); // TODO: implement for "take_ownship" "free_ownship" actions of the server
    }

    void loadListImpl(const QString &procedureName_ = QString(),
                      const QHash<QString, QVariant> &extraFields_ = QHash<QString, QVariant>()
                      )
    {
        LocalDataAPI *dataAPI = QMLObjectsBase::getInstance().getDataAPI();
        if(!dataAPI) { return; }
        RequestData *request = findRequest(RequestData::getListRequestName<DataObjectType>());
        if(request)
        {
            reloadList = true;
            return;
        }
        startListLoad();
        if(getJsonLayoutName().isEmpty())
        {
            request = dataAPI->emptyRequest(
                        RequestData::getListRequestName<DataObjectType>(),
                        QVariant() ,
                        QVariant());
            if(addRequest(request))
            {
                jsonResponseSlotImpl(0, QString(), request, QJsonDocument());
            }
            else
            {
                reverseListLoading();
            }
            dataAPI->freeRequest(request);
        }
        else
        {
            QStringList refs;
            for(const RefDescription &ref : qAsConst(m_refs))
            {
                refs.push_back(ref.m_ref);
            }
            QString parentModel;
            QString parentModelJsonFieldName;
            if(m_refs.contains(currentRefImpl()))
            {
                parentModel = m_refs.value(currentRefImpl()).m_parentModel;
                parentModelJsonFieldName = m_refs.value(currentRefImpl()).m_parentModelJsonFieldName;
            }
            QHash<QString, QVariant> extraFields(m_modelParams);
            extraFields.insert(extraFields_);
            for(const ExtraFields &f: qAsConst(m_extraFields))
            {
                IListModel *m = QMLObjectsBase::getInstance().getListModel(f.m_modelName);
                if(m)
                {
                    m->getValuesForAppId(f.m_appId, extraFields);
                }
            }
            extraFields = renameFields(extraFields);
            request = dataAPI->getList<DataObjectType>(
                        getJsonLayoutName(),
                        procedureName_,
                        refs,
                        currentRefImpl(),
                        parentModel,
                        parentModelJsonFieldName,
                        getRefAppIdImpl(),
                        getRefValueImpl(),
                        getJsonParamsGetImpl(),
                        extraFields,
                        getReadonlyImpl()
                        );
            if(!addRequest(request))
            {
                reverseListLoading();
            }
        }
    }

protected:
    bool storeAfterSaveImpl() const
    {
        return getStoreAfterSaveBase();
    }

    void setStoreAfterSaveImpl(bool storeAfterSave_)
    {
        setStoreAfterSaveBase(storeAfterSave_);
    }

    QVariant createItemImpl()
    {
        return QVariant::fromValue(createDataItemImpl());
    }

    bool setItemImpl(int index_, const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataObjectType *dataItem = qobject_cast<DataObjectType *>(obj);
        if(!dataItem) { return false; }
        return setDataItemImpl(index_, dataItem);
    }

    bool addItemImpl(const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataObjectType *dataItem = qobject_cast<DataObjectType *>(obj);
        if(!dataItem) { return false; }
        return addDataItemImpl(dataItem);
    }

    bool delItemImpl(int index_)
    {
        return delDataItemImpl(index_);
    }

    bool delItemImpl(const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataObjectType *dataItem = qobject_cast<DataObjectType *>(obj);
        if(!dataItem) { return false; }
        return delDataItemImpl(dataItem);
    }

    int indexOfItemImpl(const QVariant &item_) const
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataObjectType *dataItem = qobject_cast<DataObjectType *>(obj);
        if(!dataItem) { return -1; }
        return indexOfDataItemImpl(dataItem);
    }

    void setLayoutJsonNameImpl(const QString &layoutJsonName_)
    {
        setDataLayout<DataObjectType>().setLayoutJsonName(layoutJsonName_);
    }

    const QString &getLayoutJsonNameImpl()
    {
        return getDataLayout<DataObjectType>().getLayoutJsonName();
    }

    const QString &getLayoutJsonNameImpl() const
    {
        return getDataLayout<DataObjectType>().getLayoutJsonName();
    }

    QString getLayoutIdFieldImpl()
    {
        return getDataLayout<DataObjectType>().getIdFieldJsonName();
    }

    void jsonResponseSlotImpl(int errorCode_,
                              const QString &errorCodeStr_,
                              RequestData *request_,
                              const QJsonDocument &reply_)
    {
        if(request_->isRetried() || !findRequest(request_)) { return; }
        request_->setRetry();
        if(0 != errorCode_ && (200 > errorCode_ || 300 <= errorCode_))
        {
            modelError(errorCode_, errorCodeStr_, reply_);
            DataObjectType *itemToDel = dynamic_cast<DataObjectType *>(request_->getItemData());
            delete itemToDel;
            itemToDel = nullptr;
            request_->setItemData(nullptr);
        }
        else if(request_->getRequestName() == RequestData::getListRequestName<DataObjectType>())
        {
            modelListLoaded(reply_);
            if(reloadList)
            {
                reloadList = false;
                loadListImpl();
            }
        }
        else if(request_->getRequestName() == RequestData::addItemRequestName<DataObjectType>())
        {
            modelItemAdded(request_, reply_);
        }
        else if(request_->getRequestName() == RequestData::setItemRequestName<DataObjectType>())
        {
            modelItemSet(request_, reply_);
        }
        else if(request_->getRequestName() == RequestData::delItemRequestName<DataObjectType>())
        {
            modelItemDeleted(request_, reply_);
        }
        removeRequest(request_);
        clearTempData();
        if(m_model)
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
        if(!m_data)
        {
            return;
        }
        const auto fit = std::find_if(std::cbegin(*m_data),
                                      std::cend(*m_data),
                                      [&appId_](const DataType_ *item)->bool
        {
            if(!item) { return false; }
            const QVariant appId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        dynamic_cast<const DataObjectType *>(item)
                        );
            return appId.isValid() && appId_.isValid() && appId_ == appId;
        });
        if(std::cend(*m_data) == fit)
        {
            setCurrentIndexImpl(m_data->isEmpty()? -1: 0);
            autoLoadDataItem();
        }
        else
        {
            setCurrentIndexImpl(std::distance(std::cbegin(*m_data), fit));
            autoLoadDataItem();
        }
    }

    template<typename Op_>
    void filterItemsIf(Op_ op_)
    {
        if(!m_data)
        {
            return;
        }
        QVector<const DataType_ *> toRemove;
        toRemove.reserve(m_data->size());
        for(const DataType_ * item: qAsConst(*m_data))
        {
            if(!op_(item)) { toRemove.push_back(item); }
        }
        for(const DataType_ * item: qAsConst(toRemove))
        {
            if(nullptr == dynamic_cast<const DataObjectType *>(item))
            {
                continue;
            }
            removeItem(dynamic_cast<const DataObjectType *>(item));
        }
    }

    bool filterItemsLeftIDsImpl(const QVariantList &ids_)
    {
        if(!m_data)
        {
            return false;
        }
        filterItemsIf([&ids_](const DataType_ *i)->bool
        {
            return i &&
                    std::cend(ids_) != std::find_if(std::cbegin(ids_), std::cend(ids_),
                                                [&i](const QVariant &id_)
            {
                const QVariant id = getDataLayout<DataObjectType>().getIdJsonValue(
                            dynamic_cast<const DataObjectType *>(i)
                            );
                return id_.isValid() && id.isValid() && id_ == id;
            });
        });
        return !m_data->isEmpty();
    }

    bool filterItemsLeftIDsIntImpl(const QVector<int> &ids_)
    {
        QVariantList ids;
        convertIds(ids_, ids);
        return filterItemsLeftIDsImpl(ids);
    }

    bool filterItemsLeftExceptIDsImpl(const QVariantList &ids_)
    {
        if(!m_data)
        {
            return false;
        }
        filterItemsIf([&ids_](const DataType_ *i)->bool
        {
            return i &&
                    std::cend(ids_) == std::find_if(std::cbegin(ids_), std::cend(ids_),
                                                [&i](const QVariant &id_)
            {
                const QVariant id = getDataLayout<DataObjectType>().getIdJsonValue(
                            dynamic_cast<const DataObjectType *>(i)
                            );
                return id_.isValid() && id.isValid() && id_ == id;
            });
        });
        return !m_data->isEmpty();
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
        for(int id : qAsConst(idsInt_))
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
        if(!m_data)
        {
            return;
        }
        const QVariant oldCurrentAppId = getCurrentIndexAppId();
        const QVariant oldCurrentId = getIdFieldValueForAppId(oldCurrentAppId);
        QList<DataType_ *> loaded;
        if(!reply_.isEmpty())
        {
            for(int i = 0; ; i++)
            {
                QJsonValue itemJV = reply_[i];
                if(itemJV.isUndefined()) { break; }
                DataObjectType *item = createDataItemImpl();
                const bool ok = getDataLayout<DataObjectType>().setJsonValues(item, itemJV);
                if(!ok)
                {
                    delete item;
                    continue;
                }
                const QVariant id = getDataLayout<DataObjectType>().getIdJsonValue(item);
                DataObjectType *oldItem = findDataItemByIdImpl(id);
                if(oldItem)
                {
                    const QVariant appId = getDataLayout<DataObjectType>().getSpecialFieldValue(
                                layout::SpecialFieldEn::appId,
                                item);
                    Q_ASSERT(appId.isValid());
                    getDataLayout<DataObjectType>().setSpecialFieldValue(
                                layout::SpecialFieldEn::appId,
                                appId,
                                item);
                }
                loaded.push_back(item);
            }
        }
        clearData();
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "modelListLoaded() beginInsertRows(" << m_data->size() << "," << std::max(0, m_data->size() + loaded.size() - 1) << ")";
#endif
        beginInsertRows(QModelIndex(),
                        m_data->size(),
                        std::max(0, m_data->size() + loaded.size() - 1));
        std::copy(std::begin(loaded), std::end(loaded),
                  std::inserter(*m_data, std::end(*m_data)));
        endInsertRows();
        DataObjectType *oldIdItem = findDataItemByIdImpl(oldCurrentId);
        if(!oldIdItem)
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
        for(const DataType_ *item : qAsConst(*m_data))
        {
            if(nullptr == dynamic_cast<const DataObjectType *>(item))
            {
                continue;
            }
            autoLoadDataItem(dynamic_cast<const DataObjectType *>(item));
        }
        sortByFieldsImpl(getSortFieldsImpl(), true);
        setListLoaded();
#if defined(TRACE_LIST_LOAD_DATA)
        qDebug() << "{ modelListLoaded()" << getLayoutJsonNameImpl();
        for(const DataType_ *i: qAsConst(*m_data))
        {
            QHash<QString, QVariant> values;
            getDataLayout<DataObjectType>().getJsonValues(i, values);
            qDebug() << values;
        }
        qDebug() << "modelListLoaded() }";
#endif
    }

    virtual void modelItemAdded(RequestData *request_, const QJsonDocument &reply_)
    {
        if(!m_data)
        {
            return;
        }
        if(!dynamic_cast<DataObjectType *>(request_->getItemData())) { return; }
        const QVariant appId = request_->getItemAppId();

#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "modelItemAdded() beginInsertRows(" << m_data->size() << "," << m_data->size() << ")";
#endif
        beginInsertRows(QModelIndex(), m_data->size(), m_data->size());
        m_data->push_back(dynamic_cast<DataObjectType *>(request_->getItemData()));
        endInsertRows();

        request_->setItemData(nullptr);

        DataObjectType *item = findDataItemByAppIdImpl(appId);
        if(!item) { return; }

        getDataLayout<DataObjectType>().setJsonValues(item, reply_);
        autoLoadDataItem(item);
        if(request_->getSetCurrentItemIndex())
        {
            setCurrentIndexImpl(m_data->size() - 1);
        }
        sortByFieldsImpl(getSortFieldsImpl(), true);
        itemAddedVF();
    }

    virtual void modelItemSet(RequestData *request_, const QJsonDocument &reply_)
    {
        const QVariant id = request_->getItemId();
        DataObjectType *item = findDataItemByIdImpl(id);
        if(!item) { return; }
        getDataLayout<DataObjectType>().setJsonValues(item, reply_);
        autoLoadDataItem(item);
        sortByFieldsImpl(getSortFieldsImpl(), true);
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
        DataObjectType *item = findDataItemByIdImpl(id);
        if(!item) { return; }
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

    void autoLoadDataItem(const DataObjectType *item_ = nullptr)
    {
        if(!m_data)
        {
            return;
        }
        if(
            (
                item_ &&
                autoCreateChildrenModelsImpl()
            ) ||
            (
                !item_ &&
                autoCreateChildrenModelsOnSelectImpl() &&
                getCurrentIndexImpl() >= 0 &&
                isIndexValid(getCurrentIndexImpl(), m_data->size())
             ))
        {
            if(!item_)
            {
                item_ = dynamic_cast<const DataObjectType *>((*m_data)[getCurrentIndexImpl()]);
            }
            if(item_)
            {
                autoLoadDataItemImpl(item_);
            }
        }
    }

    void autoLoadItemImpl(const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataObjectType *dataItem = qobject_cast<DataObjectType *>(obj);
        if(!dataItem) { return; }
        autoLoadDataItemImpl(dataItem);
    }

    void clearData()
    {
        if(!m_data)
        {
            return;
        }
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "clearData() beginRemoveRows(" << 0 << "," << std::max(0, m_data->size() - 1) << ")";
#endif
        beginRemoveRows(QModelIndex(), 0, std::max(0, m_data->size() - 1));
        for(auto *&p: *m_data)
        {
            delete p;
            p = nullptr;
        }
        m_data->clear();
        endRemoveRows();
    }

    void removeItem(const DataObjectType *item_)
    {
        if(!m_data)
        {
            return;
        }
        QVariant appId0 = getDataLayout<DataObjectType>().getSpecialFieldValue(
                    layout::SpecialFieldEn::appId,
                    item_);
        if(!appId0.isValid() || appId0.isNull()) { return; }
        auto fit = std::find_if(std::begin(*m_data), std::end(*m_data),
                                      [&appId0](const DataType_ *i_) -> bool
        {
            QVariant appId1 = getDataLayout<DataObjectType>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        dynamic_cast<const DataObjectType *>(i_)
                        );
            return appId0 == appId1;
        });
        if(std::end(*m_data) == fit) { return; }
        const int index = std::distance(std::begin(*m_data), fit);
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "removeItem() beginRemoveRows(" << index << "," << index << ")";
#endif
        beginRemoveRows(QModelIndex(), index, index);
        delete *fit;
        *fit = nullptr;
        m_data->erase(fit);
        endRemoveRows();
    }

    void clearTempData()
    {
        if(!m_data)
        {
            return;
        }
        if(getStoreAfterSaveBase()) { return; }

        QVector<DataType_ *> waitingToUpdate;
        for(const RequestData *r : qAsConst(m_requests))
        {
            DataObjectType *item = nullptr;
            if(r->getRequestName() == RequestData::addItemRequestName<DataObjectType>())
            {
                const QVariant appId = r->getItemAppId();
                item = findDataItemByAppIdImpl(appId);
            }
            else if(r->getRequestName() == RequestData::setItemRequestName<DataObjectType>())
            {
                const QVariant id = r->getItemId();
                item = findDataItemByIdImpl(id);
            }
            if(item) { waitingToUpdate.push_back(item); }
        }
        if(waitingToUpdate.size() == m_data->size()) { return; }
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "clearTempData() beginRemoveRows(" << 0 << "," << std::max(0, m_data->size() - 1) << ")";
#endif
        beginRemoveRows(QModelIndex(), 0, std::max(0, m_data->size() - 1));
        for(auto *&p: *m_data)
        {
            const auto fit = std::find(
                        std::cbegin(waitingToUpdate),
                        std::cend(waitingToUpdate),
                        p
                        );
            if(std::cend(waitingToUpdate) == fit)
            {
                delete p;
                p = nullptr;
            }
        }
        m_data->clear();
        endRemoveRows();
#if defined(TRACE_LIST_DATA_ITEMS_CRUD)
        qDebug() << "clearTempData() beginInsertRows(" << 0 << "," << std::max(0, waitingToUpdate.size() - 1) << ")";
#endif
        beginInsertRows(QModelIndex(), 0, std::max(0, waitingToUpdate.size() - 1));
        std::copy(std::begin(waitingToUpdate), std::end(waitingToUpdate),
                  std::inserter(*m_data, std::end(*m_data)));
        endInsertRows();
    }

private:
    void updateDataType()
    {
        updateDataType(std::is_same<DataType_, DataObjectType_>());
    }

    void updateDataType(std::true_type)
    {
    }

    void updateDataType(std::false_type)
    {
        for(DataType_ *& p_: *m_data)
        {
            if(!p_)
            {
                continue;
            }
            DataObjectType_ *p1 = dynamic_cast<DataObjectType_ *>(p_);
            if(p1)
            {
                continue;
            }
            std::unique_ptr<DataObjectType_> pn
                    = std::make_unique<DataObjectType_>(
                        std::move(*p_),
                        dynamic_cast<ModelType_ *>(this)
                        );
            p_ = pn.release();
        }
    }

protected:
    std::shared_ptr<QVector<DataType_ *>> m_data;

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
    Q_PROPERTY(QStringList sortFields READ sortFields WRITE setSortFields NOTIFY sortFieldsChanged)             \
    Q_PROPERTY(bool readonly READ readonly WRITE setReadonly NOTIFY readonlyChanged)                            \
    Q_PROPERTY(QString objectName READ getObjectName WRITE setObjectName NOTIFY objectNameDerivedChanged)       \
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
    Q_INVOKABLE void clear()                                                                                    \
    {                                                                                                           \
        clearImpl();                                                                                            \
        emit currentIndexChanged();                                                                             \
        emit currentItemChanged();                                                                              \
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
        if(!m_data)                                                                                             \
        {                                                                                                       \
            return false;                                                                                       \
        }                                                                                                       \
        return m_data->isEmpty();                                                                               \
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
    Q_INVOKABLE void sortModel()                                                                                     \
    {                                                                                                           \
        if(sortByFieldsImpl(getSortFieldsImpl(), true))                                                         \
        {                                                                                                       \
            emit currentIndexChanged();                                                                         \
            emit currentItemChanged();                                                                          \
        }                                                                                                       \
    }                                                                                                           \
    Q_INVOKABLE void sortByFields(const QStringList &fields_)                                                   \
    {                                                                                                           \
        if(sortByFieldsImpl(fields_, true))                                                                     \
        {                                                                                                       \
            emit currentIndexChanged();                                                                         \
            emit currentItemChanged();                                                                          \
        }                                                                                                       \
    }                                                                                                           \
    Q_INVOKABLE void randOrder()                                                                                \
    {                                                                                                           \
        if(randOrderImpl(true))                                                                                 \
        {                                                                                                       \
            emit currentIndexChanged();                                                                         \
            emit currentItemChanged();                                                                          \
        }                                                                                                       \
    }                                                                                                           \
    Q_INVOKABLE void procedure(const QString &procedureName_, const QHash<QString, QVariant> &extraValues_)     \
    {                                                                                                           \
        procedureImpl(procedureName_, extraValues_);                                                            \
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
    QStringList sortFields()                                                                                    \
    {                                                                                                           \
        return getSortFieldsImpl();                                                                             \
    }                                                                                                           \
    void setSortFields(const QStringList &fields_)                                                              \
    {                                                                                                           \
        setSortFieldsImpl(fields_);                                                                             \
        emit sortFieldsChanged();                                                                               \
    }                                                                                                           \
    bool readonly()                                                                                             \
    {                                                                                                           \
        return getReadonlyImpl();                                                                               \
    }                                                                                                           \
    void setReadonly(const bool &readonly_)                                                                     \
    {                                                                                                           \
        setReadonlyImpl(readonly_);                                                                             \
        emit readonlyChanged();                                                                                 \
    }                                                                                                           \
    QString getObjectName()                                                                                     \
    {                                                                                                           \
        return QObject::objectName();                                                                           \
    }                                                                                                           \
    void setObjectName(const QString &objectName_)                                                              \
    {                                                                                                           \
        QObject::setObjectName(objectName_);                                                                    \
        emit objectNameDerivedChanged();                                                                        \
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
