#ifndef LOCALDATAAPICACHE_H
#define LOCALDATAAPICACHE_H


#include <QObject>
#include <QtSql>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/dbrequestinfo.h"


class LocalDataAPICache : public QObject
{
    Q_OBJECT
protected:
    class LocalDBRequest :
            public DBRequestInfo,
            public RequestData
    {
    public:
        LocalDBRequest() = default;
        void addJsonResult(const QJsonDocument &doc_);
        void addJsonResult(const QHash<QString, QVariant> &values_);

    private:
        QJsonDocument m_doc;
    };

public:
    explicit LocalDataAPICache(QObject *parent_ = nullptr);
    virtual ~LocalDataAPICache() override;

    static void createInstance(QObject *parent_);
    static LocalDataAPICache *getInstance();

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
        Q_UNUSED(jsonParams_);
        LocalDBRequest *r = new LocalDBRequest();
        r->init<DataType_>(layoutName_,
                           procedureName_,
                           refs_, currentRef_,
                           parentModel_, parentModelJsonFieldName_,
                           refAppId_, refValue_,
                           readonly_,
                           extraFields_);
        return getListImpl(RequestData::getListRequestName<DataType_>(), r);
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        LocalDBRequest *r = new LocalDBRequest();
        r->init<DataType_>(layoutName_, false);
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        values = merge(extraFields_, values);
        QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(layout::SpecialFieldEn::appId, item_);
        if(!appId.isValid()) { return nullptr; }
        return addItemImpl(RequestData::addItemRequestName<DataType_>(), layoutName_, appId, values, r);
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        LocalDBRequest *r = new LocalDBRequest();
        r->init<DataType_>(layoutName_, false);
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        values = merge(extraFields_, values);
        QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!id.isValid()) { return nullptr; }
        return setItemImpl(RequestData::addItemRequestName<DataType_>(), layoutName_, id, values, r);
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_, const DataType_ *item_)
    {
        LocalDBRequest *r = new LocalDBRequest();
        r->init<DataType_>(layoutName_, false);
        QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!id.isValid()) { return nullptr; }
        return delItemImpl(RequestData::delItemRequestName<DataType_>(), layoutName_, id, r);
    }

signals:
    void response(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_);
    void error(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_);

protected:
    void freeRequests();
    RequestData *getListImpl(const QString& requestName_, LocalDBRequest *r_);
    RequestData *addItemImpl(const QString& requestName_, const QVariant &appId_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_);
    RequestData *setItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_);
    RequestData *delItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &id_, LocalDBRequest *r_);
    static QHash<QString, QVariant> merge(const QHash<QString, QVariant> &v1_, const QHash<QString, QVariant> &v2_);
    void openDB();
    void closeDB();

private:
    QSqlDatabase m_databaseRW;
    QSqlDatabase m_databaseRO;
    QList<LocalDBRequest *> m_requests;
    static LocalDataAPICache *g_localDataAPI;
};


#endif // LOCALDATAAPICACHE_H
