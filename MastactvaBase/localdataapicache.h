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


// TODO: add pinup/unit tests
// TODO: split interface, dependency inversion to write unit test with mocks
// TODO: use dynamic_cast
class LocalDataAPIDefaultCacheImpl : public ILocalDataAPI
{
public:
    LocalDataAPIDefaultCacheImpl() = default;
    virtual ~LocalDataAPIDefaultCacheImpl() = default;

    virtual bool canProcess(const DBRequestBase *r_) const override;
    virtual bool getListImpl(DBRequestBase *r_) override;
    virtual bool addItemImpl(const QVariant &appId_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool setItemImpl(const QVariant &id_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool delItemImpl(const QVariant &id_,
                             DBRequestBase *r_) override;
private:
    static int getNextIdValue(
            QSqlQuery &findQuery,
            const QString &sqlNextIdRequest
            );
    static QJsonArray buildErrorDocument(const QSqlError &err);
};


class LocalDataAPICache : public QObject
{
    Q_OBJECT
public:
    explicit LocalDataAPICache(QObject *parent_ = nullptr);
    virtual ~LocalDataAPICache() override;

    static void createInstance(QObject *parent_, NetAPI * netAPI_);
    static LocalDataAPICache *getInstance();

    // TODO: maybe extract to facade class?
    // TODO: use of std::shared_ptr
    RequestData *emptyRequest(const QString &requestName_,
                              const QVariant &itemAppId_,
                              const QVariant &itemId_);
    void freeRequest(RequestData *&r_);

    // TODO: use of std::shared_ptr as we can use pointer in several places
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
        init<DataType_>(*r, RequestData::getListRequestName<DataType_>(),
                           layoutName_,
                           procedureName_,
                           refs_, currentRef_,
                           parentModel_, parentModelJsonFieldName_,
                           refAppId_, refValue_,
                           readonly_,
                           extraFields_);
        ILocalDataAPI *view = chooseView(r);
        bool res = !view
                ? m_defaultAPIImpl.getListImpl(r)
                : view->getListImpl(r)
                ;
        if(!res)
        {
            delete r;
            r = nullptr;
        }
        else
        {
            pushRequest(r);
        }
        return r;
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        LocalDBRequest *r = new LocalDBRequest();
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        values = merge(extraFields_, values);
        QVariant appId = getDataLayout<DataType_>().getSpecialFieldValue(
                    layout::SpecialFieldEn::appId,
                    item_);
        if(!appId.isValid()) { return nullptr; }
        QList<QPair<QString, layout::JsonTypesEn>> fieldsInfo;
        getDataLayout<DataType_>().getJsonFieldsInfo(fieldsInfo);
        const QHash<QString, QVariant> extraFields = removeFields(extraFields_, fieldsInfo);
        init<DataType_>(*r,
                    RequestData::addItemRequestName<DataType_>(),
                    layoutName_,
                    false,
                    extraFields);
        ILocalDataAPI *view = chooseView(r);
        bool res = !view
                ? m_defaultAPIImpl.addItemImpl(appId, values, r)
                : view->addItemImpl(appId, values, r)
                ;
        if(!res)
        {
            delete r;
            r = nullptr;
        }
        else
        {
            pushRequest(r);
        }
        return r;
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        LocalDBRequest *r = new LocalDBRequest();
        QHash<QString, QVariant> values;
        bool ok = getDataLayout<DataType_>().getJsonValues(item_, values);
        if(!ok) { return nullptr; }
        values = merge(extraFields_, values);
        QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        QList<QPair<QString, layout::JsonTypesEn>> fieldsInfo;
        getDataLayout<DataType_>().getJsonFieldsInfo(fieldsInfo);
        const QHash<QString, QVariant> extraFields = removeFields(extraFields_, fieldsInfo);
        init<DataType_>(*r,
                    RequestData::setItemRequestName<DataType_>(),
                    layoutName_,
                    false,
                    extraFields);
        if(!id.isValid()) { return nullptr; }
        ILocalDataAPI *view = chooseView(r);
        bool res = !view
                ? m_defaultAPIImpl.setItemImpl(id, values, r)
                : view->setItemImpl(id, values, r)
                ;
        if(!res)
        {
            delete r;
            r = nullptr;
        }
        else
        {
            pushRequest(r);
        }
        return r;
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        LocalDBRequest *r = new LocalDBRequest();
        QList<QPair<QString, layout::JsonTypesEn>> fieldsInfo;
        getDataLayout<DataType_>().getJsonFieldsInfo(fieldsInfo);
        const QHash<QString, QVariant> extraFields = removeFields(extraFields_, fieldsInfo);
        init<DataType_>(*r,
                    RequestData::delItemRequestName<DataType_>(),
                    layoutName_,
                    false,
                    extraFields);
        QVariant id = getDataLayout<DataType_>().getIdJsonValue(item_);
        if(!id.isValid()) { return nullptr; }
        ILocalDataAPI *view = chooseView(r);
        bool res = !view
                ? m_defaultAPIImpl.delItemImpl(id, r)
                : view->delItemImpl(id, r)
                ;
        if(!res)
        {
            delete r;
            r = nullptr;
        }
        else
        {
            pushRequest(r);
        }
        return r;
    }

signals:
    void response(int errorCode_,
                  const QString &errorCodeStr_,
                  RequestData *request_,
                  const QJsonDocument &reply_);
    void error(int errorCode_,
               const QString &errorCodeStr_,
               const QJsonDocument &reply_);

// TODO : private access writes
protected:
    void freeRequests();
    static QHash<QString, QVariant> merge(const QHash<QString, QVariant> &v1_,
                                          const QHash<QString, QVariant> &v2_);
    void openDB();
    void closeDB();
    void pushRequest(LocalDBRequest *r_);
    ILocalDataAPI *chooseView(DBRequestBase *r_);
    static QHash<QString, QVariant> removeFields(
            const QHash<QString, QVariant> &extraFields_,
            const QList<QPair<QString, layout::JsonTypesEn>> &fieldsInfo_);

private slots:
    void makeResponses();

private:
    QSqlDatabase m_databaseRW;
    QSqlDatabase m_databaseRO;
    // TODO: use std::shared_ptr
    QList<LocalDBRequest *> m_requests;
    static LocalDataAPICache *g_localDataAPI;       // TODO: do i really need a singelton?
    LocalDataAPIDefaultCacheImpl m_defaultAPIImpl;
};


#endif // LOCALDATAAPICACHE_H
