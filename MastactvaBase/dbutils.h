#ifndef DBUTILS_H
#define DBUTILS_H


#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QSqlQuery>
#include <QJsonValue>
#include "../MastactvaBase/layout_enums.h"


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

    struct JsonSqlField
    {
        JsonSqlField(
                const QString &jsonName_,
                const QString &sqlName_,
                const layout::JsonTypesEn type_,
                bool idField_
                );

        const QString &getJsonName() const;
        const QString &getSqlName() const;
        QString getSqlType() const;
        QString getBindSqlName() const;
        QString sqlValueName() const;
        layout::JsonTypesEn getType() const;
        bool isIdField() const;
        QJsonValue jsonValue(const QVariant &val_) const;

    private:
        QString jsonName;
        QString sqlName;
        layout::JsonTypesEn type;
        bool idField = false;
    };

    void bind(const JsonSqlField &field_, QSqlQuery &query_, const QJsonValue &jv_);
    void bind(const JsonSqlField &field_, QSqlQuery &query_, const QVariant &val_);

    using JsonSqlFieldsList = QList<JsonSqlField>;

    QStringList getSqlNames(const JsonSqlFieldsList &fields_);
    QStringList getBindSqlNames(const JsonSqlFieldsList &fields_);
}

#endif // DBUTILS_H
