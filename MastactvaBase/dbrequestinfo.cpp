#include "dbrequestinfo.h"
#include <QSet>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


DBRequestInfo::JsonFieldInfo::JsonFieldInfo(const QString &jsonName_,const QString &sqlName_, const layout::JsonTypesEn type_,bool idField_)
    :jsonName(jsonName_), sqlName(sqlName_), type(type_), idField(idField_)
{
    if(DBRequestInfo::keywords().contains(sqlName.toUpper()))
    {
        sqlName = QString("\"%1\"").arg(sqlName);
    }
}

QString DBRequestInfo::JsonFieldInfo::getSqlType() const
{
    if(idField) { return QString(g_sqlInt); }

    switch (type) {
    case layout::JsonTypesEn::jt_bool:
        return QString(g_sqlInt);
        break;
    case layout::JsonTypesEn::jt_int:
        return QString(g_sqlInt);
        break;
    case layout::JsonTypesEn::jt_double:
        return QString(g_sqlDouble);
        break;
    case layout::JsonTypesEn::jt_datetime:
        return QString(g_sqlText);
        break;
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

QString DBRequestInfo::JsonFieldInfo::toBindName(const QString &sqlName_)
{
    if(sqlName_.at(0)==QChar('"'))
    {
        return QString(":") + sqlName_.mid(1, sqlName_.length() - 2);
    }
    else
    {
        return QString(":") + sqlName_;
    }
}

QString DBRequestInfo::JsonFieldInfo::getBindName() const
{
    return toBindName(sqlName);
}

QString DBRequestInfo::JsonFieldInfo::sqlValueName() const
{
    if(sqlName.at(0)==QChar('"'))
    {
        return sqlName.mid(1, sqlName.length() - 2);
    }
    else
    {
        return sqlName;
    }
}

void DBRequestInfo::JsonFieldInfo::bind(QSqlQuery &query_, const QJsonValue &jv_) const
{
    if(jv_.isBool())
    {
        const int v = jv_.toBool() ? 1 : 0;
        if(layout::JsonTypesEn::jt_bool == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << int(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << double(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(double(v)));
        }
        else if(layout::JsonTypesEn::jt_datetime == type)
        {
            const QString v = dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString::number(v?1:0);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString::number(v?1:0)));
        }
    }
    else if(jv_.isDouble())
    {
        const double v = jv_.toDouble();
        if(idField)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << int(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(int(v)));
        }
        else if(layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << int(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(int(v)));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_datetime == type)
        {
            const QString v = dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString::number(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString::number(v)));
        }
    }
    else if(jv_.isString())
    {
        if(idField || layout::JsonTypesEn::jt_int == type)
        {
            const int v = QVariant::fromValue(jv_.toString()).toInt();
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
            const double v = QVariant::fromValue(jv_.toString()).toDouble();
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_datetime == type)
        {
            const QString v = dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << jv_.toString();
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(jv_.toString()));
        }
    }
    else if(jv_.isNull())
    {
        if(idField || layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << (int)0;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue((int)0));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << double(0);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(double(0)));
        }
        else if(layout::JsonTypesEn::jt_datetime == type)
        {
            const QString v = dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString();
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString()));
        }
    }
    else
    {
        if(idField || layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << (int)0;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue((int)0));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << double(0);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(double(0)));
        }
        else if(layout::JsonTypesEn::jt_datetime == type)
        {
            const QString v = dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString();
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString()));
        }
    }
}

void DBRequestInfo::JsonFieldInfo::bind(QSqlQuery &query_, const QVariant &val_) const
{
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << val_;
#endif
    query_.bindValue(getBindName(), val_);
}

QJsonValue DBRequestInfo::JsonFieldInfo::jsonValue(const QVariant &val_) const
{
    switch (type) {
    case layout::JsonTypesEn::jt_bool:
        return QJsonValue(val_.toInt() != 0);
        break;
    case layout::JsonTypesEn::jt_int:
        return QJsonValue(val_.toInt());
        break;
    case layout::JsonTypesEn::jt_double:
        if(idField) { return QJsonValue(val_.toInt()); }
        else { return QJsonValue(val_.toDouble()); }
        break;
    case layout::JsonTypesEn::jt_string:
        return QJsonValue(val_.toString());
        break;
    case layout::JsonTypesEn::jt_datetime:
        return QJsonValue(dateTimeToJsonString(dateTimeFromJsonString(val_.toString())));
        break;
    case layout::JsonTypesEn::jt_array:
    case layout::JsonTypesEn::jt_object:
    case layout::JsonTypesEn::jt_null:
    case layout::JsonTypesEn::jt_undefined:
        return QJsonValue(val_.toString());
        break;
    }
    return QJsonValue(val_.toString());
}

QString DBRequestInfo::JsonFieldInfo::toString(const QJsonValue &jv_, layout::JsonTypesEn type_)
{
    if(jv_.isBool())
    {
        if(layout::JsonTypesEn::jt_datetime == type_)
        {
            return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
        }
        bool v = jv_.toBool();
        return QString::number(v?1:0);
    }
    else if(jv_.isDouble())
    {
        if(layout::JsonTypesEn::jt_datetime == type_)
        {
            return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
        }
        double v = jv_.toDouble();
        return QString::number(v);
    }
    else if(jv_.isString())
    {
        if(layout::JsonTypesEn::jt_datetime == type_)
        {
            return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
        }
        return jv_.toString();
    }
    else if(jv_.isNull())
    {
        if(layout::JsonTypesEn::jt_datetime == type_)
        {
            return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
        }
        return QString();
    }
    else
    {
        if(layout::JsonTypesEn::jt_datetime == type_)
        {
            return dateTimeToJsonString(dateTimeFromJsonString(jv_.toString()));
        }
        return QString();
    }
}

int DBRequestInfo::JsonFieldInfo::toInt(const QJsonValue &jv_, layout::JsonTypesEn type_)
{
    Q_UNUSED(type_);
    if(jv_.isBool())
    {
        bool v = jv_.toBool();
        return v?1:0;
    }
    else if(jv_.isDouble())
    {
        double v = jv_.toDouble();
        return (int)v;
    }
    else if(jv_.isString())
    {
        return QVariant::fromValue(jv_.toString()).toInt();
    }
    else if(jv_.isNull())
    {
        return 0;
    }
    else
    {
        return 0;
    }
}


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

const QList<DBRequestInfo::JsonFieldInfo> &DBRequestInfo::getTableFieldsInfo() const
{
    return m_tableFieldsInfo;
}

void DBRequestInfo::setTableFieldsInfo(const QList<DBRequestInfo::JsonFieldInfo> &jsonFieldInfo_)
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
    for(const JsonFieldInfo &fi : m_tableFieldsInfo)
    {
        if(!values_.contains(fi.jsonName)) { continue; }
        obj.insert(fi.jsonName, fi.jsonValue(values_.value(fi.jsonName)));
    }
    return obj;
}

QString DBRequestInfo::namingConversion(const QString &name_)
{
    QString res = name_;
    res.replace("-", "_");
    return res;
}

QStringList DBRequestInfo::getSqlNames(const QList<JsonFieldInfo> &tableFieldsInfo_)
{

    QStringList res;
    for(const JsonFieldInfo &fi : qAsConst(tableFieldsInfo_))
    {
        res.push_back(fi.sqlName);
    }
    return res;
}

QStringList DBRequestInfo::getSqlBindNames(const QList<JsonFieldInfo> &tableFieldsInfo_)
{
    QStringList res;
    for(const JsonFieldInfo &fi : qAsConst(tableFieldsInfo_))
    {
        res.push_back(fi.getBindName());
    }
    return res;
}

QStringList DBRequestInfo::getSetNames(const QList<JsonFieldInfo> &tableFieldsInfo_)
{
    QStringList res;
    for(const JsonFieldInfo &fi : qAsConst(tableFieldsInfo_))
    {
        if(!fi.idField)
        {
            res.push_back(QString("%1=%2").arg(fi.sqlName, fi.getBindName()));
        }
    }
    return res;
}

QHash<QString, QVariant> DBRequestInfo::apiExtraFields(const QHash<QString, QVariant> &extraFields_)
{
    QHash<QString, QVariant> res;
    const QList<QString> keys = extraFields_.keys();
    for(const QString &key : keys)
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

const QSet<QString> &DBRequestInfo::keywords()
{
    static const QSet<QString> keys = {
        "USER",
        "ABORT",
        "ACTION",
        "ADD",
        "AFTER",
        "ALL",
        "ALTER",
        "ALWAYS",
        "ANALYZE",
        "AND",
        "AS",
        "ASC",
        "ATTACH",
        "AUTOINCREMENT",
        "BEFORE",
        "BEGIN",
        "BETWEEN",
        "BY",
        "CASCADE",
        "CASE",
        "CAST",
        "CHECK",
        "COLLATE",
        "COLUMN",
        "COMMIT",
        "CONFLICT",
        "CONSTRAINT",
        "CREATE",
        "CROSS",
        "CURRENT",
        "CURRENT_DATE",
        "CURRENT_TIME",
        "CURRENT_TIMESTAMP",
        "DATABASE",
        "DEFAULT",
        "DEFERRABLE",
        "DEFERRED",
        "DELETE",
        "DESC",
        "DETACH",
        "DISTINCT",
        "DO",
        "DROP",
        "EACH",
        "ELSE",
        "END",
        "ESCAPE",
        "EXCEPT",
        "EXCLUDE",
        "EXCLUSIVE",
        "EXISTS",
        "EXPLAIN",
        "FAIL",
        "FILTER",
        "FIRST",
        "FOLLOWING",
        "FOR",
        "FOREIGN",
        "FROM",
        "FULL",
        "GENERATED",
        "GLOB",
        "GROUP",
        "GROUPS",
        "HAVING",
        "IF",
        "IGNORE",
        "IMMEDIATE",
        "IN",
        "INDEX",
        "INDEXED",
        "INITIALLY",
        "INNER",
        "INSERT",
        "INSTEAD",
        "INTERSECT",
        "INTO",
        "IS",
        "ISNULL",
        "JOIN",
        "KEY",
        "LAST",
        "LEFT",
        "LIKE",
        "LIMIT",
        "MATCH",
        "NATURAL",
        "NO",
        "NOT",
        "NOTHING",
        "NOTNULL",
        "NULL",
        "NULLS",
        "OF",
        "OFFSET",
        "ON",
        "OR",
        "ORDER",
        "OTHERS",
        "OUTER",
        "OVER",
        "PARTITION",
        "PLAN",
        "PRAGMA",
        "PRECEDING",
        "PRIMARY",
        "QUERY",
        "RAISE",
        "RANGE",
        "RECURSIVE",
        "REFERENCES",
        "REGEXP",
        "REINDEX",
        "RELEASE",
        "RENAME",
        "REPLACE",
        "RESTRICT",
        "RIGHT",
        "ROLLBACK",
        "ROW",
        "ROWS",
        "SAVEPOINT",
        "SELECT",
        "SET",
        "TABLE",
        "TEMP",
        "TEMPORARY",
        "THEN",
        "TIES",
        "TO",
        "TRANSACTION",
        "TRIGGER",
        "UNBOUNDED",
        "UNION",
        "UNIQUE",
        "UPDATE",
        "USING",
        "VACUUM",
        "VALUES",
        "VIEW",
        "VIRTUAL",
        "WHEN",
        "WHERE",
        "WINDOW",
        "WITH",
        "WITHOUT",
    };
    return keys;
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
    for(const DBRequestInfo::JsonFieldInfo &bindInfo : qAsConst(getTableFieldsInfo()))
    {
        const QVariant val = values_.contains(bindInfo.jsonName) ? values_.value(bindInfo.jsonName) : QVariant();
        obj.insert(bindInfo.jsonName, bindInfo.jsonValue(val));
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
