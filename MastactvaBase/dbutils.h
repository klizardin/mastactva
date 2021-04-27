#ifndef DBUTILS_H
#define DBUTILS_H


#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>

namespace db
{
    bool isQuotedName(const QString &name_);
    QString quotName(const QString &name_);
    QString unquotName(const QString &name_);
    bool isRefName(const QString &name_);
    bool isBindName(const QString &name_);
    QString refName(const QString &ref_);
    QStringList refNames(const QStringList &refs_);
    QString toBindName(const QString &sqlName_);
    QStringList equalToValueConditionListFromSqlNameList(const QStringList &names_);
    QStringList filterNames(const QStringList &sqlNames_, const QList<QVariant> &leftNames_);
    QStringList applyFunction(const QStringList &sqlNames_, const QString &function_);
    QStringList textTypes(const QStringList &names_);
    QString jsonToSql(const QString &jsonName_);
    QString tableName(const QString &jsonLayoutName_, const QString &refName_);

    const QSet<QString> &sqlKeywords();
}

#endif // DBUTILS_H
