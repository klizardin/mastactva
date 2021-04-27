#include "dbrequestinfo.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


DBRequestInfo::DBRequestInfo(const QString &apiName_)
    :m_apiName(apiName_)
{
}

const QString &DBRequestInfo::getTableName() const
{
    return m_tableName;
}

void DBRequestInfo::setTableName(const QString &tableName_)
{
    m_tableName = tableName_;
}

const QList<db::JsonSqlField> &DBRequestInfo::getTableFieldsInfo() const
{
    return m_tableFieldsInfo;
}

void DBRequestInfo::setTableFieldsInfo(const QList<db::JsonSqlField> &jsonFieldInfo_)
{
    m_tableFieldsInfo = jsonFieldInfo_;
}

QStringList DBRequestInfo::getRefs(bool transparent_ /*= false*/) const
{
    if(transparent_ || (!transparent_ && m_procedureName.isEmpty())) { return m_refs; }
    else { return QStringList(); }
}

void DBRequestInfo::setRefs(const QStringList &refs_)
{
    m_refs = refs_;
}

QString DBRequestInfo::getCurrentRef(bool transparent_ /*= false*/) const
{
    if(transparent_ || (!transparent_ && m_procedureName.isEmpty())) { return m_currentRef; }
    else { return QString(); }
}

void DBRequestInfo::setCurrentRef(const QString &currentRef_)
{
    m_currentRef = currentRef_;
}

QVariant DBRequestInfo::getIdField(bool transparent_ /*= false*/) const
{
    if(transparent_ || (!transparent_ && m_procedureName.isEmpty())) { return m_idField; }
    else { return QVariant(); }
}

void DBRequestInfo::setIdField(const QVariant &idField_)
{
    m_idField = idField_;
}

bool DBRequestInfo::getReadonly() const
{
    return m_readonly;
}

void DBRequestInfo::setReadonly(bool readonly_)
{
    m_readonly = readonly_;
}

const QHash<QString, QVariant> &DBRequestInfo::getExtraFields() const
{
    return m_extraFields;
}

void DBRequestInfo::setExtraFields(const QHash<QString, QVariant> &extraFields_)
{
    m_extraFields = extraFields_;
}

void DBRequestInfo::insertExtraField(const QString &key_, const QVariant &value_)
{
    m_extraFields.insert(key_, value_);
}

const QString &DBRequestInfo::getProcedureName() const
{
    return m_procedureName;
}

void DBRequestInfo::setProcedureName(const QString &procedureName_)
{
    m_procedureName = procedureName_;
}

const QString &DBRequestInfo::getDBRequestName() const
{
    return m_requestName;
}

void DBRequestInfo::setDBRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

void DBRequestInfo::clearReferences()
{
    setCurrentRef(QString());
    setRefs(QStringList());
    setIdField(QVariant());
}

void DBRequestInfo::setDefaultAPI(ILocalDataAPI *defaultAPI_)
{
    m_defaultAPI = defaultAPI_;
}

ILocalDataAPI *DBRequestInfo::getDefaultAPI()
{
    return m_defaultAPI;
}

bool DBRequestInfo::isProcessed() const
{
    return m_processed;
}

void DBRequestInfo::setProcessed(bool processed_)
{
    m_processed = processed_;
}

const QString &DBRequestInfo::getAPIName() const
{
    return m_apiName;
}

QJsonObject DBRequestInfo::getJsonObjectFromValues(const QHash<QString, QVariant> &values_) const
{
    QJsonObject obj;
    for(const db::JsonSqlField &fi : qAsConst(m_tableFieldsInfo))
    {
        if(!values_.contains(fi.getJsonName())) { continue; }
        obj.insert(fi.getJsonName(), fi.jsonValue(values_.value(fi.getJsonName())));
    }
    return obj;
}

QStringList DBRequestInfo::getSqlNames(const QList<db::JsonSqlField> &tableFieldsInfo_)
{

    QStringList res;
    for(const db::JsonSqlField &fi : qAsConst(tableFieldsInfo_))
    {
        res.push_back(fi.getSqlName());
    }
    return res;
}

QStringList DBRequestInfo::getSqlBindNames(const QList<db::JsonSqlField> &tableFieldsInfo_)
{
    QStringList res;
    for(const db::JsonSqlField &fi : qAsConst(tableFieldsInfo_))
    {
        res.push_back(fi.getBindSqlName());
    }
    return res;
}

QStringList DBRequestInfo::getSetNames(const QList<db::JsonSqlField> &tableFieldsInfo_)
{
    QStringList res;
    for(const db::JsonSqlField &fi : qAsConst(tableFieldsInfo_))
    {
        if(!fi.isIdField())
        {
            res.push_back(QString("%1=%2").arg(fi.getSqlName(), fi.getBindSqlName()));
        }
    }
    return res;
}

QHash<QString, QVariant> DBRequestInfo::apiExtraFields(const QHash<QString, QVariant> &extraFields_)
{
    QHash<QString, QVariant> res;
    const QList<QString> keys = extraFields_.keys();
    for(const QString &key : qAsConst(keys))
    {
        if(QString(g_procedureExtraFieldName) == key) { continue; } // skip procedure params
        res.insert(key, extraFields_.value(key));
    }
    return res;
}

QHash<QString, QVariant> DBRequestInfo::procedureExtraFields(const QHash<QString, QVariant> &extraFields_)
{
    if(extraFields_.contains(g_procedureExtraFieldName))
    {
        return extraFields_.value(g_procedureExtraFieldName).toHash();
    }
    else
    {
        return QHash<QString, QVariant>();
    }
}


LocalDBRequest::LocalDBRequest()
    :DBRequestInfo(g_cachAPI)
{
}

void LocalDBRequest::addJsonResult(const QJsonDocument &doc_)
{
    m_doc = doc_;
}

void LocalDBRequest::addJsonResult(const QHash<QString, QVariant> &values_)
{
    QJsonArray array;

    QJsonObject obj;
    for(const db::JsonSqlField &bindInfo : qAsConst(getTableFieldsInfo()))
    {
        const QVariant val = values_.contains(bindInfo.getJsonName()) ? values_.value(bindInfo.getJsonName()) : QVariant();
        obj.insert(bindInfo.getJsonName(), bindInfo.jsonValue(val));
    }
    array.push_back(obj);
    m_doc = QJsonDocument(array);
}

const QJsonDocument &LocalDBRequest::reply() const
{
    return m_doc;
}

void LocalDBRequest::setError(bool error_)
{
    m_error = error_;
}

bool LocalDBRequest::error() const
{
    return m_error;
}
