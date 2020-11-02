#ifndef NETAPI_H
#define NETAPI_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include <Layout.h>
#include "qmlobjects.h"
#include "IModel.h"


class NetAPI;
class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;


class RequestData
{
public:
    const QString &getRequestName() const;
    QVariant getItemId() const;
    QVariant getItemAppId() const;
    void allowModelProccessError();

protected:
    void setRequestName(const QString &requestName_);
    void setItenId(const QVariant &itemId_);
    void setItemAppId(const QVariant &itemAppId_);
    void setReply(QNetworkReply *reply_);
    bool compare(QNetworkReply *reply_) const;
    bool processErrorInNetAPI() const;

private:
    bool m_processErrorInNetAPI = true;
    QString m_requestName;
    QVariant m_itemId = QVariant::fromValue(-1);
    QVariant m_itemAppId = QVariant::fromValue(-1);
    QNetworkReply *m_reply = nullptr;

    friend class NetAPI;
};


class NetAPI : public QObject
{
    Q_OBJECT
public:
    NetAPI(QObject *parent_ = nullptr);
    virtual ~NetAPI() override;

    template<class DataType_>
    QString getListRequestName() const
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/GET";
    }

    template<class DataType_>
    QString addItemRequestName() const
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/POST";
    }

    template<class DataType_>
    QString setItemRequestName() const
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/PATCH";
    }

    template<class DataType_>
    RequestData *getList(const QString &currentRef_)
    {
        QString parentModel;
        QString parentModelJsonFieldName;
        getDataLayout<DataType_>().getRef(currentRef_, parentModel, parentModelJsonFieldName);
        IListModel *parentModelPtr = QMLObjects::getInstance().getListModel(parentModel);
        if(nullptr != parentModelPtr)
        {
            const QVariant idField = parentModelJsonFieldName.isEmpty()
                    ? parentModelPtr->getCurrentIndexIdFieldValue()
                    : parentModelPtr->getCurrentIndexFieldValue(parentModelJsonFieldName)
                    ;
            return getListByRefImpl(getDataLayout<DataType_>().getLayoutJsonName(), currentRef_, idField);
        }
        else
        {
            return getListImpl(getDataLayout<DataType_>().getLayoutJsonName());
        }
    }

    template<class DataType_>
    RequestData *addItem(const DataType_ *item_)
    {
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
        if(!appId.isValid()) { return nullptr; }
        return addItemImpl(getDataLayout<DataType_>().getLayoutJsonName(), appId, values);
    }

    template<class DataType_>
    RequestData *setItem(const DataType_ *item_)
    {
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!id.isValid()) { return nullptr; }
        return setItemImpl(getDataLayout<DataType_>().getLayoutJsonName(), id, values);
    }

signals:
    void response(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void error(int errorCode_, const QJsonDocument &reply_);

protected slots:
    void replayFinished(QNetworkReply *reply_);

protected:
    RequestData *getListByRefImpl(const QString &jsonLayoutName_, const QString &ref_, const QVariant &id_);
    RequestData *getListImpl(const QString &jsonLayoutName_);
    RequestData *addItemImpl(const QString &jsonLayoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_);
    RequestData *setItemImpl(const QString &jsonLayoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_);

    void setBasicAuthentification(QNetworkRequest* netRequest_);
    void clearData();
    bool init();

private:
    QString m_hostName;
    QString m_loggin;
    QString m_pass;
    QString m_hostUrlBase;

    QNetworkAccessManager *m_networkManager = nullptr;
    QList<RequestData*> m_requests;
};

#endif // NETAPI_H
