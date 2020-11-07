#ifndef NETAPI_H
#define NETAPI_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include <QtQuick/QQuickItem>
#include "Layout.h"
#include "qmlobjects.h"
#include "IModel.h"


class NetAPI;
class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;


// TODO: add sqllite holder for caching requests

class RequestData
{
public:
    RequestData() = default;
    virtual ~RequestData() = default;

    const QString &getRequestName() const;
    QVariant getItemId() const;
    QVariant getItemAppId() const;
    void allowModelProccessError();

protected:
    void setRequestName(const QString &requestName_);
    void setItemId(const QVariant &itemId_);
    void setItemAppId(const QVariant &itemAppId_);
    QNetworkReply *getReply() const;
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
    QML_ELEMENT

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

    RequestData *emptyRequest(const QString &requestName_, const QVariant &itemAppId_, const QVariant &itemId_);
    void freeRequest(RequestData *&r_);

    template<class DataType_>
    RequestData *getList(const QString &layoutName_,
                         const QString &currentRef_,
                         const QString &parentModel_,
                         const QString &parentModelJsonFieldName_,
                         const QVariant &refAppId_,
                         bool jsonParams_
                         )
    {
        IListModel *parentModelPtr = QMLObjects::getInstance().getListModel(parentModel_);
        if(nullptr != parentModelPtr)
        {
            const QVariant idField = parentModelJsonFieldName_.isEmpty()
                    ? parentModelPtr->getIdFieldValueForAppId(refAppId_)
                    : parentModelPtr->getFieldValueForAppId(refAppId_, parentModelJsonFieldName_)
                    ;
            return getListByRefImpl(getListRequestName<DataType_>(), layoutName_, currentRef_, idField, jsonParams_);
        }
        else
        {
            return getListImpl(getListRequestName<DataType_>(), layoutName_);
        }
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_, const DataType_ *item_)
    {
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
        if(!appId.isValid()) { return nullptr; }
        return addItemImpl(addItemRequestName<DataType_>(), layoutName_, appId, values);
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_, const DataType_ *item_)
    {
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!id.isValid()) { return nullptr; }
        return setItemImpl(setItemRequestName<DataType_>(), layoutName_, id, values);
    }

    void setInitialized();

signals:
    void response(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void error(int errorCode_, const QJsonDocument &reply_);
    void initialized();

protected slots:
    void replayFinished(QNetworkReply *reply_);

protected:
    RequestData *getListByRefImpl(const QString& requestName_, const QString &jsonLayoutName_, const QString &ref_, const QVariant &id_, bool jsonParams_);
    RequestData *getListImpl(const QString& requestName_, const QString &jsonLayoutName_);
    RequestData *addItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_);
    RequestData *setItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_);

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
