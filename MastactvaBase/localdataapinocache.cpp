#include "localdataapinocache.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QSqlError>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


inline QString refName(const QString &ref_)
{
    return QString(g_refPrefix) + ref_;
}

inline QStringList refsNames(const QStringList &refs_)
{
    QStringList res;
    for(const auto &s : qAsConst(refs_))
    {
        res.push_back(refName(s));
    }
    return res;
}

inline QStringList textTypes(const QStringList &names_)
{
    QStringList res;
    for(const auto &s : qAsConst(names_))
    {
        res.push_back(s + QString(" ") + QString(g_sqlText));
    }
    return res;
}


bool LocalDataAPINoCacheImpl::canProcess(const DBRequestInfo *r_) const
{
    Q_UNUSED(r_);
    return true;
}

bool LocalDataAPINoCacheImpl::getListImpl(DBRequestInfo *r_)
{
    if(nullptr == r_) { return false; }
#if defined(TRACE_DB_CREATION)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty())
    {
        tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef());
    }
    QStringList tableFieldsNameTypePairs;
    for(const DBRequestInfo::JsonFieldInfo &fi : qAsConst(r_->getTableFieldsInfo()))
    {
        tableFieldsNameTypePairs.push_back(fi.sqlName + QString(" ") + fi.getSqlType());
    }
    const QStringList refs = r_->getRefs();
    const QHash<QString, QVariant> extraFields = DBRequestInfo::apiExtraFields(r_->getExtraFields());
    const QString fieldsRequests = (QStringList()
                                    << textTypes(refsNames(refs))
                                    << textTypes(refsNames(extraFields.keys()))
                                    << tableFieldsNameTypePairs
                                    ).join(g_insertFieldSpliter)
            ;
    const QString sqlRequest = QString("CREATE TABLE IF NOT EXISTS %1 ( %2 ) ;")
            .arg(tableName, fieldsRequests)
            ;
#if defined(TRACE_DB_CREATION)
    qDebug() << "create sql" << sqlRequest;
#endif
    if(!query.exec(sqlRequest))
    {
        const QSqlError err = query.lastError();
        qDebug() << "sql error "  << err.text();
    }
    query.finish();
    r_->setProcessed(true);
    return true;
}

bool LocalDataAPINoCacheImpl::addItemImpl(const QVariant &appId_,
                                          const QHash<QString, QVariant> &values_,
                                          DBRequestInfo *r_)
{
    Q_UNUSED(appId_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool LocalDataAPINoCacheImpl::setItemImpl(const QVariant &id_,
                                          const QHash<QString, QVariant> &values_,
                                          DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool LocalDataAPINoCacheImpl::delItemImpl(const QVariant &id_, DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(r_);
    return false;
}

LocalDataAPINoCache::SaveDBRequest::SaveDBRequest()
    :DBRequestInfo(g_noCachAPI)
{
}

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
#if defined(TRACE_THREADS)
    qDebug() << "LocalDataAPINoCache::LocalDataAPINoCache()" << QThread::currentThread() << QThread::currentThreadId();
#endif
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
    if(!m_dbNameRW.isEmpty())
    {
        QFile::remove(m_dbNameRW);
    }
    if(!m_dbNameRO.isEmpty())
    {
        QFile::remove(m_dbNameRO);
    }
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


QStringList conditionsFromSqlNamesames(const QStringList &refs_)
{
    QStringList res;
    for(const QString &sqlName : qAsConst(refs_))
    {
        const QString ref = refName(sqlName);
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
    if(!r_->getCurrentRef().isEmpty())
    {
        tableName += QString(g_splitTableRef) + DBRequestInfo::namingConversion(r_->getCurrentRef());
    }
    const QStringList refs = r_->getRefs();
    const QHash<QString, QVariant> extraFields = DBRequestInfo::apiExtraFields(r_->getExtraFields());
    const QString fieldNames = (QStringList()
                                << refsNames(refs)
                                << refsNames(extraFields.keys())
                                << DBRequestInfo::getSqlNames(r_->getTableFieldsInfo())
                                ).join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : qAsConst(refs))
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
    QString idFieldSQlBindName;
    const auto fitId = std::find_if(std::begin(qAsConst(r_->getTableFieldsInfo())),
                                    std::end(qAsConst(r_->getTableFieldsInfo())),
                                    [](const DBRequestInfo::JsonFieldInfo &bindInfo)->bool
    {
        return bindInfo.idField;
    });
    const bool anyIdFields = !(refs.empty()) || std::end(qAsConst(r_->getTableFieldsInfo())) != fitId;
    QStringList conditionsList;
    if(std::end(qAsConst(r_->getTableFieldsInfo())) != fitId)
    {
        idFieldJsonName = fitId->jsonName;
        idFieldSqlName = fitId->sqlName;
        idFieldSQlBindName = fitId->getBindName();
        conditionsList << QString("%1=%2").arg(idFieldSqlName, idFieldSQlBindName);
    }
    const QString conditionStr = (conditionsList
                            << conditionsFromSqlNamesames(refs)
                            << conditionsFromSqlNamesames(extraFields.keys())
                            ).join(" AND ");
    const QString sqlExistsRequest = QString("SELECT * FROM %1 WHERE %2 LIMIT 1 ;")
            .arg(tableName, conditionStr);
#if defined(TRACE_DB_CREATION)
    qDebug() << "insert sql" << sqlRequest;
    qDebug() << "find sql" << sqlExistsRequest;
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
        if(anyIdFields)
        {
            if(!idFieldJsonName.isEmpty())
            {
                const QJsonValue valueJV = itemJV[idFieldJsonName];
                const int v = DBRequestInfo::JsonFieldInfo::toInt(valueJV, layout::JsonTypesEn::jt_undefined);
                findQuery.bindValue(idFieldSQlBindName, v);
#if defined(TRACE_DB_DATA_BINDINGS)
                qDebug() << "bind find" << idFieldSQlBindName << v;
#endif
            }

            for(const QString &ref : qAsConst(bindRefs))
            {
                const QString v = defValues.value(ref);
                findQuery.bindValue(ref, v);
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind find" << ref << v;
#endif
            }
            if(!findQuery.exec() && query.lastError().type() != QSqlError::NoError)
            {
                const QSqlError err = query.lastError();
                qDebug() << "sql error " << err.text();
            }
            else if(findQuery.first())
            {
#if defined(TRACE_DB_DATA_BINDINGS)
                qDebug() << "skip row";
#endif
                // id value already exists
                continue;
            }
        }

        for(const QString &bind : qAsConst(bindRefs))
        {
            const QString v = defValues.value(bind);
            query.bindValue(bind, v);
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << bind << v;
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
            qDebug() << "sql error " << err.text();
        }
    }
    if(i > 0)
    {
        findQuery.finish();
        query.finish();
    }
}

ILocalDataAPI *LocalDataAPINoCache::chooseAPI(DBRequestInfo * r_)
{
    if(nullptr == r_) { return nullptr; }
    r_->setDefaultAPI(&m_defaultAPIImpl);

    for(ILocalDataAPI *api : qAsConst(QMLObjectsBase::getInstance().getLocalDataAPIViews()))
    {
        if(nullptr != api && api->canProcess(r_)) { return api; }
    }

    return nullptr;
}
