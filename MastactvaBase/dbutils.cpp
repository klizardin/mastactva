#include "dbutils.h"
#include <QSet>
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/timeutils.h"


namespace db
{

static const QChar g_quote = QChar('\"');


bool isQuotedName(const QString &name_)
{
    const int findIndex1 = name_.indexOf(g_quote);
    if(findIndex1 < 0)
    {
        return false;
    }

    const int findIndex2 = name_.indexOf(g_quote, findIndex1 + 1);
    Q_ASSERT(findIndex1 == 0 && findIndex2 == name_.length() - 1);
    return true;
}

QString quotName(const QString &name_)
{
    if(isQuotedName(name_)
            || isRefName(name_)
            || isBindName(name_))
    {
        return name_;
    }
    else
    {
        return QString(g_quote) + name_ + QString(g_quote);
    }
}

QString unquotName(const QString &name_)
{
    if(isQuotedName(name_))
    {
        // remove first and last symbols
        return name_.mid(1, name_.length() - 2);
    }
    else
    {
        return name_;
    }
}

bool isRefName(const QString &name_)
{
    return name_.indexOf(QString(g_refPrefix)) == 0;
}

bool isBindName(const QString &name_)
{
    return name_.indexOf(QString(g_bindPrefix)) == 0;
}

static inline QString addPrefixToName(const QString &prefix_, const QString &name_)
{
    if(isQuotedName(name_))
    {
        return prefix_ + unquotName(name_);
    }
    else
    {
        return prefix_ + name_;
    }
}

QString refName(const QString &ref_)
{
    return addPrefixToName(g_refPrefix, ref_);
}

QStringList refNames(const QStringList &refs_)
{
    QStringList res;
    for(const auto &s : qAsConst(refs_))
    {
        res.push_back(refName(s));
    }
    return res;
}

QString toBindName(const QString &sqlName_)
{
    return addPrefixToName(g_bindPrefix, sqlName_);
}

QStringList equalToValueConditionListFromSqlNameList(const QStringList &names_)
{
    QStringList res;
    for(const QString &sqlName : qAsConst(names_))
    {
        const QString ref = refName(sqlName);
        res.push_back(QString("%1=%2").arg(ref, toBindName(ref)));
    }
    return res;
}

QStringList filterNames(const QStringList &sqlNames_, const QList<QVariant> &leftNames_)
{
    if(leftNames_.isEmpty())
    {
        return sqlNames_;
    }

    QStringList res;
    for(const QString &name: qAsConst(sqlNames_))
    {
        const auto fit = std::find_if(std::cbegin(leftNames_), std::cend(leftNames_),
                                      [&name](const QVariant &val)->bool
        {
           return val.isValid() && val.toString() == name;
        });
        if(std::cend(leftNames_) != fit)
        {
            res.push_back(name);
        }
    }
    return res;
}

QStringList applyFunction(const QStringList &sqlNames_, const QString &function_)
{
    if(function_.isEmpty())
    {
        return sqlNames_;
    }

    QStringList res;
    for(const QString &name: qAsConst(sqlNames_))
    {
        res.push_back(QString("%1(%2)").arg(function_, name));
    }
    return res;
}

QStringList textTypes(const QStringList &names_)
{
    QStringList res;
    for(const auto &name_ : qAsConst(names_))
    {
        res.push_back(name_ + QString(g_spaceName) + QString(g_sqlText));
    }
    return res;
}

QString jsonToSql(const QString &jsonName_)
{
    QString res = jsonName_;
    res.replace("-", "_");
    return res;
}

QStringList jsonToSql(const QStringList &jsonNames_)
{
    QStringList res;
    res.reserve(jsonNames_.size());
    for(const auto &name_ : jsonNames_)
    {
        res.push_back(db::jsonToSql(name_));
    }
    return res;
}

QString tableName(const QString &jsonLayoutName_, const QString &refName_)
{
    if(refName_.trimmed().isEmpty())
    {
        return jsonToSql(jsonLayoutName_);
    }
    else
    {
        return jsonToSql(jsonLayoutName_) + QString(g_splitTableRef) + jsonToSql(refName_);
    }
}


const QSet<QString> &sqlKeywords()
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


JsonSqlField::JsonSqlField(
        const QString &jsonName_,
        const QString &sqlName_,
        const layout::JsonTypesEn type_,
        bool idField_
        )
    : jsonName(jsonName_),
      sqlName(sqlName_),
      type(type_),
      idField(idField_)
{
    if(db::sqlKeywords().contains(sqlName.toUpper()))
    {
        sqlName = quotName(sqlName);
    }
}

const QString &JsonSqlField::getJsonName() const
{
    return jsonName;
}

const QString &JsonSqlField::getSqlName() const
{
    return sqlName;
}

QString JsonSqlField::getSqlType() const
{
    if(idField)
    {
        return QString(g_sqlInt);
    }

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

QString JsonSqlField::getBindSqlName() const
{
    return db::toBindName(sqlName);
}

QString JsonSqlField::sqlValueName() const
{
    return unquotName(sqlName);
}

layout::JsonTypesEn JsonSqlField::getType() const
{
    return type;
}

bool JsonSqlField::isIdField() const
{
    return idField;
}

void bind(const JsonSqlField &field_, QSqlQuery &query_, const QJsonValue &jv_)
{
    if(jv_.isBool())
    {
        const int v = jv_.toBool() ? 1 : 0;
        if(layout::JsonTypesEn::jt_bool == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_int == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << int(v);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_double == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << double(v);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(double(v)));
        }
        else if(layout::JsonTypesEn::jt_datetime == field_.getType())
        {
            const QString v = date_time::dateTimeToJsonString(date_time::dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << QString::number(v?1:0);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(QString::number(v?1:0)));
        }
    }
    else if(jv_.isDouble())
    {
        const double v = jv_.toDouble();
        if(field_.isIdField())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << int(v);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(int(v)));
        }
        else if(layout::JsonTypesEn::jt_int == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << int(v);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(int(v)));
        }
        else if(layout::JsonTypesEn::jt_double == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_datetime == field_.getType())
        {
            const QString v = date_time::dateTimeToJsonString(date_time::dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << QString::number(v);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(QString::number(v)));
        }
    }
    else if(jv_.isString())
    {
        if(field_.isIdField() || layout::JsonTypesEn::jt_int == field_.getType())
        {
            const int v = QVariant::fromValue(jv_.toString()).toInt();
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_double == field_.getType())
        {
            const double v = QVariant::fromValue(jv_.toString()).toDouble();
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_datetime == field_.getType())
        {
            const QString v = date_time::dateTimeToJsonString(date_time::dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << jv_.toString();
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(jv_.toString()));
        }
    }
    else if(jv_.isNull())
    {
        if(field_.isIdField() || layout::JsonTypesEn::jt_int == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << (int)0;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue((int)0));
        }
        else if(layout::JsonTypesEn::jt_double == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << double(0);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(double(0)));
        }
        else if(layout::JsonTypesEn::jt_datetime == field_.getType())
        {
            const QString v = date_time::dateTimeToJsonString(date_time::dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << QString();
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(QString()));
        }
    }
    else
    {
        if(field_.isIdField() || layout::JsonTypesEn::jt_int == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << (int)0;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue((int)0));
        }
        else if(layout::JsonTypesEn::jt_double == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << double(0);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(double(0)));
        }
        else if(layout::JsonTypesEn::jt_datetime == field_.getType())
        {
            const QString v = date_time::dateTimeToJsonString(date_time::dateTimeFromJsonString(jv_.toString()));
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << QString();
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(QString()));
        }
    }
}

void bind(const JsonSqlField &field_, QSqlQuery &query_, const QVariant &val_)
{
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << val_;
#endif
    query_.bindValue(field_.getBindSqlName(), val_);
}

QJsonValue JsonSqlField::jsonValue(const QVariant &val_) const
{
    switch (type)
    {
    case layout::JsonTypesEn::jt_bool:
        return QJsonValue(val_.toDouble() != 0 || val_.toString().toUpper() == "TRUE");
        break;

    case layout::JsonTypesEn::jt_int:
        return QJsonValue(int(val_.toDouble()));
        break;

    case layout::JsonTypesEn::jt_double:
        if(idField)
        {
            return QJsonValue(int(val_.toDouble()));
        }
        else
        {
            return QJsonValue(val_.toDouble());
        }
        break;

    case layout::JsonTypesEn::jt_string:
        return QJsonValue(val_.toString());
        break;

    case layout::JsonTypesEn::jt_datetime:
        return QJsonValue(date_time::dateTimeToJsonString(date_time::dateTimeFromJsonString(val_.toString())));
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

QStringList getFieldListValues(
        const JsonSqlFieldsList &fields_,
        std::function<QString(const db::JsonSqlField &)> getter_)
{
    QStringList res;
    for(const db::JsonSqlField &fi : qAsConst(fields_))
    {
        res.push_back(getter_(fi));
    }
    return res;
}


QStringList getSqlNames(const JsonSqlFieldsList &fields_)
{
    return getFieldListValues(
                fields_,
                [](const db::JsonSqlField &fi_)->QString
    {
        return fi_.getSqlName();
    });
}

QStringList getBindSqlNames(const JsonSqlFieldsList &fields_)
{
    return getFieldListValues(
                fields_,
                [](const db::JsonSqlField &fi_)->QString
    {
        return fi_.getBindSqlName();
    });
}

QStringList getSqlNameEqualBindSqlNameList(const JsonSqlFieldsList &fields_)
{
    QStringList res;
    for(const db::JsonSqlField &fi : fields_)
    {
        if(fi.isIdField())
        {
            continue;
        }

        res.push_back(QString("%1=%2").arg(fi.getSqlName(), fi.getBindSqlName()));
    }
    return res;
}

QJsonObject getJsonObject(const QHash<QString, QVariant> &values_, const JsonSqlFieldsList &fields_)
{
    QJsonObject obj;
    for(const db::JsonSqlField &fi : fields_)
    {
        if(!values_.contains(fi.getJsonName()))
        {
            continue;
        }

        const QVariant val = values_.value(fi.getJsonName());
        const QJsonValue jsonVal = fi.jsonValue(val);
        obj.insert(fi.getJsonName(), jsonVal);
    }
    return obj;
}

QStringList getSqlNameAndTypeList(const JsonSqlFieldsList &fields_)
{
    return getFieldListValues(
                fields_,
                [](const db::JsonSqlField &fi_)->QString
    {
        return fi_.getSqlName() + QString(g_spaceName) + fi_.getSqlType();
    });
}

QString getCreateTableSqlRequest(
        const QString &jsonLayoutName_,
        const QString &jsonRefName_,
        const JsonSqlFieldsList &fields_,
        const QStringList &refs_,
        const QStringList &extraRefs_
        )
{
    const QString tableName = db::tableName(jsonLayoutName_, jsonRefName_);
    QStringList nameAndTypeList = getSqlNameAndTypeList(fields_);
    nameAndTypeList << db::textTypes(
                           db::refNames(db::jsonToSql(refs_))
                           );
    nameAndTypeList << db::textTypes(
                           db::refNames(db::jsonToSql(extraRefs_))
                           );
    const QString fieldsRequests = nameAndTypeList.join(g_insertFieldSpliter);
    const QString sqlRequest = QString("CREATE TABLE IF NOT EXISTS %1 ( %2 ) ;")
            .arg(tableName, fieldsRequests)
            ;
    return sqlRequest;
}

} // namespace db


DBRequestBase::DBRequestBase(const QString &apiName_)
    :m_apiName(apiName_)
{
}

const QString &DBRequestBase::getTableName() const
{
    return m_tableName;
}

void DBRequestBase::setTableName(const QString &tableName_)
{
    m_tableName = tableName_;
}

const QList<db::JsonSqlField> &DBRequestBase::getTableFieldsInfo() const
{
    return m_tableFieldsInfo;
}

void DBRequestBase::setTableFieldsInfo(const QList<db::JsonSqlField> &jsonFieldInfo_)
{
    m_tableFieldsInfo = jsonFieldInfo_;
}

QStringList DBRequestBase::getRefs(bool transparent_ /*= false*/) const
{
    if(transparent_ || (!transparent_ && m_procedureName.isEmpty()))
    {
        return m_refs;
    }
    else
    {
        return QStringList();
    }
}

void DBRequestBase::setRefs(const QStringList &refs_)
{
    m_refs = refs_;
}

QString DBRequestBase::getCurrentRef(bool transparent_ /*= false*/) const
{
    if(transparent_ || (!transparent_ && m_procedureName.isEmpty()))
    {
        return m_currentRef;
    }
    else
    {
        return QString();
    }
}

void DBRequestBase::setCurrentRef(const QString &currentRef_)
{
    m_currentRef = currentRef_;
}

QVariant DBRequestBase::getIdField(bool transparent_ /*= false*/) const
{
    if(transparent_ || (!transparent_ && m_procedureName.isEmpty()))
    {
        return m_idField;
    }
    else
    {
        return QVariant();
    }
}

void DBRequestBase::setIdField(const QVariant &idField_)
{
    m_idField = idField_;
}

bool DBRequestBase::getReadonly() const
{
    return m_readonly;
}

void DBRequestBase::setReadonly(bool readonly_)
{
    m_readonly = readonly_;
}

const QHash<QString, QVariant> &DBRequestBase::getExtraFields() const
{
    return m_extraFields;
}

void DBRequestBase::setExtraFields(const QHash<QString, QVariant> &extraFields_)
{
    m_extraFields = extraFields_;
}

void DBRequestBase::insertExtraField(const QString &key_, const QVariant &value_)
{
    m_extraFields.insert(key_, value_);
}

const QString &DBRequestBase::getProcedureName() const
{
    return m_procedureName;
}

void DBRequestBase::setProcedureName(const QString &procedureName_)
{
    m_procedureName = procedureName_;
}

const QString &DBRequestBase::getDBRequestName() const
{
    return m_requestName;
}

void DBRequestBase::setDBRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

void DBRequestBase::clearReferences()
{
    setCurrentRef(QString());
    setRefs(QStringList());
    setIdField(QVariant());
}

void DBRequestBase::setDefaultAPI(ILocalDataAPI *defaultAPI_)
{
    m_defaultAPI = defaultAPI_;
}

ILocalDataAPI *DBRequestBase::getDefaultAPI()
{
    return m_defaultAPI;
}

bool DBRequestBase::isProcessed() const
{
    return m_processed;
}

void DBRequestBase::setProcessed(bool processed_)
{
    m_processed = processed_;
}

const QString &DBRequestBase::getAPIName() const
{
    return m_apiName;
}

QHash<QString, QVariant> DBRequestBase::apiExtraFields(const QHash<QString, QVariant> &extraFields_)
{
    QHash<QString, QVariant> res;
    const QList<QString> keys = extraFields_.keys();
    for(const QString &key : qAsConst(keys))
    {
        if(QString(g_procedureExtraFieldName) == key)
        {
            continue;
        } // skip procedure params

        res.insert(key, extraFields_.value(key));
    }
    return res;
}

QHash<QString, QVariant> DBRequestBase::procedureExtraFields(const QHash<QString, QVariant> &extraFields_)
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


