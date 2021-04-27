#ifndef DBUTILS_H
#define DBUTILS_H


#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>

namespace db
{
    QString refName(const QString &ref_);
    QStringList refNames(const QStringList &refs_);
    QString toBindName(const QString &sqlName_);
    QStringList equalToValueConditionListFromSqlNameList(const QStringList &names_);
    QStringList filterNames(const QStringList &sqlNames_, const QList<QVariant> &leftNames_);
    QStringList applyFunction(const QStringList &sqlNames_, const QString &function_);
}

#endif // DBUTILS_H
