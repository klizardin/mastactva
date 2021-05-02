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

    const QString sqlRequest = db::getCreateTableSqlRequest(
                r_->getTableName(),
                r_->getCurrentRef(),
                r_->getTableFieldsInfo(),
                r_->getRefs(),
                DBRequestBase::apiExtraFields(r_->getExtraFields()).keys()
                );
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

bool LocalDataAPINoCache::hasDuplicate(
        QSqlQuery &findQuery_,
        QJsonValue &replayItem_,
        const db::JsonSqlFieldsList &fields_,
        const db::JsonSqlFieldAndValuesList refsValues_,
        const db::JsonSqlFieldsList::const_iterator &idField_
        ) const
{
    if(db::idFieldExist(idField_, fields_))
    {
        db::bind(*idField_, findQuery_, replayItem_[idField_->getJsonName()]);
    }
    db::bind(refsValues_, findQuery_);

    if(!findQuery_.exec() && findQuery_.lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = findQuery_.lastError();
        qDebug() << "sql error " << err.text();
        return false;
    }
    else
    {
        return findQuery_.first();
    }
}

void LocalDataAPINoCache::fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_)
{
    if(nullptr == r_ || reply_.isEmpty()) { return; }
#if defined(TRACE_DB_CREATION)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    // function fill table with replay datas but not add duplicates
    // to find duplicates is used db::getFindSqlRequest
    // duplicates are searched by ref fields and id field

    if(!r_->getReadonly()) { return; } // don't save data for RW tables
    QSqlDatabase db = QSqlDatabase::database(r_->getReadonly() ? g_dbNameRO : g_dbNameRW);
    QSqlQuery insertQuery(db);
    QSqlQuery findQuery(db);

    const QStringList refs = r_->getRefs();
    const QHash<QString, QVariant> extraFields = DBRequestBase::apiExtraFields(r_->getExtraFields());

    const db::JsonSqlFieldAndValuesList refsValues = db::createRefValuesList(
                refs,
                extraFields.keys(),
                extraFields,
                r_->getCurrentRef(),
                r_->getIdField()
                );

    const QString sqlRequest = db::getInsertSqlRequest(
                r_->getTableName(),
                r_->getCurrentRef(),
                r_->getTableFieldsInfo(),
                refs,
                extraFields.keys()
                );

    const auto idField = db::findIdField(r_->getTableFieldsInfo());
    const bool anyIdFields = !(refs.empty()) || db::idFieldExist(idField, r_->getTableFieldsInfo());

    const QString sqlExistsRequest = db::getFindSqlRequest(
                r_->getTableName(),
                r_->getCurrentRef(),
                r_->getTableFieldsInfo(),   // use only id field
                refs,
                extraFields.keys()
                );

#if defined(TRACE_DB_CREATION)
    qDebug() << "insert sql" << sqlRequest;
    qDebug() << "find sql" << sqlExistsRequest;
#endif

    int i = 0;
    for(i = 0; ; i++)
    {
        QJsonValue replayItem = reply_[i];
        if(replayItem.isUndefined())
        {
            break;
        }

        if(0 == i)
        {
            // create if any data exists
            insertQuery.prepare(sqlRequest);
            findQuery.prepare(sqlExistsRequest);
        }

        if(anyIdFields)
        {
            if(hasDuplicate(
                        findQuery,
                        replayItem,
                        r_->getTableFieldsInfo(),
                        refsValues,
                        idField
                        )
                    )
            {
#if defined(TRACE_DB_DATA_BINDINGS)
                qDebug() << "skip row";
#endif
                // id value already exists
                continue;
            }
        }

        // fill insert query
        db::bind(refsValues, insertQuery);
        db::bind(r_->getTableFieldsInfo(), replayItem, insertQuery);

        if(!insertQuery.exec())
        {
            const QSqlError err = insertQuery.lastError();
            qDebug() << "sql error " << err.text();
        }
    }
    if(i > 0)
    {
        findQuery.finish();
        insertQuery.finish();
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
