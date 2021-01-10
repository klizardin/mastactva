#include "localdataapinocache.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QSqlError>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"


static const char *g_dbName = "mastactva_";
static const char *g_dbNameExt = ".db3";
static const char *g_sqlText = "TEXT";
static const char *g_sqlInt = "INTEGER";
static const char *g_sqlDouble = "REAL";


QString LocalDataAPINoCache::JsonFieldInfo::getSqlType() const
{
    switch (type) {
    case layout::JsonTypesEn::jt_bool:
        return QString(g_sqlInt);
        break;
    case layout::JsonTypesEn::jt_double:
    case layout::JsonTypesEn::jt_string:
    case layout::JsonTypesEn::jt_array:
    case layout::JsonTypesEn::jt_object:
    case layout::JsonTypesEn::jt_null:
    case layout::JsonTypesEn::jt_undefined:
        return QString(g_sqlText);
        break;
    }
    return QString(g_sqlText);
}

QString LocalDataAPINoCache::JsonFieldInfo::getBindName() const
{
    return QString(":") + sqlName;
}

void LocalDataAPINoCache::JsonFieldInfo::bind(QSqlQuery &query_, const QJsonValue &jv_) const
{
    if(jv_.isBool())
    {
        bool v = jv_.toBool();
        if(layout::JsonTypesEn::jt_bool == type)
        {
            //qDebug() << getBindName() << (v?1:0);
            query_.bindValue(getBindName(), QVariant::fromValue(v?1:0));
        }
        else
        {
            //qDebug() << getBindName() << QString::number(v?1:0);
            query_.bindValue(getBindName(), QVariant::fromValue(QString::number(v?1:0)));
        }
    }
    else if(jv_.isDouble())
    {
        double v = jv_.toDouble();
        //qDebug() << getBindName() << QString::number(v);
        query_.bindValue(getBindName(), QVariant::fromValue(QString::number(v)));
    }
    else if(jv_.isString())
    {
        //qDebug() << getBindName() << jv_.toString();
        query_.bindValue(getBindName(), QVariant::fromValue(jv_.toString()));
    }
    else if(jv_.isNull())
    {
        //qDebug() << getBindName() << QString();
        query_.bindValue(getBindName(), QVariant::fromValue(QString()));
    }
    else
    {
        //qDebug() << getBindName() << QString();
        query_.bindValue(getBindName(), QVariant::fromValue(QString()));
    }
}


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

const QList<LocalDataAPINoCache::JsonFieldInfo> &LocalDataAPINoCache::SaveDBRequest::getTableFieldsInfo() const
{
    return m_tableFieldsInfo;
}

void LocalDataAPINoCache::SaveDBRequest::setTableFieldsInfo(const QList<LocalDataAPINoCache::JsonFieldInfo> &jsonFieldInfo_)
{
    m_tableFieldsInfo = jsonFieldInfo_;
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
    m_dbName = QString(g_dbName) + dateTimeToJsonString(QDateTime::currentDateTime()) + QString(g_dbNameExt);
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
    QFile::remove(m_dbName);
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
    m_database.setDatabaseName(m_dbName);
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

inline QStringList textTypes(const QStringList &names_)
{
    QStringList res;
    for(const auto &s : names_)
    {
        res.push_back(s + QString(" ") + QString(g_sqlText));
    }
    return res;
}


static const char * g_insertFieldSpliter = " , ";


void LocalDataAPINoCache::createTable(
        const QString &tableName_,
        const QList<JsonFieldInfo> &tableFieldsInfo_,
        const QStringList &refs_,
        const QString &currentRef_
        )
{
    QSqlQuery query = QSqlQuery(m_database);
    QString tableName = tableName_;
    if(!currentRef_.isEmpty()) { tableName += QString("_by_") + namingConversion(currentRef_); }
    QStringList tableFieldsNameTypePairs;
    for(const JsonFieldInfo &fi : tableFieldsInfo_)
    {
        tableFieldsNameTypePairs.push_back(fi.sqlName + QString(" ") + fi.getSqlType());
    }
    const QString fieldsRequests = (QStringList() << textTypes(refsNames(refs_)) << tableFieldsNameTypePairs).join(g_insertFieldSpliter) + QString(g_insertFieldSpliter);
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


void LocalDataAPINoCache::fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_)
{
    if(nullptr == r_ || reply_.isEmpty()) { return; }
    QSqlQuery query = QSqlQuery(m_database);
    QString tableName = r_->getTableName();
    if(!r_->getCurrentRef().isEmpty()) { tableName += QString("_by_") + namingConversion(r_->getCurrentRef()); }
    const QString fieldNames = (QStringList() << refsNames(r_->getRefs()) << getSqlNames(r_->getTableFieldsInfo())).join(g_insertFieldSpliter);
    QHash<QString, QString> defValues;
    QStringList bindRefs;
    for(const QString &ref : r_->getRefs())
    {
        const QString refBindName = QString(":") + refName(ref);
        bindRefs.push_back(refBindName);
        defValues.insert(refBindName, ref == r_->getCurrentRef() ? r_->getIdField().toString() : "");
    }
    const QString fieldNamesBindings = (QStringList() << bindRefs << getSqlBindNames(r_->getTableFieldsInfo())).join(g_insertFieldSpliter);
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
        for(const JsonFieldInfo &bindInfo : r_->getTableFieldsInfo())
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
}

QString LocalDataAPINoCache::namingConversion(const QString &name_)
{
    QString res = name_;
    res.replace("-", "_");
    return res;
}

QStringList LocalDataAPINoCache::getSqlNames(const QList<JsonFieldInfo> &tableFieldsInfo_) const
{
    QStringList res;
    for(const JsonFieldInfo &fi : tableFieldsInfo_)
    {
        res.push_back(fi.sqlName);
    }
    return res;
}

QStringList LocalDataAPINoCache::getSqlBindNames(const QList<JsonFieldInfo> &tableFieldsInfo_) const
{
    QStringList res;
    for(const JsonFieldInfo &fi : tableFieldsInfo_)
    {
        res.push_back(fi.getBindName());
    }
    return res;
}
