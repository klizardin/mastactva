#include "localdataapinocache.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QSqlError>
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
    //QString savePath = QDir(m_savePath).filePath(g_dbName);
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
    //QString savePath = QDir(m_savePath).filePath(g_dbName);
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(g_dbName);
    m_database.open();
}

bool LocalDataAPINoCache::isSaveToDBMode() const
{
    return !m_savePath.isEmpty();
}


static const char * g_refPrefix = "ref_";


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


static const char * g_createFieldSpliter = " TEXT , ";


void LocalDataAPINoCache::createTable(
        const QString &tableName_,
        const QStringList &tableFieldNames_,
        const QStringList &refs_,
        const QString &currentRef_
        )
{
    QSqlQuery query = QSqlQuery(m_database);
    QString tableName = tableName_;
    if(!currentRef_.isEmpty()) { tableName += QString("_by_") + namingConversion(currentRef_); }
    const QString fieldsRequests = (QStringList() << refsNames(refs_) << tableFieldNames_).join(g_createFieldSpliter) + QString(g_createFieldSpliter);
    const QString sqlRequest = QString("CREATE TABLE IF NOT EXISTS %1 ( %2 )")
            .arg(tableName, fieldsRequests.mid(0, fieldsRequests.length() - 2))
            ;
    //qDebug() << sqlRequest;
    if(!query.exec(sqlRequest))
    {
        const QSqlError err = query.lastError();
        qDebug() << err.driverText();
        qDebug() << err.databaseText();
        qDebug() << err.nativeErrorCode();
        qDebug() << err.text();
    }
}


static const char * g_insertFieldSpliter = " , ";


void LocalDataAPINoCache::fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_)
{
    if(nullptr == r_ || reply_.isEmpty()) { return; }
    QSqlQuery query = QSqlQuery(m_database);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString("_by_") + namingConversion(r_->getCurrentRef()); }
    const QString fieldNames = (QStringList() << refsNames(r_->getRefs()) << r_->getTableFieldNames()).join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : r_->getRefs())
    {
        const QString refBindName = QString(":") + refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : "");
    }
    QStringList bindFields;
    for(const QString &fieldName : r_->getTableFieldNames())
    {
        bindFields.push_back(QString(":") + fieldName);
    }
    const QString fieldNamesBindings = (QStringList() << bindRefs << bindFields).join(g_insertFieldSpliter);
    const QString sqlRequest = QString("INSERT INTO %1 ( %2 ) VALUES ( %3 )")
            .arg(tableName, fieldNames, fieldNamesBindings);
    //qDebug() << sqlRequest;
    query.prepare(sqlRequest);
    for(int i = 0; ; i++)
    {
        QJsonValue itemJV = reply_[i];
        if(itemJV.isUndefined()) { break; }
        for(const QString &bind : bindRefs)
        {
            const QString v = defValues.value(bind);
            query.bindValue(bind, v);
            //qDebug() << bind << v;
        }
        for(const QString &bind : bindFields)
        {
            const QString jsonFieldName = r_->getFieldToJsonNames().value(bind.mid(1));
            const QJsonValue valueJV = itemJV[jsonFieldName];
            QString v;
            if(!valueJV.isUndefined())
            {
                switch(valueJV.type())
                {
                case QJsonValue::Null:
                    v.clear();
                    break;
                case QJsonValue::Bool:
                    v = QString::number(valueJV.toBool() ? 1 : 0);
                    break;
                case QJsonValue::Double:
                    v = QVariant::fromValue(valueJV.toDouble()).toString();
                    break;
                case QJsonValue::String:
                    v = valueJV.toString();
                    break;
                case QJsonValue::Array:
                case QJsonValue::Object:
                    Q_ASSERT(false);
                    break;
                }
            }
            query.bindValue(bind, v);
            //qDebug() << bind << v;
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
}

QString LocalDataAPINoCache::namingConversion(const QString &name_)
{
    QString res = name_;
    res.replace("-", "_");
    return res;
}
