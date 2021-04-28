#include "localdataapinocache.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QSqlError>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/timeutils.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/jsonutils.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


bool LocalDataAPINoCacheImpl::canProcess(const DBRequestBase *r_) const
{
    Q_UNUSED(r_);
    return true;
}

bool LocalDataAPINoCacheImpl::getListImpl(DBRequestBase *r_)
{
    if(nullptr == r_) { return false; }
#if defined(TRACE_DB_CREATION)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery query(db);
    const QString tableName = db::tableName(r_->getTableName(), r_->getCurrentRef());
    QStringList tableFieldsNameTypePairs;
    for(const db::JsonSqlField &fi : qAsConst(r_->getTableFieldsInfo()))
    {
        tableFieldsNameTypePairs.push_back(fi.getSqlName() + QString(" ") + fi.getSqlType());
    }
    const QStringList refs = r_->getRefs();
    const QHash<QString, QVariant> extraFields = DBRequestBase::apiExtraFields(r_->getExtraFields());
    const QString fieldsRequests = (QStringList()
                                    << db::textTypes(db::refNames(refs))
                                    << db::textTypes(db::refNames(extraFields.keys()))
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
                                          DBRequestBase *r_)
{
    Q_UNUSED(appId_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool LocalDataAPINoCacheImpl::setItemImpl(const QVariant &id_,
                                          const QHash<QString, QVariant> &values_,
                                          DBRequestBase *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool LocalDataAPINoCacheImpl::delItemImpl(const QVariant &id_, DBRequestBase *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(r_);
    return false;
}

LocalDataAPINoCache::SaveDBRequest::SaveDBRequest()
    :DBRequestBase(g_noCachAPI)
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
    QString dateStr = date_time::dateTimeToJsonString(QDateTime::currentDateTime());
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
    const QString tableName = db::tableName(r_->getTableName(), r_->getCurrentRef());

    const QStringList refs = r_->getRefs();
    const QHash<QString, QVariant> extraFields = DBRequestBase::apiExtraFields(r_->getExtraFields());
    const QString fieldNames = (QStringList()
                                << db::refNames(refs)
                                << db::refNames(extraFields.keys())
                                << db::getSqlNames(r_->getTableFieldsInfo())
                                ).join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : qAsConst(refs))
    {
        const QString refBindName = db::toBindName(db::refName(ref));
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : QString());
    }
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(extraFields));
        it != std::cend(qAsConst(extraFields))
        ; ++it
        )
    {
        const QString refBindName = db::toBindName(db::refName(it.key()));
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, it.value().toString());
    }
    const QString fieldNamesBindings = (QStringList()
                                        << bindRefs
                                        << db::getBindSqlNames(r_->getTableFieldsInfo())
                                        ).join(g_insertFieldSpliter);
    const QString sqlRequest = QString("INSERT INTO %1 ( %2 ) VALUES ( %3 ) ;")
            .arg(tableName, fieldNames, fieldNamesBindings);

    QString idFieldJsonName;
    QString idFieldSqlName;
    QString idFieldSQlBindName;
    const auto fitId = std::find_if(std::cbegin(qAsConst(r_->getTableFieldsInfo())),
                                    std::cend(qAsConst(r_->getTableFieldsInfo())),
                                    [](const db::JsonSqlField &bindInfo)->bool
    {
        return bindInfo.isIdField();
    });
    const bool anyIdFields = !(refs.empty()) || std::end(qAsConst(r_->getTableFieldsInfo())) != fitId;
    QStringList conditionsList;
    if(std::cend(qAsConst(r_->getTableFieldsInfo())) != fitId)
    {
        idFieldJsonName = fitId->getJsonName();
        idFieldSqlName = fitId->getSqlName();
        idFieldSQlBindName = fitId->getBindSqlName();
        conditionsList << QString("%1=%2").arg(idFieldSqlName, idFieldSQlBindName);
    }
    const QString conditionStr = (conditionsList
                            << db::equalToValueConditionListFromSqlNameList(refs)
                            << db::equalToValueConditionListFromSqlNameList(extraFields.keys())
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
                const int v = json::toInt(valueJV, layout::JsonTypesEn::jt_undefined);
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
        for(const db::JsonSqlField &bindInfo : qAsConst(r_->getTableFieldsInfo()))
        {
            const QJsonValue valueJV = itemJV[bindInfo.getJsonName()];
            db::bind(bindInfo, query, valueJV);
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

ILocalDataAPI *LocalDataAPINoCache::chooseAPI(DBRequestBase * r_)
{
    if(nullptr == r_) { return nullptr; }
    r_->setDefaultAPI(&m_defaultAPIImpl);

    for(ILocalDataAPI *api : qAsConst(QMLObjectsBase::getInstance().getLocalDataAPIViews()))
    {
        if(nullptr != api && api->canProcess(r_)) { return api; }
    }

    return nullptr;
}
