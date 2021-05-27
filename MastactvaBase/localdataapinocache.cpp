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
            return request_ && r_ && *r_ == request_;
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
    if(g_localDataAPI)
    {
        return;
    }

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
        db::ISqlQuery *findQuery_,
        QJsonValue &replayItem_,
        const db::JsonSqlFieldsList &fields_,
        const db::JsonSqlFieldAndValuesList refsValues_,
        const db::JsonSqlFieldsList::const_iterator &idField_
        )
{
    if(!findQuery_)
    {
        return false;
    }

    if(db::idFieldExist(idField_, fields_))
    {
        db::bind(*idField_, findQuery_, replayItem_[idField_->getJsonName()]);
    }
    db::bind(refsValues_, findQuery_);

    if(!findQuery_->exec() && findQuery_->lastError().type() != QSqlError::NoError)
    {
        const QSqlError err = findQuery_->lastError();
        qDebug() << "sql error " << err.text();
        return false;
    }
    else
    {
        return findQuery_->first();
    }
}

void LocalDataAPINoCache::insertItem(
        db::ISqlQuery *insertQuery_,
        QJsonValue &replayItem_,
        const db::JsonSqlFieldsList &fields_,
        const db::JsonSqlFieldAndValuesList &refsValues_
        )
{
    db::bind(refsValues_, insertQuery_);
    db::bind(fields_, replayItem_, insertQuery_);

    if(!insertQuery_->exec())
    {
        const QSqlError err = insertQuery_->lastError();
        qDebug() << "sql error " << err.text();
    }
}

void LocalDataAPINoCache::fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_)
{
    if(!r_ || reply_.isEmpty())
    {
        return;
    }

#if defined(TRACE_DB_CREATION)
    qDebug() << "readonly " << r_->getReadonly();
#endif
    // function fill table with replay datas but not add duplicates
    // to find duplicates is used db::getFindSqlRequest
    // duplicates are searched by ref fields and id field

    if(!r_->getReadonly()) { return; } // don't save data for RW tables

    const QStringList refs = r_->getRefs();
    const QHash<QString, QVariant> extraFields = DBRequestBase::apiExtraFields(r_->getExtraFields());

    const db::JsonSqlFieldAndValuesList refsValues = db::createRefValuesList(
                refs,
                extraFields.keys(),
                extraFields,
                r_->getCurrentRef(),
                r_->getIdField()
                );

    const QString insertSqlRequest = db::getInsertSqlRequest(
                r_->getTableName(),
                r_->getCurrentRef(),
                r_->getTableFieldsInfo(),
                refs,
                extraFields.keys()
                );

    const auto idField = db::findIdField(r_->getTableFieldsInfo());
    const bool anyIdFields = !refs.empty() || db::idFieldExist(idField, r_->getTableFieldsInfo());

    const QString findSqlRequest = db::getFindSqlRequest(
                r_->getTableName(),
                r_->getCurrentRef(),
                r_->getTableFieldsInfo(),   // use only id field
                refs,
                extraFields.keys()
                );

#if defined(TRACE_DB_CREATION)
    qDebug() << "insert sql" << insertSqlRequest;
    qDebug() << "find sql" << findSqlRequest;
#endif

    auto queriesPair = m_defaultAPIImpl.getRequestsPair(r_);
    auto insertQuery(std::move(queriesPair.first));
    auto findQuery(std::move(queriesPair.second));
    for(int i = 0; ; i++)
    {
        QJsonValue replayItem = reply_[i];
        if(replayItem.isUndefined())
        {
            // no (more) data, exit
            break;
        }

        if(0 == i)
        {
            // create if any data exists
            insertQuery->prepare(insertSqlRequest);
            findQuery->prepare(findSqlRequest);
        }

        if(anyIdFields)
        {
            if(hasDuplicate(
                        findQuery.get(),
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

        insertItem(
                    insertQuery.get(),
                    replayItem,
                    r_->getTableFieldsInfo(),
                    refsValues
                    );
    }
}

ILocalDataAPI *LocalDataAPINoCache::chooseAPI(DBRequestBase * r_)
{
    if(!r_)
    {
        return nullptr;
    }

    r_->setDefaultAPI(&m_defaultAPIImpl);

    for(ILocalDataAPI *api : qAsConst(QMLObjectsBase::getInstance().getLocalDataAPIViews()))
    {
        if(api && api->canProcess(r_))
        {
            return api;
        }
    }

    return nullptr;
}
