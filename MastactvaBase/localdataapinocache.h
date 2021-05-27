#ifndef LOCALDATAAPINOCACHE_H
#define LOCALDATAAPINOCACHE_H


#include <QObject>
#include <QtSql>
#include <QVector>
#include <QList>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/dbrequestinfo.h"


// TODO : add pinup test for implamentation
// TODO : split to 2 interfaces ILocalDataReadAPI ILocalDataWriteAPI
class LocalDataAPINoCacheImpl : public ILocalDataGetAPI
{
public:
    LocalDataAPINoCacheImpl() = default;
    virtual ~LocalDataAPINoCacheImpl() = default;
    virtual bool canProcess(const DBRequestBase *r_) const override;
    virtual bool getListImpl(DBRequestBase *r_) override;
};


class LocalDataAPINoCache : public QObject
{
    Q_OBJECT

private:
    class SaveDBRequest : public DBRequestBase
    {
    public:
        SaveDBRequest();
        bool operator == (const RequestData *request_) const;
        void setRequest(const RequestData *request_);

    private:
        const RequestData *m_request = nullptr;
    };

public:
    explicit LocalDataAPINoCache(QObject *parent_ = nullptr);
    virtual ~LocalDataAPINoCache() override;

    static void createInstance(QObject *parent_, NetAPI *netAPI_);
    static LocalDataAPINoCache *getInstance();

    RequestData *emptyRequest(const QString &requestName_,
                              const QVariant &itemAppId_,
                              const QVariant &itemId_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->emptyRequest(requestName_, itemAppId_, itemId_);
    }

    void freeRequest(RequestData *&r_)
    {
        if(!m_netAPI) { return; }
        m_netAPI->freeRequest(r_);
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
        SaveDBRequest *r = nullptr;
        if(isSaveToDBMode())
        {
            r = new SaveDBRequest();
            init<DataType_>(*r, RequestData::getListRequestName<DataType_>(),
                               layoutName_,
                               procedureName_,
                               refs_, currentRef_,
                               parentModel_, parentModelJsonFieldName_,
                               refAppId_, refValue_,
                               readonly_,
                               extraFields_
                               );
            ConcretePtr<LocalDataAPINoCacheImpl,ILocalDataAPI> view(chooseAPI(r));
            if(!view)
            {
                m_defaultAPIImpl.getListImpl(r);
            }
            else
            {
                view->getListImpl(r);
            }
        }
        if(!m_netAPI) { return nullptr; }
        RequestData *resRequest = m_netAPI->getList<DataType_>(layoutName_,
                                             procedureName_,
                                             refs_,
                                             currentRef_,
                                             parentModel_,
                                             parentModelJsonFieldName_,
                                             refAppId_,
                                             refValue_,
                                             jsonParams_,
                                             extraFields_,
                                             readonly_
                                             );
        if(resRequest && isSaveToDBMode())
        {
            r->setRequest(resRequest);
            m_requests.push_back(r);
        }
        else
        {
            delete r;
            r = nullptr;
        }
        return resRequest;
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->addItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->setItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->delItem(layoutName_, item_, extraFields_);
    }

    void startSave(const QString &savePath_);
    void endSave();

signals:
    void response(int errorCode_,
                  const QString &errorCodeStr_,
                  RequestData *request_,
                  const QJsonDocument &reply_);
    void error(int errorCode_,
               const QString &errorCodeStr_,
               const QJsonDocument &reply_);

protected slots:
    void responseFromStartegySlot(int errorCode_,
                                  const QString &errorCodeStr_,
                                  RequestData *request_,
                                  const QJsonDocument &reply_);
    void errorFromStartegySlot(int errorCode_,
                               const QString &errorCodeStr_,
                               const QJsonDocument &reply_);

protected:
    void cleanRequests();
    void cleanPath();
    void createDB();
    bool isSaveToDBMode() const;
    void fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_);
    ILocalDataAPI *chooseAPI(DBRequestBase *r_);

private:
    static bool hasDuplicate(
            QSqlQuery &findQuery_,
            QJsonValue &replayItem_,
            const db::JsonSqlFieldsList &fields_,
            const db::JsonSqlFieldAndValuesList refsValues_,
            const db::JsonSqlFieldsList::const_iterator &idField_
            );
    static void insertItem(
            QSqlQuery &insertQuery_,
            QJsonValue &replayItem_,
            const db::JsonSqlFieldsList &fields_,
            const db::JsonSqlFieldAndValuesList &refsValues_
            );

private:
    NetAPI *m_netAPI = nullptr;
    QString m_savePath;
    QString m_dbNameRW;
    QString m_dbNameRO;
    QSqlDatabase m_databaseRW;
    QSqlDatabase m_databaseRO;
    QList<SaveDBRequest *> m_requests;
    LocalDataAPINoCacheImpl m_defaultAPIImpl;
    static LocalDataAPINoCache *g_localDataAPI;

    friend class LocalDataAPINoCacheImpl;
};


#endif // LOCALDATAAPINOCACHE_H
