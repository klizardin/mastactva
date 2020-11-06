#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>
#include <QList>
#include <QHash>
#include <QVariant>
#include "Layout.h"
#include "netapi.h"
#include "qmlobjects.h"
#include "IModel.h"


// TODO : add notification on data changes
// TODO : event on data loaded check if data was changed, notify child nodes

template<class DataType_>
class ListModelBaseOfData : public QAbstractListModel, public IListModel
{
private:
    class RefDescription
    {
    public:
        QString m_ref;
        QString m_parentModel;
        QString m_parentModelJsonFieldName;
    };

public:
    explicit ListModelBaseOfData(QObject *parent_)
        :QAbstractListModel(parent_)
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

    virtual const QString &getQMLLayoutName() const override
    {
        return m_QMLLayoutName;
    }

    virtual const QString &getJsonLayoutName() const override
    {
        return getDataLayout<DataType_>().getLayoutJsonName();
    }

    virtual QString getLayoutIdFiledName() const override
    {
        return getDataLayout<DataType_>().getIdFieldJsonName();
    }

    virtual QVariant getCurrentIndexAppId() const override
    {
        if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return QVariant::fromValue(-1); }
        return getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, m_data[m_currentIndex]);
    }

    virtual bool getValuesForAppId(const QVariant &appId_, QHash<QString, QVariant> &values_) const override
    {
        const DataType_ *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(nullptr == item)
        {
            if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return false; }
            item = m_data[m_currentIndex];
        }
        return getDataLayout<DataType_>().getJsonValues(item, values_);
    }

    virtual QVariant getIdFieldValueForAppId(const QVariant &appId_) const override
    {
        const DataType_ *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(nullptr == item)
        {
            if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return false; }
            item = m_data[m_currentIndex];
        }
        return getDataLayout<DataType_>().getIdJsonValue(item);
    }

    virtual QVariant getFieldValueForAppId(const QVariant &appId_, const QString &jsonFieldName) const override
    {
        const DataType_ *item = appId_.isValid() ? findDataItemByAppIdImpl(appId_) : nullptr;
        if(nullptr == item)
        {
            if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return false; }
            item = m_data[m_currentIndex];
        }
        return getDataLayout<DataType_>().getJsonValue(item, jsonFieldName);
    }

protected:
    void setRefAppIdImpl(const QVariant &appId_)
    {
        m_refAppId = appId_;
    }

    QVariant getRefAppIdImpl() const
    {
        return m_refAppId;
    }

    int getCurrentIndexImpl() const
    {
        return m_currentIndex;
    }

    void setCurrentIndexImpl(int index_)
    {
        m_currentIndex = index_;
    }

    const QString &currentRefImpl() const
    {
        return m_currentRef;
    }

    void setCurrentRefImpl(const QString &ref_)
    {
        m_currentRef = ref_;
    }

    bool storeAfterSaveImpl() const
    {
        return getDataLayout<DataType_>().storeAfterSave();
    }

    void setStoreAfterSaveImpl(bool storeAfterSave_)
    {
        setDataLayout<DataType_>().setStoreAfterSave(storeAfterSave_);
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
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_>*>(this)->findDataItemByIdImpl(id_));
    }

    QVariant findItemByIdImpl(const QVariant &id_)
    {
        return QVariant::fromValue(static_cast<QObject *>(findDataItemByIdImpl(id_)));
    }

    const DataType_ *findDataItemByAppIdImpl(const QVariant &appId_) const
    {
        const auto fit = std::find_if(std::begin(m_data),
                                      std::end(m_data),
                                      [&appId_](const DataType_ *item)->bool
        {
            if(nullptr == item) { return false; }
            const QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
            QObject *obj1 = qvariant_cast<QObject *>(appId);
            QObject *obj2 = qvariant_cast<QObject *>(appId_);
            return nullptr != obj1 && obj1 == obj2;
        });
        return std::end(m_data) == fit ? nullptr : *fit;
    }

    DataType_ *findDataItemByAppIdImpl(const QVariant &appId_)
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_> *>(this)->findDataItemByAppIdImpl(appId_));
    }

    QVariant findItemByAppIdImpl(const QVariant &appId_)
    {
        return QVariant::fromValue(static_cast<QObject *>(findDataItemByAppIdImpl(appId_)));
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
        QString parentModel;
        QString parentModelJsonFieldName;
        if(m_refs.contains(currentRefImpl()))
        {
            parentModel = m_refs.value(currentRefImpl()).m_parentModel;
            parentModelJsonFieldName = m_refs.value(currentRefImpl()).m_parentModelJsonFieldName;
        }
        request = netAPI->getList<DataType_>(getJsonLayoutName(), currentRefImpl(), parentModel, parentModelJsonFieldName, getRefAppIdImpl());
        if(nullptr != request) { m_requests.push_back(request); }
    }

    DataType_ *createDataItemImpl()
    {
        DataType_ *dta = new DataType_(static_cast<QObject *>(this));
        getDataLayout<DataType_>().setSpecialFieldValue(
                    layout::SpecialFieldEn::appId,
                    QVariant::fromValue(static_cast<QObject *>(dta)),
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
        if(nullptr == netAPI) { return false;; }

        bool ret = getDataLayout<DataType_>().copyQMLFields(item_, m_data[index_]);
        if(!ret) { return ret; }

        RequestData *request = netAPI->setItem(getJsonLayoutName(), m_data[index_]);
        if(nullptr != request) { m_requests.push_back(request); }
        return nullptr != request;
    }

    bool addDataItemImpl(DataType_ *item_)
    {
        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
        if(nullptr == netAPI) { return false;; }

        m_data.push_back(item_);

        RequestData *request = netAPI->addItem(getJsonLayoutName(), m_data.back());
        if(nullptr != request) { m_requests.push_back(request); }
        return nullptr != request;
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

    void setLayoutQMLNameImpl(const QString &layoutQMLName_)
    {
        m_QMLLayoutName = layoutQMLName_;
    }

    const QString &getLayoutQMLNameImpl() const
    {
        return m_QMLLayoutName;
    }

    void setLayoutIdFieldImpl(const QString &fieldJsonName_)
    {
        setDataLayout<DataType_>().setIdField(fieldJsonName_);
    }

    QString getLayoutIdFieldImpl()
    {
        return getDataLayout<DataType_>().getIdFieldJsonName();
    }

    void setLayoutRefImpl(const QString &fieldJsonName_, const QString &parentModel_, const QString &parentModelRefJsonName_)
    {
        m_refs.insert(fieldJsonName_, {fieldJsonName_, parentModel_, parentModelRefJsonName_});
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
        if(errorCode_ != 0)
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
    }

    template<typename ModelType_>
    bool init(ModelType_ *model_)
    {
        NetAPI *netAPI = QMLObjects::getInstance().getNetAPI();
        if(nullptr == netAPI) { return false; }
        QObject::connect(netAPI, SIGNAL(response(int, RequestData *, const QJsonDocument &)), model_, SLOT(jsonResponseSlot(int, RequestData *, const QJsonDocument &)));
        return true;
    }

protected:
    virtual void handleError(int errorCode_, const QJsonDocument &reply_)
    {
        Q_UNUSED(errorCode_);
        Q_UNUSED(reply_);
    }

    virtual void listLoaded(const QJsonDocument &reply_)
    {
        Q_ASSERT(reply_.isArray());
        clearData();
        QList<DataType_ *> loaded;
        for(int i = 0; ; i++)
        {
            QJsonValue itemJV = reply_[i];
            if(itemJV.isUndefined()) { break; }
            DataType_ *item = new DataType_(this);
            const bool ok = getDataLayout<DataType_>().setJsonValues(item, itemJV);
            if(!ok)
            {
                delete item;
                continue;
            }
            loaded.push_back(item);
        }
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + loaded.size() - 1);
        std::copy(std::begin(loaded), std::end(loaded),
                  std::inserter(m_data, std::end(m_data)));
        endInsertRows();
        loaded.clear();
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

protected:
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

    RequestData * findRequest(const QString &requestName_)
    {
        const auto fit = std::find_if(std::begin(m_requests),
                                      std::end(m_requests),
                                      [&requestName_](RequestData *request_)->bool
        {
            return nullptr != request_ && request_->getRequestName() == requestName_;
        });
        return std::end(m_requests) == fit ? nullptr : *fit;
    }

    bool findRequest(RequestData *request_) const
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

    void removeRequest(RequestData *request_)
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
    QHash<QString, RefDescription> m_refs;
    int m_currentIndex = -1;
    QVector<RequestData *> m_requests;
    bool reloadList = false;
    QString m_currentRef;
    QVariant m_refAppId;
    QString m_QMLLayoutName;
};

// TODO: may be refactor define

#define LAYOUT_MODEL_IMPL()                                                                                     \
public:                                                                                                         \
    Q_PROPERTY(QString layoutQMLName READ getLayoutQMLName WRITE setLayoutQMLName NOTIFY layoutQMLNameChanged)  \
    Q_PROPERTY(QString layoutIdField READ getLayoutIdField WRITE setLayoutIdField NOTIFY layoutIdFieldChanged)  \
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)             \
    Q_PROPERTY(QString currentRef READ currentRef WRITE setCurrentRef NOTIFY currentRefChanged)                 \
    Q_PROPERTY(QVariant refAppId READ getRefAppId WRITE setRefAppId NOTIFY refAppIdChanged)                     \
    Q_PROPERTY(bool storeAfterSave READ storeAfterSave WRITE setStoreAfterSave NOTIFY storeAfterSaveChanged)    \
    Q_INVOKABLE QVariant findItemById(const QVariant &id_)                                                      \
    {                                                                                                           \
        return findItemByIdImpl(id_);                                                                           \
    }                                                                                                           \
    Q_INVOKABLE QVariant findItemByAppId(const QVariant &appId_)                                                \
    {                                                                                                           \
        return findItemByAppIdImpl(appId_);                                                                     \
    }                                                                                                           \
    Q_INVOKABLE void loadList()                                                                                 \
    {                                                                                                           \
        loadListImpl();                                                                                         \
    }                                                                                                           \
    Q_INVOKABLE QVariant createItem()                                                                           \
    {                                                                                                           \
        return createItemImpl();                                                                                \
    }                                                                                                           \
    Q_INVOKABLE bool setItem(int index_, const QVariant &item_)                                                 \
    {                                                                                                           \
        return setItemImpl(index_, item_);                                                                      \
    }                                                                                                           \
    Q_INVOKABLE bool addItem(const QVariant &item_)                                                             \
    {                                                                                                           \
        return addItemImpl(item_);                                                                              \
    }                                                                                                           \
    Q_INVOKABLE void setLayoutRef(const QString &fieldJsonName_, const QString &parentModel_, const QString &parentModelRefJsonName_)   \
    {                                                                                                           \
        setLayoutRefImpl(fieldJsonName_, parentModel_, parentModelRefJsonName_);                                \
    }                                                                                                           \
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
/* end macro LAYOUTMODEL() */


#endif // MODEL_H
