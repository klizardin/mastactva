#include "localdataapicache.h"
#include "../MastactvaBase/utils.h"


#define TRACE_DB_USE


void LocalDataAPICache::LocalDBRequest::addJsonResult(const QJsonDocument &doc_)
{
    m_doc = doc_;
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
    const QString currentRefName = refName(r_->getCurrentRef());
    const QString currentRefBindName = DBRequestInfo::JsonFieldInfo::toBindName(currentRefName);
    const QString condition = !r_->getCurrentRef().isEmpty()
            ? QString("WHERE %1==%2").arg(currentRefName, currentRefBindName)
            : QString()
            ;
    const QString sqlRequest = QString("SELECT %1 FROM %2 %3")
            .arg(fieldsRequests.mid(0, fieldsRequests.length() - 2), tableName, condition)
            ;
#if defined(TRACE_DB_USE)
    qDebug() << sqlRequest;
#endif
    bool sqlRes = true;
    if(r_->getCurrentRef().isEmpty())
    {
        sqlRes = query.exec(sqlRequest);
    }
    else
    {
        query.prepare(sqlRequest);
        query.bindValue(currentRefBindName, r_->getIdField().toString());
        sqlRes = query.exec();
    }

    QJsonArray jsonArray;
    if(!sqlRes)
    {
        const QSqlError err = query.lastError();
        qDebug() << err.driverText();
        qDebug() << err.databaseText();
        qDebug() << err.nativeErrorCode();
        qDebug() << err.text();
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

RequestData *LocalDataAPICache::addItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
{
    return nullptr;
}

RequestData *LocalDataAPICache::setItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
{
    return nullptr;
}

RequestData *LocalDataAPICache::delItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &id_, LocalDBRequest *r_)
{
    return nullptr;
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
