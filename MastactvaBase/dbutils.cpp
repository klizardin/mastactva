#include "dbutils.h"
#include <QSet>
#include "../MastactvaBase/names.h"


namespace db
{

static const QChar g_quote = QChar('\"');
static const char *g_space = " ";


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
        res.push_back(name_ + QString(g_space) + QString(g_sqlText));
    }
    return res;
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

}
