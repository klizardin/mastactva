#include "localdataapinocache.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QSqlError>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"


//#define TRACE_DB_CREATION
//#define TRACE_DB_DATA_BINDINGS


bool LocalDataAPINoCache::SaveDBRequest::operator == (const RequestData *request_) const
{
    return nullptr != m_request && nullptr != request_ && m_request == request_;
}

void LocalDataAPINoCache::SaveDBRequest::setRequest(const RequestData *request_)
{
    m_request = request_;
}


LocalDataAPINoCache::LocalDataAPINoCache(QObject *parent_ /* = nullptr*/)
    : QObject(parent_)
{
    m_netAPI = QMLObjectsBase::getInstance().getNetAPI();

    QObject::connect(m_netAPI, SIGNAL(response(int, const QString &, RequestData *, const QJsonDocument &)),
                     this, SLOT(responseFromStartegySlot(int, const QString &, RequestData *, const QJsonDocument &)));
    QObject::connect(m_netAPI, SIGNAL(error(int, const QString &, const QJsonDocument &)),
                     this, SLOT(errorFromStartegySlot(int, const QString &, const QJsonDocument &)));
}

LocalDataAPINoCache::~LocalDataAPINoCache()
{
    cleanRequests();
}

void LocalDataAPINoCache::responseFromStartegySlot(
        int errorCode_,
        const QString &errorCodeStr_,
        RequestData *request_,
        const QJsonDocument &reply_
        )
{
    if(isSaveToDBMode())
    {
        const auto fit = std::find_if(std::begin(m_requests), std::end(m_requests),
                                      [request_](const SaveDBRequest *r_)->bool
        {
            return nullptr != request_ && nullptr != r_ && *r_ == request_;
        });
        if(std::end(m_requests) != fit)
        {
            const SaveDBRequest *r = *fit;
            m_requests.erase(fit);
            fillTable(r, reply_);
            delete r;
            r = nullptr;
        }
    }

    emit response(errorCode_, errorCodeStr_, request_, reply_);
}

void LocalDataAPINoCache::errorFromStartegySlot(
        int errorCode_,
        const QString &errorCodeStr_,
        const QJsonDocument &reply_
        )
{
    emit error(errorCode_, errorCodeStr_, reply_);
}

LocalDataAPINoCache *LocalDataAPINoCache::g_localDataAPI = nullptr;

void LocalDataAPINoCache::createInstance(QObject *parent_, NetAPI *netAPI_)
{
    if(nullptr != g_localDataAPI) { return; }
    g_localDataAPI = new LocalDataAPINoCache(parent_);
    g_localDataAPI->m_netAPI = netAPI_;
}

LocalDataAPINoCache *LocalDataAPINoCache::getInstance()
{
    return g_localDataAPI;
}

void LocalDataAPINoCache::startSave(const QString &savePath_)
{
    m_savePath = savePath_;
    QString dateStr = dateTimeToJsonString(QDateTime::currentDateTime());
    dateStr.replace(":", "_");
    dateStr.replace("-", "_");
    dateStr.replace(".", "_");
    m_dbNameRW = QString(g_dbNameRW) + dateStr + QString(g_dbNameExt);
    m_dbNameRO = QString(g_dbNameRO) + dateStr + QString(g_dbNameExt);
    cleanPath();
    createDB();
}

void LocalDataAPINoCache::endSave()
{
    m_databaseRW.close();
    m_databaseRO.close();
    m_savePath.clear();
}

void LocalDataAPINoCache::cleanRequests()
{
    for(auto *&p : m_requests)
    {
        delete p;
        p = nullptr;
    }
    m_requests.clear();
}

void LocalDataAPINoCache::cleanPath()
{
    QFile::remove(m_dbNameRW);
    QFile::remove(m_dbNameRO);
    QDirIterator fit(m_savePath, QStringList() << "*.*", QDir::NoFilter, QDirIterator::Subdirectories);
    while(fit.hasNext())
    {
        QFile f(fit.next());
        f.remove();
    }
}

void LocalDataAPINoCache::createDB()
{
    m_databaseRO = QSqlDatabase::addDatabase("QSQLITE", g_dbNameRO);
    m_databaseRO.setDatabaseName(m_dbNameRO);
    m_databaseRO.open();
    m_databaseRW = QSqlDatabase::addDatabase("QSQLITE", g_dbNameRW);
    m_databaseRW.setDatabaseName(m_dbNameRW);
    m_databaseRW.open();
}

bool LocalDataAPINoCache::isSaveToDBMode() const
{
    return !m_savePath.isEmpty();
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

inline QStringList textTypes(const QStringList &names_)
{
    QStringList res;
    for(const auto &s : names_)
    {
        res.push_back(s + QString(" ") + QString(g_sqlText));
    }
    return res;
}


void LocalDataAPINoCache::createTable(const SaveDBRequest * r_)
{
    if(nullptr == r_) { return; }
#if defined(TRACE_DB_CREATION)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef()); }
    QStringList tableFieldsNameTypePairs;
    for(const DBRequestInfo::JsonFieldInfo &fi : qAsConst(r_->getTableFieldsInfo()))
    {
        tableFieldsNameTypePairs.push_back(fi.sqlName + QString(" ") + fi.getSqlType());
    }
    const QString fieldsRequests = (QStringList()
                                    << textTypes(refsNames(r_->getRefs()))
                                    << textTypes(refsNames(r_->getExtraFields().keys()))
                                    << tableFieldsNameTypePairs
                                    ).join(g_insertFieldSpliter) +
            QString(g_insertFieldSpliter)
            ;
    const QString sqlRequest = QString("CREATE TABLE IF NOT EXISTS %1 ( %2 )")
            .arg(tableName, fieldsRequests.mid(0, fieldsRequests.length() - 2))
            ;
#if defined(TRACE_DB_CREATION)
    qDebug() << sqlRequest;
#endif
    if(!query.exec(sqlRequest))
    {
        const QSqlError err = query.lastError();
        qDebug() << err.driverText();
        qDebug() << err.databaseText();
        qDebug() << err.nativeErrorCode();
        qDebug() << err.text();
    }
    query.finish();
}

QStringList conditionsFromRefs(const QStringList &bindRefs_)
{
    QStringList res;
    for(const QString &ref : bindRefs_)
    {
        res.push_back(QString("%1=%2").arg(ref, DBRequestInfo::JsonFieldInfo::toBindName(ref)));
    }
    return res;
}

void LocalDataAPINoCache::fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_)
{
    if(nullptr == r_ || reply_.isEmpty()) { return; }
#if defined(TRACE_DB_CREATION)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    if(!r_->getReadonly()) { return; } // don't save data for RW tables
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
    QString idFieldSQlBindName;
    const auto fitId = std::find_if(std::begin(qAsConst(r_->getTableFieldsInfo())), std::end(qAsConst(r_->getTableFieldsInfo())),
                                    [](const DBRequestInfo::JsonFieldInfo &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    if(std::end(qAsConst(r_->getTableFieldsInfo())) != fitId)
    {
        idFieldJsonName = fitId->jsonName;
        idFieldSqlName = fitId->sqlName;
        idFieldSQlBindName = fitId->getBindName();
    }
    QString conditionStr = (QStringList()
                            << QString("%1=%2").arg(idFieldSqlName, idFieldSQlBindName)
                            << conditionsFromRefs(bindRefs)
                            ).join(" AND ");
    const QString sqlExistsRequest = QString("SELECT * FROM %1 WHERE %2 LIMIT 1")
            .arg(tableName, conditionStr);
#if defined(TRACE_DB_CREATION)
    qDebug() << sqlRequest;
    qDebug() << sqlExistsRequest;
#endif
    int i = 0;
    for(i = 0; ; i++)
    {
        QJsonValue itemJV = reply_[i];
        if(itemJV.isUndefined()) { break; }
        if(0 == i)
        {
            query.prepare(sqlRequest);
            findQuery.prepare(sqlExistsRequest);
        }
        if(!idFieldSqlName.isEmpty())
        {
            const QJsonValue valueJV = itemJV[idFieldJsonName];
            const QString v = DBRequestInfo::JsonFieldInfo::toString(valueJV);
            findQuery.bindValue(idFieldSQlBindName, v);
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << idFieldSQlBindName << v;
#endif
            for(const QString &ref : qAsConst(bindRefs))
            {
                const QString v = defValues.value(ref);
                findQuery.bindValue(DBRequestInfo::JsonFieldInfo::toBindName(ref), v);
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << ref << v;
#endif
            }
            if(!findQuery.exec())
            {
                const QSqlError err = query.lastError();
                qDebug() << err.driverText();
                qDebug() << err.databaseText();
                qDebug() << err.nativeErrorCode();
                qDebug() << err.text();
            }
            else
            {
                if(findQuery.first()) { continue; } // id value already exists
            }
        }

        for(const QString &bind : qAsConst(bindRefs))
        {
            const QString v = defValues.value(bind);
            query.bindValue(bind, v);
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << bind << v;
#endif
        }
        for(const DBRequestInfo::JsonFieldInfo &bindInfo : qAsConst(r_->getTableFieldsInfo()))
        {
            const QJsonValue valueJV = itemJV[bindInfo.jsonName];
            bindInfo.bind(query, valueJV);
        }
        if(!query.exec())
        {
            const QSqlError err = query.lastError();
            qDebug() << err.driverText();
            qDebug() << err.databaseText();
            qDebug() << err.nativeErrorCode();
            qDebug() << err.text();
        }
    }
    if(i > 0)
    {
        query.finish();
    }
}
