#ifndef LOCALDATAAPINOCACHE_H
#define LOCALDATAAPINOCACHE_H


#include <QObject>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/netapi.h"


class LocalDataAPINoCache : public QObject
{
    Q_OBJECT
public:
    explicit LocalDataAPINoCache(QObject *parent_ = nullptr);

    static void createInstance(QObject *parent_, NetAPI *netAPI_);
    static LocalDataAPINoCache *getInstance();

    RequestData *emptyRequest(const QString &requestName_, const QVariant &itemAppId_, const QVariant &itemId_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->emptyRequest(requestName_, itemAppId_, itemId_);
    }

    void freeRequest(RequestData *&r_)
    {
        if(nullptr == m_netAPI) { return; }
        m_netAPI->freeRequest(r_);
    }

    template<class DataType_>
    RequestData *getList(const QString &layoutName_,
                         const QString &currentRef_,
                         const QString &parentModel_,
                         const QString &parentModelJsonFieldName_,
                         const QVariant &refAppId_,
                         const QVariant &refValue_,
                         bool jsonParams_,
                         const QHash<QString, QVariant> &extraFields_
                         )
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->getList<DataType_>(layoutName_,
                                             currentRef_,
                                             parentModel_,
                                             parentModelJsonFieldName_,
                                             refAppId_,
                                             refValue_,
                                             jsonParams_,
                                             extraFields_
                                             );
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->addItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->setItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_, const DataType_ *item_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->delItem(layoutName_, item_);
    }

    void startSave(const QString &savePath_);
    void endSave();

signals:
    void response(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_);
    void error(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_);

protected slots:
    void responseFromStartegySlot(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_);
    void errorFromStartegySlot(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_);

private:
    NetAPI *m_netAPI = nullptr;
    static LocalDataAPINoCache *g_localDataAPI;
};


#endif // LOCALDATAAPINOCACHE_H