#include "localdataapicache.h"
#include <QNetworkReply>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/qmlobjects.h"


#define TRACE_DB_USE


inline QString refName(const QString &ref_)
{
    return QString(g_refPrefix) + ref_;
}

inline QStringList refsNames(const QStringList &refs_)
{
    QStringList res;
    for(const auto &s : refs_)
    {
        res.push_back(refName(s));
    }
    return res;
}

QStringList equalToValueConditionsFromSqlNamesFromSqlNames(const QStringList &names_)
{
    QStringList res;
    for(const QString &sqlName : names_)
    {
        const QString ref = refName(sqlName);
        res.push_back(QString("%1=%2").arg(ref, DBRequestInfo::JsonFieldInfo::toBindName(ref)));
    }
    return res;
}

QStringList filterNames(const QStringList &sqlNames_, const QList<QVariant> &leftNames_)
{
    if(leftNames_.isEmpty()) { return sqlNames_; }
    QStringList res;
    for(const QString &name: sqlNames_)
    {
        const auto fit = std::find_if(std::begin(leftNames_), std::end(leftNames_),
                                      [&name](const QVariant &val)->bool
        {
           return val.isValid() && val.toString() == name;
        });
        if(std::end(leftNames_) != fit)
        {
            res.push_back(name);
        }
    }
    return res;
}

QStringList applyFunction(const QStringList &sqlNames_, const QString &function_)
{
    if(function_.isEmpty()) { return sqlNames_; }
    QStringList res;
    for(const QString &name: sqlNames_)
    {
        res.push_back(QString("%1(%2)").arg(function_, name));
    }
    return res;
}


bool LocalDataAPIDefaultCacheImpl::canProcess(const DBRequestInfo *r_) const
{
    Q_UNUSED(r_);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::getListImpl(DBRequestInfo *r_)
{
    if(nullptr == r_) { return false; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef()); }
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
                applyFunction(
                    filterNames(r_->getSqlNames(r_->getTableFieldsInfo()), procedureFilterFields)
                    , procedureArgFunction
                    ).join(g_insertFieldSpliter)
                );
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    const QStringList refs = r_->getRefs();
    for(const QString &ref : refs)
    {
        const QString refBindName = QString(":") + refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : QString());
    }
    const QHash<QString, QVariant> extraFields = DBRequestInfo::apiExtraFields(r_->getExtraFields());
    for(QHash<QString, QVariant>::const_iterator it = std::begin(qAsConst(extraFields));
        it != std::end(qAsConst(extraFields))
        ; ++it
        )
    {
        const QString refBindName = QString(":") + refName(it.key());
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, it.value().toString());
    }
    const QString procedureConditions = procedureFields.contains(g_procedureConditionName)
            ? procedureFields.value(g_procedureConditionName).toString()
            : QString()
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
                            << equalToValueConditionsFromSqlNamesFromSqlNames(refs)
                            << equalToValueConditionsFromSqlNamesFromSqlNames(extraFields.keys())
                            ).join(" AND ");
    const QString conditionStr = hasCondition
            ? QString("WHERE %1")
              .arg(conditionCases.isEmpty()
                   ? procedureConditions
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
#if defined(TRACE_DB_USE)
    qDebug() << "select sql" << sqlRequest;
#endif
    bool sqlRes = true;
    if(hasCondition || !procedureArgs.isEmpty())
    {
        query.prepare(sqlRequest);
        for(const QString &ref : qAsConst(bindRefs))
        {
            const QString v = defValues.value(ref);
            query.bindValue(ref, v);
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << ref << v;
#endif
        }
        const QList<QString> procedureArgsKeys = procedureArgs.keys();
        for(const QString &key : procedureArgsKeys)
        {
            const QVariant v = procedureArgs.value(key);
            query.bindValue(key, v);
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << key << v;
#endif
        }
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
            for(const DBRequestInfo::JsonFieldInfo &fi : qAsConst(r_->getTableFieldsInfo()))
            {
                const QVariant val = query.value(fi.sqlName);
                if(val.isValid())
                {
                    jsonObj.insert(fi.jsonName, fi.jsonValue(val));
                }
            }
            jsonArray.push_back(jsonObj);
        } while(query.next());
    }
    r->addJsonResult(QJsonDocument(jsonArray));
    r->setProcessed(true);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    if(nullptr == r_) { return false; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemAppId(appId_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QSqlQuery findQuery(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef()); }
    const QHash<QString, QVariant> extraFields = DBRequestInfo::apiExtraFields(r_->getExtraFields());
    const QStringList refs = r_->getRefs();
    const QString fieldNames = (QStringList()
                                << refsNames(refs)
                                << refsNames(extraFields.keys())
                                << DBRequestInfo::getSqlNames(r_->getTableFieldsInfo())
                                ).join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : refs)
    {
        const QString refBindName = QString(":") + refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : QString());
    }
    for(QHash<QString, QVariant>::const_iterator it = std::begin(extraFields);
        it != std::end(extraFields)
        ; ++it
        )
    {
        const QString refBindName = QString(":") + refName(it.key());
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
    const auto fitId = std::find_if(std::begin(qAsConst(r_->getTableFieldsInfo())),
                                    std::end(qAsConst(r_->getTableFieldsInfo())),
                                    [](const DBRequestInfo::JsonFieldInfo &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    if(std::end(qAsConst(r_->getTableFieldsInfo())) != fitId)
    {
        idFieldJsonName = fitId->jsonName;
        idFieldSqlName = fitId->sqlName;
    }
    const QString sqlNextIdRequest = QString("SELECT MAX(%1) FROM %2 ;")
            .arg(idFieldSqlName, tableName);

    query.prepare(sqlRequest);
    for(const QString &bind : qAsConst(bindRefs))
    {
        const QString v = defValues.value(bind);
        query.bindValue(bind, v);
#if defined(TRACE_DB_DATA_BINDINGS)
        qDebug() << "bind" << bind << v;
#endif
    }
    int nextId = 1;
    for(const DBRequestInfo::JsonFieldInfo &bindInfo : qAsConst(r_->getTableFieldsInfo()))
    {
        if(bindInfo.idField)
        {
            if(!findQuery.exec(sqlNextIdRequest) && findQuery.lastError().type() != QSqlError::NoError)
            {
                const QSqlError err = findQuery.lastError();
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
    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
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
        for(const DBRequestInfo::JsonFieldInfo &bindInfo : qAsConst(r_->getTableFieldsInfo()))
        {
            if(bindInfo.idField)
            {
                values.insert(bindInfo.jsonName, QVariant::fromValue(nextId));
            }
        }
        r->addJsonResult(values);
    }
    query.finish();
    r->setProcessed(true);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    if(nullptr == r_) { return false; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemId(id_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef()); }

    QString idFieldJsonName;
    QString idFieldSqlName;
    QString idFieldSqlBindName;
    const auto fitId = std::find_if(std::begin(qAsConst(r_->getTableFieldsInfo())),
                                    std::end(qAsConst(r_->getTableFieldsInfo())),
                                    [](const DBRequestInfo::JsonFieldInfo &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    if(std::end(qAsConst(r_->getTableFieldsInfo())) != fitId)
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

    query.prepare(sqlRequest);
    for(const DBRequestInfo::JsonFieldInfo &bindInfo : qAsConst(r_->getTableFieldsInfo()))
    {
        const QVariant val = values_.value(bindInfo.jsonName);
        bindInfo.bind(query, val);
    }

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
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
    query.finish();
    r->setProcessed(true);
    return true;
}

bool LocalDataAPIDefaultCacheImpl::delItemImpl(const QVariant &id_, DBRequestInfo *r_)
{
    if(nullptr == r_) { return false; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemId(id_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef()); }

    QString idFieldJsonName;
    QString idFieldSqlName;
    QString idFieldSqlBindName;
    const auto fitId = std::find_if(std::begin(qAsConst(r_->getTableFieldsInfo())),
                                    std::end(qAsConst(r_->getTableFieldsInfo())),
                                    [](const DBRequestInfo::JsonFieldInfo &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    if(std::end(qAsConst(r_->getTableFieldsInfo())) != fitId)
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

    query.prepare(sqlRequest);
    fitId->bind(query, id_);

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
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
    query.finish();
    r->setProcessed(true);
    return true;
}


LocalDataAPICache::LocalDataAPICache(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
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

RequestData *LocalDataAPICache::emptyRequest(const QString &requestName_, const QVariant &itemAppId_, const QVariant &itemId_)
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
    const auto fit = std::find_if(std::begin(m_requests), std::end(m_requests),
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

QHash<QString, QVariant> LocalDataAPICache::merge(const QHash<QString, QVariant> &v1_, const QHash<QString, QVariant> &v2_)
{
    QHash<QString, QVariant> res;
    for(QHash<QString, QVariant>::const_iterator it = v2_.begin(); it != v2_.end(); ++it)
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
    for(QHash<QString, QVariant>::const_iterator it = v1_.begin(); it != v1_.end(); ++it)
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
    for(LocalDBRequest *r : m_requests)
    {
        if(r->isProcessed()) { res.push_back(r); }
    }
    for(const LocalDBRequest *r : res)
    {
        const auto fit = std::find(std::begin(m_requests), std::end(m_requests), r);
        m_requests.erase(fit);
    }

    for(LocalDBRequest *&r : res)
    {
        static QNetworkReply::NetworkError errCode = QNetworkReply::InternalServerError;
        static const QString errorCodeStr = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(errCode);
        static QNetworkReply::NetworkError noErrorCode = QNetworkReply::NoError;
        static const QString noErrorCodeStr = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(noErrorCode);
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

    const bool continueNotify = m_requests.isEmpty();
    if(continueNotify)
    {
        QTimer::singleShot(0, this, &LocalDataAPICache::makeResponses);
    }
}

ILocalDataAPI *LocalDataAPICache::chooseView(DBRequestInfo *r_)
{
    if(nullptr == r_) { return nullptr; }
    r_->setDefaultAPI(&m_defaultAPIImpl);

//    if(r_->getExtraFields().contains(g_procedureExtraFieldName) &&
//            r_->getExtraFields().value(g_procedureExtraFieldName).toHash().contains(g_procedureDefaultAPI))
//    {
//        // don't use views, use default API implementation
//        return nullptr;
//    }

    for(ILocalDataAPI *api : qAsConst(QMLObjectsBase::getInstance().getLocalDataAPIViews()))
    {
        if(nullptr != api && api->canProcess(r_)) { return api; }
    }

    return nullptr;
}
