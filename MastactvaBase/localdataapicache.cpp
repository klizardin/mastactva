#include "localdataapicache.h"
#include "../MastactvaBase/utils.h"


#define TRACE_DB_USE


void LocalDataAPICache::LocalDBRequest::addJsonResult(const QJsonDocument &doc_)
{
    m_doc = doc_;
}

void LocalDataAPICache::LocalDBRequest::addJsonResult(const QHash<QString, QVariant> &values_)
{
    QJsonArray array;

    QJsonObject obj;
    for(const DBRequestInfo::JsonFieldInfo &bindInfo : qAsConst(getTableFieldsInfo()))
    {
        const QVariant val = values_.contains(bindInfo.jsonName) ? values_.value(bindInfo.jsonName) : QVariant();
        obj.insert(bindInfo.jsonName, bindInfo.jsonValue(val));
    }
    array.push_back(obj);
    m_doc = QJsonDocument(array);
}

void LocalDataAPICache::LocalDBRequest::setError(bool error_)
{
    m_error = error_;
}

bool LocalDataAPICache::LocalDBRequest::error() const
{
    return m_error;
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


void LocalDataAPICache::createInstance(QObject *parent_)
{
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
    m_requests.push_back(r);
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

QStringList conditionsFromSqlNames(const QStringList &names_)
{
    QStringList res;
    for(const QString &sqlName : names_)
    {
        const QString ref = refName(sqlName);
        res.push_back(QString("%1=%2").arg(ref, DBRequestInfo::JsonFieldInfo::toBindName(ref)));
    }
    return res;
}

RequestData *LocalDataAPICache::getListImpl(const QString& requestName_, LocalDBRequest *r_)
{
    if(nullptr == r_) { return nullptr; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    r_->setRequestName(requestName_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef()); }
    const QString fieldsRequests = (r_->getSqlNames(r_->getTableFieldsInfo())).join(g_insertFieldSpliter) + QString(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : qAsConst(r_->getRefs()))
    {
        const QString refBindName = QString(":") + refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : QString());
    }
    for(QHash<QString, QVariant>::const_iterator it = std::begin(r_->getExtraFields());
        it != std::end(r_->getExtraFields())
        ; ++it
        )
    {
        const QString refBindName = QString(":") + refName(it.key());
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, it.value().toString());
    }
    const bool hasCondition = !(r_->getRefs().isEmpty()) || !(r_->getExtraFields().isEmpty());
    const QString conditionCases = (QStringList()
                            << conditionsFromSqlNames(r_->getRefs())
                            << conditionsFromSqlNames(r_->getExtraFields().keys())
                            ).join(" AND ");
    const QString conditionStr = hasCondition
            ? QString("WHERE %1").arg(conditionCases)
            : QString()
            ;
    const QString sqlRequest = QString("SELECT %1 FROM %2 %3")
            .arg(fieldsRequests.mid(0, fieldsRequests.length() - 2), tableName, conditionStr)
            ;
#if defined(TRACE_DB_USE)
    qDebug() << "select sql" << sqlRequest;
#endif
    bool sqlRes = true;
    if(!hasCondition)
    {
        sqlRes = query.exec(sqlRequest);
    }
    else
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
        sqlRes = query.exec();
    }

    QJsonArray jsonArray;
    if(!sqlRes && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        //qDebug() << err.driverText();
        //qDebug() << err.databaseText();
        //qDebug() << err.nativeErrorCode();
        qDebug() << "sql error" << err.text();
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
        r_->setError(true);
    }
    else if(query.first())
    {
        do
        {
            QJsonObject jsonObj;
            for(const DBRequestInfo::JsonFieldInfo &fi : qAsConst(r_->getTableFieldsInfo()))
            {
                const QVariant val = query.value(fi.sqlName);
                jsonObj.insert(fi.jsonName, fi.jsonValue(val));
            }
            jsonArray.push_back(jsonObj);
        } while(query.next());
    }
    r_->addJsonResult(QJsonDocument(jsonArray));
    m_requests.push_back(r_);
    return r_;
}

RequestData *LocalDataAPICache::addItemImpl(const QString& requestName_, const QVariant &appId_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
{
    if(nullptr == r_) { return nullptr; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    r_->setRequestName(requestName_);
    r_->setItemAppId(appId_);

    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QSqlQuery findQuery(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef()); }
    const QString fieldNames = (QStringList()
                                << refsNames(r_->getRefs())
                                << refsNames(r_->getExtraFields().keys())
                                << DBRequestInfo::getSqlNames(r_->getTableFieldsInfo())
                                ).join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : qAsConst(r_->getRefs()))
    {
        const QString refBindName = QString(":") + refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : QString());
    }
    for(QHash<QString, QVariant>::const_iterator it = std::begin(r_->getExtraFields());
        it != std::end(r_->getExtraFields())
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
    const QString sqlRequest = QString("INSERT INTO %1 ( %2 ) VALUES ( %3 )")
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
    const QString sqlNextIdRequest = QString("SELECT MAX(%1) FROM %2")
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
                //qDebug() << err.driverText();
                //qDebug() << err.databaseText();
                //qDebug() << err.nativeErrorCode();
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
        //qDebug() << err.driverText();
        //qDebug() << err.databaseText();
        //qDebug() << err.nativeErrorCode();
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
        r_->setError(true);
        r_->addJsonResult(QJsonDocument(jsonArray));
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
        r_->addJsonResult(values);
    }
    query.finish();
    m_requests.push_back(r_);
    return r_;
}

RequestData *LocalDataAPICache::setItemImpl(const QString& requestName_, const QVariant &id_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
{
    if(nullptr == r_) { return nullptr; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    r_->setRequestName(requestName_);
    r_->setItemId(id_);

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
        return nullptr;
    }
    const QStringList setNames = DBRequestInfo::getSetNames(r_->getTableFieldsInfo());
    const QString setStr = setNames.join(g_insertFieldSpliter);

    const QString sqlRequest = QString("UPDATE %1 SET %2 WHERE %3=%4")
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
        //qDebug() << err.driverText();
        //qDebug() << err.databaseText();
        //qDebug() << err.nativeErrorCode();
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
        r_->setError(true);
        r_->addJsonResult(QJsonDocument(jsonArray));
    }
    else
    {
        r_->addJsonResult(values_);
    }
    query.finish();
    m_requests.push_back(r_);
    return r_;
}

RequestData *LocalDataAPICache::delItemImpl(const QString& requestName_, const QVariant &id_, LocalDBRequest *r_)
{
    if(nullptr == r_) { return nullptr; }
#if defined(TRACE_DB_USE)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    r_->setRequestName(requestName_);
    r_->setItemId(id_);

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
        return nullptr;
    }

    const QString sqlRequest = QString("DELETE FROM %1 WHERE %3=%4")
            .arg(tableName, idFieldSqlName, idFieldSqlBindName);

    query.prepare(sqlRequest);
    fitId->bind(query, id_);

    if(!query.exec() && query.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = query.lastError();
        //qDebug() << err.driverText();
        //qDebug() << err.databaseText();
        //qDebug() << err.nativeErrorCode();
        qDebug() << "sql error " << err.text();

        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue(err.text()));
        jsonArray.push_back(jsonObj);
        r_->setError(true);
        r_->addJsonResult(QJsonDocument(jsonArray));
    }
    else
    {
        r_->addJsonResult(QJsonDocument(QJsonArray()));
    }
    query.finish();
    m_requests.push_back(r_);
    return r_;
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
