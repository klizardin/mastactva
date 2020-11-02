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


template<class DataType_>
class ListModelBaseOfData : public QAbstractListModel, public IListModel
{
public:
    explicit ListModelBaseOfData(QObject *parent_)
        :QAbstractListModel(parent_)
    {
        getDataLayout<DataType_>().initQMLModelRoleNames(m_roleNames);
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

    virtual const QString &getLayoutName() const override
    {
        return getDataLayout<DataType_>().getLayoutJsonName();
    }

    virtual QString getLayoutIdFiledName() const override
    {
        return getDataLayout<DataType_>().getIdFieldJsonName();
    }

    virtual int getCurrentIndex() const override
    {
        return m_currentIndex;
    }

    virtual bool getCurrentIndexValues(QHash<QString, QVariant> &values_) const override
    {
        if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return false; }
        return getDataLayout<DataType_>().getJsonValues(m_data[m_currentIndex], values_);
    }

    virtual QVariant getCurrentIndexIdFieldValue() const override
    {
        if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return QVariant(); }
        return getDataLayout<DataType_>().getIdJsonValue(m_data[m_currentIndex]);
    }

    virtual QVariant getCurrentIndexFieldValue(const QString &jsonFieldName) const override
    {
        if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return QVariant(); }
        return getDataLayout<DataType_>().getJsonValue(m_data[m_currentIndex], jsonFieldName);
    }

    const QString &currentRefImpl() const
    {
        return m_currentRef;
    }

    void setCurrentRefImpl(const QString &ref_)
    {
        m_currentRef = ref_;
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
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_>>(this).findDataItemByIdImpl(id_));
    }

    const DataType_ *findDataItemByAppIdImpl(const QVariant &appId_) const
    {
        const auto fit = std::find_if(std::begin(m_data),
                                      std::end(m_data),
                                      [&appId_](const DataType_ *item)->bool
        {
            if(nullptr == item) { return false; }
            const QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item);
            return appId.isValid() && appId == appId_;
        });
        return std::end(m_data) == fit ? nullptr : *fit;
    }

    DataType_ *findDataItemAppByIdImpl(const QVariant &appId_)
    {
        return const_cast<DataType_ *>(const_cast<const ListModelBaseOfData<DataType_>>(this).findDataItemByAppIdImpl(appId_));
    }

protected:
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
        request = netAPI->getList<DataType_>(currentRefImpl());
        if(nullptr != request) { m_requests.push_back(request); }
    }

    DataType_ *createDataItemImpl()
    {
    }

    QVariant createItemImpl()
    {
        return QVariant::fromValue(createDataItemImpl());
    }

    bool setDataItemImpl(int index_, DataType_ *item_)
    {
    }

    bool addDataItemImpl(DataType_ *item_)
    {
        m_data.push_back(item_);
        //TODO: add save item call
    }

    bool setItemImpl(int index_, const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataType_ *dataItem = qobject_cast<DataType_ *>(obj);
        if(nullptr == dataItem) { return false; }
        return setDataItemImpl(index_, dataItem);
    }

    bool addItemImpl(QVariant &item_)
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

    void setLayoutIdImpl(const QString &fieldJsonName_)
    {
        setDataLayout<DataType_>().setIdField(fieldJsonName_);
    }

    void setLayoutRefImpl(const QString &fieldJsonName_, const QString &parentModel_, const QString &parentModelRefJsonName_)
    {
        setDataLayout<DataType_>().setRef(fieldJsonName_, parentModel_, parentModelRefJsonName_);
    }

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

    void onJsonResponse(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
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
    }

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
            if(itemJV.isUndefined())
            {
                break;
            }
            DataType_ *item = new DataType_(this);
            const bool ok = getDataLayout<DataType_>.setJsonValues(item, itemJV);
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
        getDataLayout<DataType_>.setJsonValues(item, reply_);
    }

    virtual void itemSet(RequestData *request_, const QJsonDocument &reply_)
    {
        const QVariant id = request_->getItemId();
        DataType_ *item = findDataItemByIdImpl(id);
        if(nullptr == item) { return; }
        getDataLayout<DataType_>.setJsonValues(item, reply_);
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

private:
    QVector<DataType_ *> m_data;
    QHash<int, QByteArray> m_roleNames;
    int m_currentIndex = -1;
    QVector<RequestData *> m_requests;
    bool reloadList = false;
    QString m_currentRef;
};


#endif // MODEL_H
