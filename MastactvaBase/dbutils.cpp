#include "dbutils.h"
#include <QSet>
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/timeutils.h"
#include "../MastactvaBase/format.h"


template<> inline
db::SqlNameOrigin fmt::toType(const JsonName &jsonName_, const db::SqlNameOrigin &)
{
    return db::SqlNameOrigin(db::jsonToSql(jsonName_.toString()));
}

template<> inline
db::SqlName fmt::toType(const db::JsonSqlField &field_, const db::SqlName &)
{
    return db::SqlName(fmt::toType(field_.getJsonName(),db::SqlNameOrigin{}));
}

template<> inline
db::SqlType fmt::toType(const db::JsonSqlField &field_, const db::SqlType &)
{
    return db::SqlType(field_.getType(), field_.isIdField());
}

template<> inline
db::RefSqlName fmt::toType(const db::JsonSqlField &field_, const db::RefSqlName &)
{
    return db::RefSqlName(fmt::toType(field_.getJsonName(),db::SqlNameOrigin{}));
}

template<> inline
db::BindSqlName fmt::toType(const db::JsonSqlField &field_, const db::BindSqlName &)
{
    return db::BindSqlName(fmt::toType(field_.getJsonName(), db::SqlNameOrigin{}));
}

template<> inline
db::BindRefSqlName fmt::toType(const db::JsonSqlField &field_, const db::BindRefSqlName &)
{
    return db::BindRefSqlName(fmt::toType(field_.getJsonName(),db::SqlNameOrigin{}));
}

template<typename DestType_> inline
DestType_ toType(const db::SqlNameOrigin &src_, const DestType_ &)
{
    return DestType_(src_);
}


template<> inline
QString fmt::toString(const db::SqlName &name_)
{
    const QString result = name_.quoted()
            ? db::quotName(name_.toString())
            : name_.toString()
            ;
    db::checkSqlName(result);
    return result;
}

template<> inline
QString fmt::toString(const db::BindSqlName &name_)
{
    return db::toBindName(name_.toString());
}

template<> inline
QString fmt::toString(const db::RefSqlName &name_)
{
    return db::refName(name_.toString());
}

template<> inline
QString fmt::toString(const db::BindRefSqlName &name_)
{
    return db::toBindName(db::refName(name_.toString()));
}

template<> inline
QString fmt::toString(const db::SqlType &type_)
{
    return db::getSqlType(type_.type(), type_.isIdField());
}

template<> inline
QString fmt::toString(const db::SqlTableName &name_)
{
    return db::tableName(name_.m_tableName, name_.m_refName);
}


namespace db
{

static const QChar g_quote = QChar('\"');


template<layout::JsonTypesEn jsonTypeValue_>
class LayoutJsonTypesTraits
{
public:
    static const char *sql_type_str()
    {
        return g_sqlText;
    }
};

#define LAYOUT_JSON_TYPES_TRAITS(jsonTypeValue_, sqlTypeStr_)   \
template<>                                                      \
class LayoutJsonTypesTraits<jsonTypeValue_>                     \
{                                                               \
public:                                                         \
    static const char *sql_type_str()                           \
    {                                                           \
        return sqlTypeStr_;                                     \
    }                                                           \
};                                                              \
/*end macro LAYOUT_JSON_TYPES_TRAITS*/

LAYOUT_JSON_TYPES_TRAITS(layout::JsonTypesEn::jt_bool, g_sqlInt)
LAYOUT_JSON_TYPES_TRAITS(layout::JsonTypesEn::jt_int, g_sqlInt)
LAYOUT_JSON_TYPES_TRAITS(layout::JsonTypesEn::jt_double, g_sqlDouble)


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
    const QString result = addPrefixToName(g_refPrefix, ref_);
    checkSqlName(result);
    return result;
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
    const QString result = addPrefixToName(g_bindPrefix, sqlName_);
    checkSqlName(result);
    return result;
}

QStringList equalToValueConditionListFromSqlNameList(const QStringList &names_)
{
    QStringList res;
    for(const QString &sqlName : qAsConst(names_))
    {
        const QString ref = refName(db::jsonToSql(sqlName));
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
        res.push_back(
                    name_ +
                    QString(g_spaceName) +
                    QString(
                        LayoutJsonTypesTraits<
                            layout::JsonTypesEn::jt_string
                            >::sql_type_str()
                        )
                    );
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
    return db::tableName(JsonName(jsonLayoutName_), JsonName(refName_));
}

QString tableName(const JsonName &jsonLayoutName_, const JsonName &refName_)
{
    const auto firstPart = fmt::toTypeValue(db::SqlNameOrigin{}, jsonLayoutName_);
    QString result;
    if(refName_.isEmpty())
    {
        const auto singleFirstPart = fmt::toTypeValue(db::SqlName{}, firstPart);
        result = fmt::sum(singleFirstPart);
    }
    else
    {
        const auto secondPart = fmt::toTypeValue(db::SqlNameOrigin{}, refName_);
        result = fmt::sum(firstPart, QString(g_splitTableRef), secondPart);
    }
    checkSqlName(result);
    return result;
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


SqlNameOrigin::SqlNameOrigin(const QString &name_)
{
    static_cast<QString &>(*this) = jsonToSql(name_);
}

const QString &SqlNameOrigin::toString() const
{
    return static_cast<const QString &>(*this);
}


SqlName::SqlName(const SqlNameOrigin &name_)
    : SqlNameOrigin(name_)
{
    m_quoted = sqlKeywords().contains(static_cast<const QString &>(*this).toUpper());
}

bool SqlName::quoted() const
{
    return m_quoted;
}


SqlType::SqlType(const layout::JsonTypesEn type_, bool isIdField_)
    : m_type(type_),
      m_isIdField(isIdField_)
{
}

layout::JsonTypesEn SqlType::type() const
{
    return m_type;
}

bool SqlType::isIdField() const
{
    return m_isIdField;
}


BindSqlName::BindSqlName(const SqlNameOrigin &name_)
    : SqlNameOrigin(name_)
{
}


RefSqlName::RefSqlName(const SqlNameOrigin &name_)
    : SqlNameOrigin(name_)
{
}


BindRefSqlName::BindRefSqlName(const SqlNameOrigin &name_)
    : SqlNameOrigin(name_)
{
}


JsonSqlField::JsonSqlField(
        const QString &jsonName_,
        const layout::JsonTypesEn type_,
        bool idField_
        )
    : jsonName(jsonName_),
      type(type_),
      idField(idField_)
{
    sqlNameOrigin = fmt::toTypeValue(db::SqlNameOrigin{}, jsonName);
}

QString JsonSqlField::getJsonName() const
{
    return  jsonName.toString();
}

QString JsonSqlField::getSqlName() const
{
    return fmt::toString(fmt::toTypeValue(SqlName{}, sqlNameOrigin));
}

QString getSqlType(layout::JsonTypesEn type_, bool idField_)
{
    if(idField_)
    {
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_int
            >::sql_type_str();
    }

    switch (type_)
    {
    case layout::JsonTypesEn::jt_bool:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_int
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_int:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_int
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_double:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_double
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_datetime:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_datetime
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_string:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_string
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_array:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_array
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_object:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_object
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_null:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_null
            >::sql_type_str();
        break;

    case layout::JsonTypesEn::jt_undefined:
        return LayoutJsonTypesTraits<
            layout::JsonTypesEn::jt_undefined
            >::sql_type_str();
        break;
    }

    return LayoutJsonTypesTraits<
        layout::JsonTypesEn::jt_undefined
        >::sql_type_str();
}

void checkSqlName(const QString &sqlName_)
{
    Q_ASSERT(!sqlKeywords().contains(sqlName_.toUpper()));
}

QString JsonSqlField::getSqlType() const
{
    return db::getSqlType(getType(), isIdField());
}

QString JsonSqlField::getBindSqlName() const
{
    return fmt::toString(fmt::toTypeValue(BindSqlName{}, sqlNameOrigin));
}

QString JsonSqlField::sqlValueName() const
{
    return fmt::toString(sqlNameOrigin);
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
        if(field_.isIdField() || layout::JsonTypesEn::jt_int == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << int(v);
#endif
            query_.bindValue(field_.getBindSqlName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_bool == field_.getType())
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << field_.getBindName() << v;
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
        if(field_.isIdField() || layout::JsonTypesEn::jt_int == field_.getType())
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
            const int v = int(QVariant::fromValue(jv_.toString()).toDouble());
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
        return QJsonValue(val_.toDouble() != 0 || val_.toString().toUpper() == g_trueName);
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

JsonSqlFieldAndValue::JsonSqlFieldAndValue(
        const QString &jsonName_,
        const layout::JsonTypesEn type_,
        bool idField_,
        const QVariant &value_
        )
    : JsonSqlField(jsonName_, type_, idField_)
    , m_value(value_)
{
}

QJsonValue JsonSqlFieldAndValue::jsonValue() const
{
    return JsonSqlField::jsonValue(m_value);
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

QJsonObject getJsonObject(const JsonSqlFieldsList &fields_, QSqlQuery &query_)
{
    QJsonObject jsonObj;
    for(const db::JsonSqlField &fi : qAsConst(fields_))
    {
        const QVariant val = query_.value(fi.sqlValueName());
        if(val.isValid())
        {
            jsonObj.insert(fi.getJsonName(), fi.jsonValue(val));
        }
    }
    return jsonObj;
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

JsonSqlFieldsList::const_iterator findIdField(const JsonSqlFieldsList &fields_)
{
    return std::find_if(
                std::begin(fields_),
                std::end(fields_),
                [](const JsonSqlField &field_)->bool
    {
        return field_.isIdField();
    });
}

bool idFieldExist(JsonSqlFieldsList::const_iterator it_, const JsonSqlFieldsList &fields_)
{
    return it_ != std::end(fields_);
}

void bind(const JsonSqlFieldsList &fields_, const QJsonValue &item_, QSqlQuery &query_)
{
    for(const db::JsonSqlField &fi_ : fields_)
    {
        const QJsonValue valueJV = item_[fi_.getJsonName()];
        db::bind(fi_, query_, valueJV);
    }
}

void bind(const JsonSqlFieldsList &fields_, QHash<QString, QVariant> values_, QSqlQuery &query_)
{
    for(const db::JsonSqlField &fi_ : fields_)
    {
        const QVariant val = values_.value(fi_.getJsonName());
        db::bind(fi_, query_, val);
    }
}

JsonSqlFieldsList filter(const JsonSqlFieldsList &fields_, std::function<bool(const JsonSqlField &)> func_)
{
    JsonSqlFieldsList result;
    result.reserve(fields_.size());
    for(const JsonSqlField &fi_ : fields_)
    {
        if(func_(fi_))
        {
            result.push_back(fi_);
        }
    }
    return result;
}

JsonSqlFieldsList filter(const JsonSqlFieldsList &fields_, const QList<QVariant> &leftFields_)
{
    if(leftFields_.isEmpty())
    {
        return fields_;
    }

    return db::filter(fields_, [&leftFields_](const db::JsonSqlField &fi_)->bool
    {
        const auto fit = std::find_if(
                    std::cbegin(leftFields_),
                    std::cend(leftFields_),
                    [&fi_](const QVariant &v_)->bool
        {
            return v_.isValid() && fi_.getSqlName() == v_.toString();
        });
        return std::cend(leftFields_) != fit;
    });
}

void bind(const JsonSqlFieldAndValuesList &fields_, QSqlQuery &query_)
{
    for(const db::JsonSqlFieldAndValue &fi_ : fields_)
    {
        db::bind(fi_, query_, fi_.jsonValue());
    }
}

void setIdField(const JsonSqlFieldsList &fields_, QHash<QString, QVariant> &values_, int newIdValue_)
{
    const auto fit = findIdField(fields_);
    if(!idFieldExist(fit, fields_))
    {
        return;
    }
    values_[fit->getJsonName()] = QVariant::fromValue(newIdValue_);
}

JsonSqlFieldAndValuesList createRefValuesList(
        const QStringList &refs_,
        const QStringList &extraRefs_,
        const QHash<QString, QVariant> &extraFields_,
        const QString &currentRef_,
        const QVariant &idValue_
        )
{
    JsonSqlFieldAndValuesList result;

    result.reserve(refs_.size() + extraRefs_.size());
    for(const QString &ref_ : refs_)
    {
        result.push_back(
                    JsonSqlFieldAndValue(
                        ref_,
                        layout::JsonTypesEn::jt_string, // ref type is tring
                        false,
                        ref_ == currentRef_ ? idValue_ : QVariant() // use only current ref value
                                              // others are empty strings ("")
                        )
                    );
    }
    for(const QString &extraRef_ : extraRefs_)
    {
        result.push_back(
                    JsonSqlFieldAndValue(
                        extraRef_,
                        layout::JsonTypesEn::jt_string, // ref type is tring
                        false,
                        extraFields_.contains(extraRef_) ? extraFields_.value(extraRef_) : QVariant()
                        )
                    );
    }

    return result;
}

QString getCreateTableSqlRequest(
        const QString &jsonLayoutName_,
        const QString &jsonRefName_,
        const JsonSqlFieldsList &fields_,
        const QStringList &refs_,
        const QStringList &extraRefs_
        )
{
    static const char* s_SqlNameAndTypeFmt = "%1%2%3";
    static const char* s_noSeparator = "";

    const auto mainFields = fmt::list(
        fmt::format(
            QString(s_SqlNameAndTypeFmt),
            db::SqlName{},
            fmt::constant(g_spaceName),
            db::SqlType{}
            ),
        fields_,
        s_noSeparator
        );

    const auto refFields = fmt::list(
        fmt::format(
            QString(s_SqlNameAndTypeFmt),
            db::RefSqlName{},
            fmt::constant(g_spaceName),
            fmt::constant(db::getSqlType(layout::JsonTypesEn::jt_string, false))  // ref type is tring
            ),
        fmt::toTypeList(
            db::SqlNameOrigin{},
            fmt::toTypeList(
                JsonName{},
                fmt::merge(refs_, extraRefs_)
                )
            ),
        s_noSeparator
        );

    const auto request = fmt::format(
        "CREATE TABLE IF NOT EXISTS %1 ( %2 );",
        db::SqlTableName{JsonName(jsonLayoutName_), JsonName(jsonRefName_)},
        fmt::list(
            fmt::merge(
                fmt::toTypeList(QString{}, mainFields),
                fmt::toTypeList(QString{}, refFields)
            ),
            g_insertFieldSpliter
            )
        );

    return request;
}

QString getFindSqlRequest(
        const QString &jsonLayoutName_,
        const QString &jsonRefName_,
        const db::JsonSqlFieldsList &fields_,
        const QStringList &refs_,
        const QStringList &extraRefs_
        )
{
    static const char* s_SqlNameAndTypeFmt = "%1=%2";
    static const char* s_noSeparator = "";

    const auto fit = db::findIdField(fields_);

    const auto idField = fmt::list(
        fmt::format(
            QString(s_SqlNameAndTypeFmt),
            db::SqlName{},
            db::BindSqlName{}
            ),
        db::idFieldExist(fit, fields_) ? JsonSqlFieldsList({*fit, }) : JsonSqlFieldsList{},
        s_noSeparator
        );

    const auto refFields = fmt::list(
        fmt::format(
            QString(s_SqlNameAndTypeFmt),
            db::RefSqlName{},
            db::BindRefSqlName{}
            ),
        fmt::toTypeList(
            db::SqlNameOrigin{},
            fmt::toTypeList(
                JsonName{},
                fmt::merge(refs_, extraRefs_)
                )
            ),
        s_noSeparator
        );

    const auto request = fmt::format(
        "SELECT * FROM %1 WHERE %2 LIMIT 1 ;",
        db::SqlTableName{JsonName(jsonLayoutName_), JsonName(jsonRefName_)},
        fmt::list(
            fmt::merge(
                fmt::toTypeList(QString{}, idField),
                fmt::toTypeList(QString{}, refFields)
            ),
            " AND "
            )
        );

    return request;
}

QString getInsertSqlRequest(
        const QString &jsonLayoutName_,
        const QString &jsonRefName_,
        const db::JsonSqlFieldsList &fields_,
        const QStringList &refs_,
        const QStringList &extraRefs_
        )
{
    static const char* s_noSeparator = "";

    const auto refFields = fmt::list(
        fmt::format(
            QString("%1"),
            db::RefSqlName{}
            ),
        fmt::toTypeList(
            db::SqlNameOrigin{},
            fmt::toTypeList(
                JsonName{},
                fmt::merge(refs_, extraRefs_)
                )
            ),
        s_noSeparator
        );

    const auto mainFields = fmt::list(
        fmt::format(
            QString("%1"),
            db::SqlName{}
            ),
        fields_,
        s_noSeparator
        );

    const auto bindRefFields = fmt::list(
        fmt::format(
            QString("%1"),
            db::BindRefSqlName{}
            ),
        fmt::toTypeList(
            db::SqlNameOrigin{},
            fmt::toTypeList(
                JsonName{},
                fmt::merge(refs_, extraRefs_)
                )
            ),
        s_noSeparator
        );

    const auto bindMainFields = fmt::list(
        fmt::format(
            QString("%1"),
            db::BindSqlName{}
            ),
        fields_,
        s_noSeparator
        );

    const auto request = fmt::format(
        "INSERT INTO %1 ( %2 ) VALUES ( %3 ) ;",
        db::SqlTableName{JsonName(jsonLayoutName_), JsonName(jsonRefName_)},
        fmt::list(
            fmt::merge(
                fmt::toTypeList(QString{}, refFields),
                fmt::toTypeList(QString{}, mainFields)
            ),
            g_insertFieldSpliter
            ),
        fmt::list(
            fmt::merge(
                fmt::toTypeList(QString{}, bindRefFields),
                fmt::toTypeList(QString{}, bindMainFields)
            ),
            g_insertFieldSpliter
            )
        );

    return request;
}

QString getNextIdSqlRequest(
        const QString &jsonLayoutName_,
        const QString &jsonRefName_,
        const db::JsonSqlFieldsList &fields_
        )
{
    const auto fit = db::findIdField(fields_);

    if(!db::idFieldExist(fit, fields_))
    {
        return QString{};
    }

    const auto request = fmt::format(
        "SELECT MAX(%1) FROM %2 ;",
        fit->getSqlName(),
        db::SqlTableName{JsonName(jsonLayoutName_), JsonName(jsonRefName_)}
        );

    return request;
}

QString getDeleteSqlRequest(
        const QString &jsonLayoutName_,
        const QString &jsonRefName_,
        const db::JsonSqlFieldsList &fields_
        )
{
    const auto fit = db::findIdField(fields_);

    if(!db::idFieldExist(fit, fields_))
    {
        return QString{};
    }

    const auto request = fmt::format(
        "DELETE FROM %1 WHERE %3=%4 ;",
        db::SqlTableName{JsonName(jsonLayoutName_), JsonName(jsonRefName_)},
        fit->getSqlName(),
        fit->getBindSqlName()
        );

    return request;
}

QString getUpdateSqlRequest(
        const QString &jsonLayoutName_,
        const QString &jsonRefName_,
        const db::JsonSqlFieldsList &fields_
        )
{
    const auto fit = db::findIdField(fields_);

    if(!db::idFieldExist(fit, fields_))
    {
        return QString{};
    }

    const auto fieldsWithoutId = filter(
                fields_,
                [](const JsonSqlField &fi_)->bool
    {
        return !fi_.isIdField();
    });

    const auto request = fmt::format(
        "UPDATE %1 SET %2 WHERE %3=%4 ;",
        db::SqlTableName{JsonName(jsonLayoutName_), JsonName(jsonRefName_)},
        fmt::list(
            fmt::format("%1=%2", SqlName{}, BindSqlName{}),
            fieldsWithoutId,
            g_insertFieldSpliter
        ),
        fit->getSqlName(),
        fit->getBindSqlName()
        );

    return request;
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
