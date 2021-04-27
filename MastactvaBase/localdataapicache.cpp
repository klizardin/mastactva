#include "localdataapicache.h"
#include <QNetworkReply>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


bool LocalDataAPIDefaultCacheImpl::canProcess(const DBRequestInfo *r_) const
{
    Q_UNUSED(r_);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::getListImpl(DBRequestInfo *r_)
{
    if(nullptr == r_)
    {
        return false;
    }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r_->getReadonly();
#endif

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    const QString tableName = db::tableName(r_->getTableName(), r_->getCurrentRef());

    const QHash<QString, QVariant> procedureFields = DBRequestInfo::procedureExtraFields(r_->getExtraFields());
    const QString procedureSelectFunction = procedureFields.contains(g_procedureSelectFunctionName)
            ? procedureFields.value(g_procedureSelectFunctionName).toString()
            : QString()
            ;
    const QString procedureArgFunction = procedureFields.contains(g_procedureArgFunctionName)
            ? procedureFields.value(g_procedureArgFunctionName).toString()
            : QString()
            ;
    const QList<QVariant> procedureFilterFields = procedureFields.contains(g_procedureFilterNamesName)
            ? procedureFields.value(g_procedureFilterNamesName).toList()
            : QList<QVariant>()
            ;
    const QString fieldsOfRequest = QString("%1 %2").arg(
                procedureSelectFunction,
                db::applyFunction(
                    db::filterNames(r_->getSqlNames(r_->getTableFieldsInfo()), procedureFilterFields)
                    , procedureArgFunction
                    ).join(g_insertFieldSpliter)
                );
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    const QStringList refs = r_->getRefs();
    for(const QString &ref : qAsConst(refs))
    {
        const QString refBindName = QString(":") + db::refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : QString());
    }
    const QHash<QString, QVariant> extraFields = DBRequestInfo::apiExtraFields(r_->getExtraFields());
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(extraFields));
        it != std::cend(qAsConst(extraFields))
        ; ++it
        )
    {
        const QString refBindName = QString(":") + db::refName(it.key());
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, it.value().toString());
    }
    const QString procedureConditions = procedureFields.contains(g_procedureConditionName)
            ? procedureFields.value(g_procedureConditionName).toString()
            : QString()
            ;
    const QList<QVariant> procedureFilterConditions = procedureFields.contains(g_procedureFilterConditionsName)
            ? procedureFields.value(g_procedureFilterConditionsName).toList()
            : QList<QVariant>()
            ;
    const QString procedureOrderBy = procedureFields.contains(g_procedureOrderByName)
            ? QString("%1 %2").arg(g_procedureOrderByName, procedureFields.value(g_procedureOrderByName).toString())
            : QString()
            ;
    const QString procedureLimit = procedureFields.contains(g_procedureLimitName)
            ? QString("%1 %2").arg(g_procedureLimitName, procedureFields.value(g_procedureLimitName).toString())
            : QString()
            ;
    const QHash<QString, QVariant> procedureArgs = procedureFields.contains(g_procedureArguments)
            ? procedureFields.value(g_procedureArguments).toHash()
            : QHash<QString, QVariant>()
            ;
    const bool hasCondition = !(refs.isEmpty()) || !(extraFields.isEmpty()) || !(procedureConditions.isEmpty());
    const QString conditionCases = (QStringList()
                            << db::equalToValueConditionListFromSqlNameList(
                                        db::filterNames(refs,procedureFilterConditions)
                                        )
                            << db::equalToValueConditionListFromSqlNameList(
                                        db::filterNames(extraFields.keys(),procedureFilterConditions)
                                        )
                            ).join(" AND ");
    const QString conditionStr = hasCondition
            ? QString("WHERE %1")
              .arg(conditionCases.isEmpty()
                   ? procedureConditions
                   : procedureConditions.isEmpty()
                     ? conditionCases
                     : QString("%1 AND ( %2 )").arg(conditionCases, procedureConditions)
                   )
            : QString()
            ;
    const QString sqlRequest = QString("SELECT %1 FROM %2 %3 %4 %5 ;")
            .arg(fieldsOfRequest,
                 tableName,
                 conditionStr,
                 procedureOrderBy,
                 procedureLimit
                 )
            ;

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "select sql" << sqlRequest;
#endif

    bool sqlRes = true;
    if(hasCondition || !procedureArgs.isEmpty())
    {
        query.prepare(sqlRequest);

        for(const QString &ref : qAsConst(bindRefs))
        {
            if(!procedureFilterConditions.isEmpty()
                    && !procedureFilterConditions.contains(QVariant::fromValue(ref))
                    ) { continue; }
            const QString v = defValues.value(ref);
            query.bindValue(ref, v);
        }
        const QList<QString> procedureArgsKeys = procedureArgs.keys();
        for(const QString &key : qAsConst(procedureArgsKeys))
        {
            const QVariant v = procedureArgs.value(key);
            query.bindValue(key, v);
        }

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
        qDebug() << "bound" << query.boundValues();
#endif

        sqlRes = query.exec();
    }
    else
    {
        sqlRes = query.exec(sqlRequest);
    }

    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    QJsonArray jsonArray;
    if(!sqlRes && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        qDebug() << "select sql" << sqlRequest;
        qDebug() << "bound" << query.boundValues();
        qDebug() << "sql error" << err.text();
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
        r->setError(true);
    }
    else if(query.first())
    {
        do
        {
            QJsonObject jsonObj;
            for(const db::JsonSqlField &fi : qAsConst(r_->getTableFieldsInfo()))
            {
                const auto fitFld = std::find_if(std::cbegin(procedureFilterFields), std::cend(procedureFilterFields),
                                                 [&fi](const QVariant &v)->bool
                {
                    return v.isValid() && fi.sqlName == v.toString();
                });
                if(!procedureFilterFields.isEmpty() && std::cend(procedureFilterFields) == fitFld) { continue; }
                const QVariant val = query.value(fi.sqlValueName());
                if(val.isValid())
                {
                    jsonObj.insert(fi.jsonName, fi.jsonValue(val));
                }
            }
            jsonArray.push_back(jsonObj);
        } while(query.next());
    }

    r->addJsonResult(QJsonDocument(jsonArray));

#if defined(TRACE_DB_DATA_RETURN) || defined(TRACE_DB_REQUESTS)
    qDebug() << "select sql result" << r->reply();
#endif

    r->setProcessed(true);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::addItemImpl(const QVariant &appId_,
                                               const QHash<QString, QVariant> &values_,
                                               DBRequestInfo *r_)
{
    if(nullptr == r_)
    {
        return false;
    }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r_->getReadonly();
#endif

    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemAppId(appId_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QSqlQuery findQuery(db);
    const QString tableName = db::tableName(r_->getTableName(), r_->getCurrentRef());

    const QHash<QString, QVariant> extraFields = DBRequestInfo::apiExtraFields(r_->getExtraFields());
    const QStringList refs = r_->getRefs();
    const QString fieldNames = (QStringList()
                                << db::refNames(refs)
                                << db::refNames(extraFields.keys())
                                << DBRequestInfo::getSqlNames(r_->getTableFieldsInfo())
                                ).join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : qAsConst(refs))
    {
        const QString refBindName = QString(":") + db::refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : QString());
    }
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(extraFields));
        it != std::cend(qAsConst(extraFields))
        ; ++it
        )
    {
        const QString refBindName = QString(":") + db::refName(it.key());
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, it.value().toString());
    }
    const QString fieldNamesBindings = (QStringList()
                                        << bindRefs
                                        << DBRequestInfo::getSqlBindNames(r_->getTableFieldsInfo())
                                        ).join(g_insertFieldSpliter);
    const QString sqlRequest = QString("INSERT INTO %1 ( %2 ) VALUES ( %3 ) ;")
            .arg(tableName, fieldNames, fieldNamesBindings);

    QString idFieldJsonName;
    QString idFieldSqlName;
    const auto fitId = std::find_if(std::cbegin(qAsConst(r_->getTableFieldsInfo())),
                                    std::cend(qAsConst(r_->getTableFieldsInfo())),
                                    [](const db::JsonSqlField &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    if(std::cend(qAsConst(r_->getTableFieldsInfo())) != fitId)
    {
        idFieldJsonName = fitId->jsonName;
        idFieldSqlName = fitId->sqlName;
    }
    const QString sqlNextIdRequest = QString("SELECT MAX(%1) FROM %2 ;")
            .arg(idFieldSqlName, tableName);

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "insert sql" << sqlRequest;
    qDebug() << "select max sql" << sqlNextIdRequest;
#endif

    query.prepare(sqlRequest);
    for(const QString &bind : qAsConst(bindRefs))
    {
        const QString v = defValues.value(bind);
        query.bindValue(bind, v);
    }
    int nextId = 1;
    for(const db::JsonSqlField &bindInfo : qAsConst(r_->getTableFieldsInfo()))
    {
        if(bindInfo.idField)
        {

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
    qDebug() << "select max sql bound" << findQuery.boundValues();
#endif

            if(!findQuery.exec(sqlNextIdRequest) && findQuery.lastError().type() != QSqlError::NoError)
            {
                const QSqlError err = findQuery.lastError();
                qDebug() << "sql request " << sqlNextIdRequest;
                qDebug() << "bound " << findQuery.boundValues();
                qDebug() << "sql error " << err.text();
            }
            else if(findQuery.first())
            {
                nextId = findQuery.value(0).toInt() + 1;
            }
            bindInfo.bind(query, QVariant::fromValue(nextId));
            findQuery.finish();
        }
        else
        {
            const QVariant val = values_.value(bindInfo.jsonName);
            bindInfo.bind(query, val);
        }
    }

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
    qDebug() << "insert sql bound" << query.boundValues();
#endif

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        qDebug() << "sql request " << sqlRequest;
        qDebug() << "bound " << query.boundValues();
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
        r->setError(true);
        r->addJsonResult(QJsonDocument(jsonArray));
    }
    else
    {
        QHash<QString, QVariant> values = values_;
        for(const db::JsonSqlField &bindInfo : qAsConst(r_->getTableFieldsInfo()))
        {
            if(bindInfo.idField)
            {
                values.insert(bindInfo.jsonName, QVariant::fromValue(nextId));
            }
        }
        r->addJsonResult(values);

    }

#if defined(TRACE_DB_DATA_RETURN) || defined(TRACE_DB_REQUESTS)
    qDebug() << "insert sql result" << r->reply();
#endif

    query.finish();
    r->setProcessed(true);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::setItemImpl(const QVariant &id_,
                                               const QHash<QString, QVariant> &values_,
                                               DBRequestInfo *r_)
{
    if(nullptr == r_)
    {
        return false;
    }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r_->getReadonly();
#endif

    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemId(id_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    const QString tableName = db::tableName(r_->getTableName(), r_->getCurrentRef());

    QString idFieldJsonName;
    QString idFieldSqlName;
    QString idFieldSqlBindName;
    const auto fitId = std::find_if(std::cbegin(qAsConst(r_->getTableFieldsInfo())),
                                    std::cend(qAsConst(r_->getTableFieldsInfo())),
                                    [](const db::JsonSqlField &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    if(std::cend(qAsConst(r_->getTableFieldsInfo())) != fitId)
    {
        idFieldJsonName = fitId->jsonName;
        idFieldSqlName = fitId->sqlName;
        idFieldSqlBindName = fitId->getBindName();
    }
    else
    {
        Q_ASSERT(false);
        return false;
    }
    const QStringList setNames = DBRequestInfo::getSetNames(r_->getTableFieldsInfo());
    const QString setStr = setNames.join(g_insertFieldSpliter);

    const QString sqlRequest = QString("UPDATE %1 SET %2 WHERE %3=%4 ;")
            .arg(tableName, setStr, idFieldSqlName, idFieldSqlBindName);

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "update sql" << sqlRequest;
#endif

    query.prepare(sqlRequest);
    for(const db::JsonSqlField &bindInfo : qAsConst(r_->getTableFieldsInfo()))
    {
        const QVariant val = values_.value(bindInfo.jsonName);
        bindInfo.bind(query, val);
    }

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
    qDebug() << "update sql bound" << query.boundValues();
#endif

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        qDebug() << "sql request " << sqlRequest;
        qDebug() << "bound " << query.boundValues();
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
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

    query.finish();
    r->setProcessed(true);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::delItemImpl(const QVariant &id_, DBRequestInfo *r_)
{
    if(nullptr == r_)
    { return false; }

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "readonly " << r_->getReadonly();
#endif

    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemId(id_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    const QString tableName = db::tableName(r_->getTableName(), r_->getCurrentRef());

    QString idFieldJsonName;
    QString idFieldSqlName;
    QString idFieldSqlBindName;
    const auto fitId = std::find_if(std::cbegin(qAsConst(r_->getTableFieldsInfo())),
                                    std::cend(qAsConst(r_->getTableFieldsInfo())),
                                    [](const db::JsonSqlField &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    if(std::cend(qAsConst(r_->getTableFieldsInfo())) != fitId)
    {
        idFieldJsonName = fitId->jsonName;
        idFieldSqlName = fitId->sqlName;
        idFieldSqlBindName = fitId->getBindName();
    }
    else
    {
        Q_ASSERT(false);
        return false;
    }

    const QString sqlRequest = QString("DELETE FROM %1 WHERE %3=%4 ;")
            .arg(tableName, idFieldSqlName, idFieldSqlBindName);

#if defined(TRACE_DB_USE) || defined(TRACE_DB_REQUESTS)
    qDebug() << "delete sql" << sqlRequest;
#endif

    query.prepare(sqlRequest);
    fitId->bind(query, id_);

#if defined(TRACE_DB_DATA_BINDINGS) || defined(TRACE_DB_REQUESTS)
    qDebug() << "delete sql bound" << query.boundValues();
#endif

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        qDebug() << "sql request " << sqlRequest;
        qDebug() << "bound " << query.boundValues();
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
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

    query.finish();
    r->setProcessed(true);
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
    if(nullptr == g_localDataAPI)
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
        return nullptr != r_ && nullptr != r && static_cast<const RequestData *>(r) == r_;
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

QHash<QString, QVariant> LocalDataAPICache::merge(const QHash<QString, QVariant> &v1_,
                                                  const QHash<QString, QVariant> &v2_)
{
    QHash<QString, QVariant> res;
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(v2_));
        it != std::cend(qAsConst(v2_));
        ++it)
    {
        if(v1_.contains(it.key()))
        {
            res.insert(it.key(), v1_.value(it.key())); // for common prefere v1_
        }
        else
        {
            res.insert(it.key(), it.value());
        }
    }
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(v1_));
        it != std::cend(qAsConst(v1_));
        ++it)
    {
        if(!v2_.contains(it.key()))
        {
            res.insert(it.key(), it.value());
        }
    }
    return res;
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
    if(nullptr == r_) { return; }

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

ILocalDataAPI *LocalDataAPICache::chooseView(DBRequestInfo *r_)
{
    if(nullptr == r_) { return nullptr; }
    r_->setDefaultAPI(&m_defaultAPIImpl);

    for(ILocalDataAPI *api : qAsConst(QMLObjectsBase::getInstance().getLocalDataAPIViews()))
    {
        if(nullptr != api && api->canProcess(r_)) { return api; }
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
