#include "localdataapinocache.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include "../MastactvaBase/qmlobjects.h"


static const char *g_dbName = "mastactva.db3";


LocalDataAPINoCache::SaveDBRequest::SaveDBRequest(const RequestData *request_)
{
    m_request = request_;
}

bool LocalDataAPINoCache::SaveDBRequest::operator == (const RequestData *request_) const
{
    return nullptr != m_request && nullptr != request_ && m_request == request_;
}

const QString &LocalDataAPINoCache::SaveDBRequest::getTableName() const
{
    return m_tableName;
}

void LocalDataAPINoCache::SaveDBRequest::setTableName(const QString &tableName_)
{
    m_tableName = tableName_;
}

const QStringList &LocalDataAPINoCache::SaveDBRequest::getTableFieldNames() const
{
    return m_tableFieldNames;
}

void LocalDataAPINoCache::SaveDBRequest::setTableFieldNames(const QStringList &tableFiledNames_)
{
    m_tableFieldNames = tableFiledNames_;
}

const QStringList &LocalDataAPINoCache::SaveDBRequest::getRefs() const
{
    return m_refs;
}

void LocalDataAPINoCache::SaveDBRequest::setRefs(const QStringList &refs_)
{
    m_refs = refs_;
}

const QString &LocalDataAPINoCache::SaveDBRequest::getCurrentRef() const
{
    return m_currentRef;
}

void LocalDataAPINoCache::SaveDBRequest::setCurrentRef(const QString &currentRef_)
{
    m_currentRef = currentRef_;
}

const QVariant &LocalDataAPINoCache::SaveDBRequest::getIdField() const
{
    return m_idField;
}

void LocalDataAPINoCache::SaveDBRequest::setIdField(const QVariant &idField_)
{
    m_idField = idField_;
}

const QHash<QString, QString> &LocalDataAPINoCache::SaveDBRequest::getFieldToJsonNames() const
{
    return m_fieldToJsonNames;
}

void LocalDataAPINoCache::SaveDBRequest::setFieldToJsonNames(const QHash<QString, QString> & fieldToJson_)
{
    m_fieldToJsonNames = fieldToJson_;
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
    cleanPath();
    createDB();
}

void LocalDataAPINoCache::endSave()
{
    m_database.close();
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
    QFile::remove(g_dbName);
    QDirIterator fit(m_savePath, QStringList() << "*.*", QDir::NoFilter, QDirIterator::Subdirectories);
    while(fit.hasNext())
    {
        QFile f(fit.next());
        f.remove();
    }
}

void LocalDataAPINoCache::createDB()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(g_dbName);
    m_database.open();
}

bool LocalDataAPINoCache::isSaveToDBMode() const
{
    return !m_savePath.isEmpty();
}


static const char * g_createFieldSpliter = " TEXT , ";


void LocalDataAPINoCache::createTable(
        const QString &tableName_,
        const QStringList &tableFieldNames_,
        const QStringList &refs_
        )
{
    QSqlQuery query = QSqlQuery(m_database);
    const QString fieldsRequests = refs_.join(g_createFieldSpliter) + tableFieldNames_.join(g_createFieldSpliter);
    const QString sqlRequest = QString("CREATE TABLE IF NOT EXISTS %1 ( %2 )")
            .arg(tableName_, fieldsRequests.mid(0, fieldsRequests.length() - 2))
            ;
    query.exec(sqlRequest);
}


static const char * g_insertFieldSpliter = " , ";


void LocalDataAPINoCache::fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_)
{
    if(nullptr == r_ || reply_.isEmpty()) { return; }
    QSqlQuery query = QSqlQuery(m_database);
    const QString fieldNames = r_->getRefs().join(g_insertFieldSpliter) + r_->getTableFieldNames().join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : r_->getRefs())
    {
        bindRefs.push_back(QString(":") + ref);
        defValues.insert(ref, ref == r_->getCurrentRef() ? r_->getIdField().toString() : "");
    }
    QStringList bindFields;
    for(const QString &fieldName : r_->getTableFieldNames())
    {
        bindFields.push_back(QString(":") + fieldName);
    }
    const QString fieldNamesBindings = bindRefs.join(g_insertFieldSpliter) + bindFields.join(g_insertFieldSpliter);
    const QString sqlRequest = QString("INSERT INTO %1 ( %2 ) VALUES ( %3 )")
            .arg(r_->getTableName(), fieldNames, fieldNamesBindings);
    query.prepare(sqlRequest);
    for(int i = 0; ; i++)
    {
        QJsonValue itemJV = reply_[i];
        if(itemJV.isUndefined()) { break; }
        for(const QString &bind : bindRefs)
        {
            query.bindValue(bind, defValues.value(bind));
        }
        for(const QString &bind : bindFields)
        {
            const QString jsonFieldName = r_->getFieldToJsonNames().value(bind.mid(1));
            QJsonValue valueJV = itemJV[jsonFieldName];
            query.bindValue(bind, !valueJV.isUndefined() && valueJV.isString() ? valueJV.toString() : QString());
        }
        query.exec();
    }
}

QString LocalDataAPINoCache::namingConversion(const QString &name_)
{
    QString res = name_;
    res.replace("-", "_");
    return res;
}
