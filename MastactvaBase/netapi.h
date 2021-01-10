#ifndef NETAPI_H
#define NETAPI_H


#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include <QtQuick/QQuickItem>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/requestdata.h"


class NetAPI;
class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;


// TODO: add sqllite holder for caching requests
class NetRequestData: public RequestData
{
public:
    NetRequestData() = default;
    virtual ~NetRequestData();

protected:
    QNetworkReply *getReply() const;
    void setReply(QNetworkReply *reply_);
    bool compare(QNetworkReply *reply_) const;

private:
    QNetworkReply *m_reply = nullptr;

    friend class NetAPI;
};


class NetAPI : public QObject
{
    Q_OBJECT
    //QML_ELEMENT

public:
    static void createInstance(QObject *parent_);
    static NetAPI *getInstance();

public:
    NetAPI(QObject *parent_ = nullptr);
    virtual ~NetAPI() override;

    void updateAuthConsts();

    RequestData *emptyRequest(const QString &requestName_, const QVariant &itemAppId_, const QVariant &itemId_);
    void freeRequest(RequestData *&r_);

    template<class DataType_>
    RequestData *getList(const QString &layoutName_,
                         const QString &procedureName_,
                         const QStringList &refs_,
                         const QString &currentRef_,
                         const QString &parentModel_,
                         const QString &parentModelJsonFieldName_,
                         const QVariant &refAppId_,
                         const QVariant &refValue_,
                         bool jsonParams_,
                         const QHash<QString, QVariant> &extraFields_,
                         bool readonly_
                         )
    {
        Q_UNUSED(refs_);
        Q_UNUSED(readonly_);
        if(!procedureName_.isEmpty())
        {
            return getListByProcedureImpl(RequestData::getListRequestName<DataType_>(), layoutName_, procedureName_, jsonParams_, extraFields_);
        }
        IListModel *parentModelPtr = QMLObjectsBase::getInstance().getListModel(parentModel_);
        if(nullptr != parentModelPtr || (!refAppId_.isValid() && refValue_.isValid()))
        {
            const QVariant idField =
                    refValue_.isValid()
                        ? refValue_
                        : parentModelJsonFieldName_.isEmpty()
                          ? parentModelPtr->getIdFieldValueForAppId(refAppId_)
                          : parentModelPtr->getFieldValueForAppId(refAppId_, parentModelJsonFieldName_)
                    ;
            return getListByRefImpl(RequestData::getListRequestName<DataType_>(), layoutName_, currentRef_, idField, jsonParams_, extraFields_);
        }
        else
        {
            return getListImpl(RequestData::getListRequestName<DataType_>(), layoutName_, extraFields_);
        }
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        values = merge(extraFields_, values);
        QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
        if(!appId.isValid()) { return nullptr; }
        return addItemImpl(RequestData::addItemRequestName<DataType_>(), layoutName_, appId, values);
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        values = merge(extraFields_, values);
        QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!id.isValid()) { return nullptr; }
        return setItemImpl(RequestData::setItemRequestName<DataType_>(), layoutName_, id, values);
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_, const DataType_ *item_)
    {
        const QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!id.isValid() || id.isNull()) { return nullptr; }
        return delItemImpl(RequestData::delItemRequestName<DataType_>(), layoutName_, id);
    }

signals:
    void response(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_);
    void error(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_);

protected slots:
    void replayFinished(QNetworkReply *reply_);

protected:
    RequestData *getListByRefImpl(const QString& requestName_, const QString &jsonLayoutName_, const QString &ref_, const QVariant &id_, bool jsonParams_, const QHash<QString, QVariant> &extraFields_);
    RequestData *getListByProcedureImpl(const QString& requestName_, const QString &jsonLayoutName_, const QString &procedureName_, bool jsonParams_, const QHash<QString, QVariant> &extraFields_);
    RequestData *getListImpl(const QString& requestName_, const QString &jsonLayoutName_, const QHash<QString, QVariant> &extraFields_);
    RequestData *addItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_);
    RequestData *setItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_);
    RequestData *delItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &id_);

    void setBasicAuthentification(QNetworkRequest* netRequest_);
    void clearData();
    bool init();

    QHash<QString, QVariant> merge(const QHash<QString, QVariant> &v1_, const QHash<QString, QVariant> &v2_);

private:
    QString m_hostName;
    QString m_loggin;
    QString m_pass;
    QString m_hostUrlBase;

    QNetworkAccessManager *m_networkManager = nullptr;
    QList<NetRequestData*> m_requests;
};


#endif // NETAPI_H
