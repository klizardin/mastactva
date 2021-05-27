#include "localdataapicache.h"
#include <QNetworkReply>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


bool LocalDataAPIDefaultCacheImpl::canProcess(const DBRequestBase *r_) const
{
    const DBRequestPtr<const LocalDBRequest> r(r_);
    return r.operator bool();
}

bool LocalDataAPIDefaultCacheImpl::getListImpl(DBRequestBase *r_)
{
    const DBRequestPtr<LocalDBRequest> r(r_);
    if(!r)
    {
        return false;
    }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r->getReadonly();
#endif

    const QHash<QString, QVariant> procedureFields = DBRequestBase::procedureExtraFields(r->getExtraFields());
    const QList<QVariant> procedureFilterFields = procedureFields.contains(g_procedureFilterNamesName)
            ? procedureFields.value(g_procedureFilterNamesName).toList()
            : QList<QVariant>()
            ;
    const QStringList refs = r->getRefs();
    const QHash<QString, QVariant> extraFields = DBRequestBase::apiExtraFields(r->getExtraFields());
    const QString procedureConditions = procedureFields.contains(g_procedureConditionName)
            ? procedureFields.value(g_procedureConditionName).toString()
            : QString()
            ;
    const QList<QVariant> procedureFilterConditions = procedureFields.contains(g_procedureFilterConditionsName)
            ? procedureFields.value(g_procedureFilterConditionsName).toList()
            : QList<QVariant>()
            ;
    const QHash<QString, QVariant> procedureArgs = procedureFields.contains(g_procedureArguments)
            ? procedureFields.value(g_procedureArguments).toHash()
            : QHash<QString, QVariant>()
            ;

    const bool hasRefConditions = !(refs.isEmpty()) || !(extraFields.isEmpty());
    const bool hasProcedureConditions = !(procedureConditions.isEmpty()) && !procedureArgs.isEmpty();
    const bool hasCondition = hasRefConditions || hasProcedureConditions;

    const db::JsonSqlFieldAndValuesList refsValues =
            db::filter(
                db::createRefValuesList(
                    refs,
                    extraFields.keys(),
                    extraFields,
                    r->getCurrentRef(),
                    r->getIdField()
                    )
                , procedureFilterConditions
                );

    const QString sqlRequest = db::getSelectSqlRequest(
                r->getTableName(),
                r->getCurrentRef(),
                r->getTableFieldsInfo(),
                r->getRefs(),
                extraFields.keys(),
                procedureFields
                );

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "select sql" << sqlRequest;
#endif

    QSqlDatabase base = getBase(r);
    db::SqlQueryRAII query(base);

    bool sqlRes = true;
    if(hasCondition)
    {
        query.prepare(sqlRequest);
        db::bind(refsValues, query);
        db::bind(procedureArgs, query);

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
        qDebug() << "bound" << query.boundValues();
#endif

        sqlRes = query.exec();
    }
    else
    {
        sqlRes = query.exec(sqlRequest);
    }

    QJsonArray jsonArray;
    if(!sqlRes && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        qDebug() << "select sql" << sqlRequest;
        qDebug() << "sql error" << err.text();
        jsonArray = buildErrorDocument(err);
        r->setError(true);
    }
    else if(query.first())
    {
        const db::JsonSqlFieldsList filteredFields = db::filter(
                    r->getTableFieldsInfo(),
                    procedureFilterFields
                    );
        do
        {
            QJsonObject jsonObj = db::getJsonObject(filteredFields, query);
            jsonArray.push_back(jsonObj);
        } while(query.next());
    }

    r->addJsonResult(QJsonDocument(jsonArray));

#if defined(TRACE_DB_DATA_RETURN) || defined(TRACE_DB_REQUESTS)
    qDebug() << "select sql result" << r->reply();
#endif

    return true;
}

int LocalDataAPIDefaultCacheImpl::getNextIdValue(
        QSqlQuery &findQuery,
        const QString &sqlNextIdRequest
        )
{
    int nextId = -1;
    if(sqlNextIdRequest.isEmpty())
    {
        return nextId;
    }

    if(!findQuery.exec(sqlNextIdRequest)
            && findQuery.lastError().type() != QSqlError::NoError
            )
    {
        const QSqlError err = findQuery.lastError();
        throw err;
    }
    else if(findQuery.first())
    {
        nextId = findQuery.value(0).toInt() + 1;
    }

    return nextId;
}

QJsonArray LocalDataAPIDefaultCacheImpl::buildErrorDocument(const QSqlError &err_)
{
    QJsonArray jsonArray;
    QJsonObject jsonObj;
    jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err_.text()));
    jsonArray.push_back(jsonObj);
    return jsonArray;
}

bool LocalDataAPIDefaultCacheImpl::addItemImpl(const QVariant &appId_,
                                               const QHash<QString, QVariant> &values_,
                                               DBRequestBase *r_)
{
    const DBRequestPtr<LocalDBRequest> r(r_);
    if(!r)
    {
        return false;
    }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r->getReadonly();
#endif

    r->setItemAppId(appId_);

    const QHash<QString, QVariant> extraFields = DBRequestBase::apiExtraFields(r->getExtraFields());
    const QStringList refs = r->getRefs();

    const QString sqlRequest = db::getInsertSqlRequest(
                r->getTableName(),
                r->getCurrentRef(),
                r->getTableFieldsInfo(),
                refs,
                extraFields.keys()
                );

    const db::JsonSqlFieldAndValuesList refsValues = db::createRefValuesList(
                refs,
                extraFields.keys(),
                extraFields,
                r->getCurrentRef(),
                r->getIdField()
                );

    const QString sqlNextIdRequest = getNextIdSqlRequest(
                r->getTableName(),
                r->getCurrentRef(),
                r->getTableFieldsInfo()
                );

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "insert sql" << sqlRequest;
    qDebug() << "select max sql" << sqlNextIdRequest;
#endif

    try
    {
        QSqlDatabase base = getBase(r);
        db::SqlQueryRAII findQuery(base);
        QHash<QString, QVariant> values = values_;
        const int nextId = getNextIdValue(findQuery, sqlNextIdRequest);
        db::setIdField(r->getTableFieldsInfo(), values, nextId);

        db::SqlQueryRAII query(base);
        query.prepare(sqlRequest);
        db::bind(refsValues, query);
        db::bind(r->getTableFieldsInfo(), values, query);

        if(!query.exec() && query.lastError().type() != QSqlError::NoError)
        {
            const QSqlError err = query.lastError();
            throw err;
        }
        else
        {
            r->addJsonResult(values);
        }

#if defined(TRACE_DB_DATA_RETURN) || defined(TRACE_DB_REQUESTS)
    qDebug() << "insert sql result" << r->reply();
#endif
    }
    catch(const QSqlError &err_)
    {
        qDebug() << "sql request " << sqlRequest;
        qDebug() << "select max sql" << sqlNextIdRequest;
        qDebug() << "sql error " << err_.text();

        QJsonArray jsonArray = buildErrorDocument(err_);
        r->setError(true);
        r->addJsonResult(QJsonDocument(jsonArray));
    }
    return true;
}

bool LocalDataAPIDefaultCacheImpl::setItemImpl(const QVariant &id_,
                                               const QHash<QString, QVariant> &values_,
                                               DBRequestBase *r_)
{
    const DBRequestPtr<LocalDBRequest> r(r_);
    if(!r)
    {
        return false;
    }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r->getReadonly();
#endif

    r->setItemId(id_);

    const auto fitId = db::findIdField(r->getTableFieldsInfo());
    if(!db::idFieldExist(fitId, r->getTableFieldsInfo()))
    {
        Q_ASSERT(false);
        return false;
    }

    const QString sqlRequest = getUpdateSqlRequest(
                r->getTableName(),
                r->getCurrentRef(),
                r->getTableFieldsInfo()
                );

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "update sql" << sqlRequest;
#endif

    QSqlDatabase base = getBase(r);
    db::SqlQueryRAII query(base);
    query.prepare(sqlRequest);
    db::bind(r->getTableFieldsInfo(), values_, query);

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
    qDebug() << "update sql bound" << query.boundValues();
#endif

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        qDebug() << "sql request " << sqlRequest;
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray = buildErrorDocument(err);
        r->setError(true);
        r->addJsonResult(QJsonDocument(jsonArray));
    }
    else
    {
        r->addJsonResult(values_);
    }

#if defined(TRACE_DB_DATA_RETURN) || defined(TRACE_DB_REQUESTS)
    qDebug() << "update sql result" << r->reply();
#endif
    return true;
}

bool LocalDataAPIDefaultCacheImpl::delItemImpl(const QVariant &id_, DBRequestBase *r_)
{
    const DBRequestPtr<LocalDBRequest> r(r_);
    if(!r)
    {
        return false;
    }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r->getReadonly();
#endif

    r->setItemId(id_);

    const auto fitId = db::findIdField(r->getTableFieldsInfo());
    if(!db::idFieldExist(fitId, r->getTableFieldsInfo()))
    {
        Q_ASSERT(false);
        return false;
    }

    const QString sqlRequest = db::getDeleteSqlRequest(
                r->getTableName(),
                r->getCurrentRef(),
                r->getTableFieldsInfo()
                );
    Q_ASSERT(!sqlRequest.trimmed().isEmpty());

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "delete sql" << sqlRequest;
#endif

    QSqlDatabase base = getBase(r);
    db::SqlQueryRAII query(base);
    query.prepare(sqlRequest);
    db::bind(*fitId, query, id_);

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
    qDebug() << "delete sql bound" << query.boundValues();
#endif

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        qDebug() << "sql request " << sqlRequest;
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray = buildErrorDocument(err);
        r->setError(true);
        r->addJsonResult(QJsonDocument(jsonArray));
    }
    else
    {
        r->addJsonResult(QJsonDocument(QJsonArray()));
    }

#if defined(TRACE_DB_DATA_RETURN) || defined(TRACE_DB_REQUESTS)
    qDebug() << "delete sql result" << r->reply();
#endif
    return true;
}


LocalDataAPICache::LocalDataAPICache(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "LocalDataAPICache::LocalDataAPICache()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    openDB();
}

LocalDataAPICache::~LocalDataAPICache()
{
    closeDB();
    freeRequests();
}


LocalDataAPICache *LocalDataAPICache::g_localDataAPI = nullptr;


void LocalDataAPICache::createInstance(QObject *parent_, NetAPI *netAPI_)
{
    Q_UNUSED(netAPI_);
    if(!g_localDataAPI)
    {
        g_localDataAPI = new LocalDataAPICache(parent_);
    }
}

LocalDataAPICache *LocalDataAPICache::getInstance()
{
    return g_localDataAPI;
}

RequestData *LocalDataAPICache::emptyRequest(const QString &requestName_,
                                             const QVariant &itemAppId_,
                                             const QVariant &itemId_)
{
    LocalDBRequest *r = new LocalDBRequest();
    r->setRequestName(requestName_);
    r->setItemAppId(itemAppId_);
    r->setItemId(itemId_);
    //m_requests.push_back(r); // TODO: test free the request
    return r;
}

void LocalDataAPICache::freeRequest(RequestData *&r_)
{
    auto fit = std::find_if(std::begin(m_requests), std::end(m_requests),
                                  [&r_](const LocalDBRequest *r)->bool
    {
        return r_ && r && static_cast<const RequestData *>(r) == r_;
    });
    if(std::end(m_requests) == fit) { return; }
    m_requests.erase(fit);
    delete r_;
    r_ = nullptr;
}

void LocalDataAPICache::freeRequests()
{
    for(auto *&p : m_requests)
    {
        delete p;
        p = nullptr;
    }
    m_requests.clear();
}

void LocalDataAPICache::openDB()
{
    m_databaseRO = QSqlDatabase::addDatabase("QSQLITE", g_dbNameRO);
    m_databaseRO.setDatabaseName(QString(g_dbNameRO) + QString(g_dbNameExt));
    m_databaseRO.open();
    m_databaseRW = QSqlDatabase::addDatabase("QSQLITE", g_dbNameRW);
    m_databaseRW.setDatabaseName(QString(g_dbNameRW) + QString(g_dbNameExt));
    m_databaseRW.open();
}

void LocalDataAPICache::closeDB()
{
    m_databaseRO.close();
    m_databaseRW.close();
}

void LocalDataAPICache::pushRequest(LocalDBRequest *r_)
{
    if(!r_) { return; }

    r_->setRequestName(r_->getDBRequestName());
    const bool startNotify = m_requests.isEmpty();
    m_requests.push_back(r_);

    if(startNotify)
    {
        QTimer::singleShot(0, this, &LocalDataAPICache::makeResponses);
    }
}

void LocalDataAPICache::makeResponses()
{
    // TODO: move code to utils code
    QList<LocalDBRequest *> res;
    for(LocalDBRequest *r : qAsConst(m_requests))
    {
        if(r->isProcessed()) { res.push_back(r); }
    }
    for(const LocalDBRequest *r : qAsConst(res))
    {
        const auto fit = std::find(std::begin(m_requests), std::end(m_requests), r);
        m_requests.erase(fit);
    }

    for(LocalDBRequest *&r : res)
    {
        static QNetworkReply::NetworkError errCode = QNetworkReply::InternalServerError;
        static const QString errorCodeStr = QMetaEnum::fromType<QNetworkReply::NetworkError>()
                .valueToKey(errCode);
        static QNetworkReply::NetworkError noErrorCode = QNetworkReply::NoError;
        static const QString noErrorCodeStr = QMetaEnum::fromType<QNetworkReply::NetworkError>()
                .valueToKey(noErrorCode);
        if(r->error())
        {
            emit error(int(errCode), errorCodeStr, r->reply());
        }
        else
        {
            emit response(int(noErrorCode), noErrorCodeStr, r, r->reply());
        }
        delete r;
        r = nullptr;
    }

    const bool continueNotify = !m_requests.isEmpty();
    if(continueNotify)
    {
        QTimer::singleShot(0, this, &LocalDataAPICache::makeResponses);
    }
}

ILocalDataAPI *LocalDataAPICache::chooseView(DBRequestBase *r_)
{
    if(!r_) { return nullptr; }
    r_->setDefaultAPI(&m_defaultAPIImpl);

    for(ILocalDataAPI *api : qAsConst(QMLObjectsBase::getInstance().getLocalDataAPIViews()))
    {
        if(api && api->canProcess(r_)) { return api; }
    }

    return nullptr;
}

QHash<QString, QVariant> LocalDataAPICache::removeFields(const QHash<QString, QVariant> &extraFields_,
                                      const QList<QPair<QString, layout::JsonTypesEn>> &fieldsInfo_)
{
    QHash<QString, QVariant> res = extraFields_;
    for(const QPair<QString, layout::JsonTypesEn> &f : qAsConst(fieldsInfo_))
    {
        res.remove(f.first);
    }
    return res;
}
