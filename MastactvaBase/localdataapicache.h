#ifndef LOCALDATAAPICACHE_H
#define LOCALDATAAPICACHE_H


#include <QObject>
#include <QtSql>
#include <QVector>
#include <QList>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/dbrequestinfo.h"


class LocalDataAPICache : public QObject
{
    Q_OBJECT
protected:
    class LocalDBRequest : public DBRequestInfo
    {
    public:
        LocalDBRequest() = default;
    };

public:
    explicit LocalDataAPICache(QObject *parent_ = nullptr);
    virtual ~LocalDataAPICache() override;

    static void createInstance(QObject *parent_);
    static LocalDataAPICache *getInstance();

    RequestData *emptyRequest(const QString &requestName_, const QVariant &itemAppId_, const QVariant &itemId_)
    {
        return nullptr;
    }

    void freeRequest(RequestData *&r_)
    {
    }

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
        Q_UNUSED(jsonParams_);
        LocalDBRequest *r = new LocalDBRequest();
        r->init<DataType_>(layoutName_, procedureName_, refs_, currentRef_, parentModel_, parentModelJsonFieldName_, refAppId_, refValue_, readonly_);
        return getList(layoutName_, extraFields_, r);
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        return nullptr;
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        return nullptr;
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_, const DataType_ *item_)
    {
        return nullptr;
    }

signals:
    void response(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_);
    void error(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_);

protected:
    void freeRequests();
    RequestData *getList(const QString &layoutName_, const QHash<QString, QVariant> &extraFields_, LocalDBRequest *r_);

private:
    QSqlDatabase m_databaseRW;
    QSqlDatabase m_databaseRO;
    static LocalDataAPICache *g_localDataAPI;
};


#endif // LOCALDATAAPICACHE_H
