#ifndef DBUTILS_H
#define DBUTILS_H


#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QSqlQuery>
#include <QJsonValue>
#include <QJsonObject>
#include "../MastactvaBase/layout_enums.h"
#include "../MastactvaBase/layout_type.h"
#include "../MastactvaBase/format_type.h"


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
    QStringList jsonToSql(const QStringList &jsonNames_);
    QString tableName(const QString &jsonLayoutName_, const QString &refName_);
    QString getSqlType(layout::JsonTypesEn type, bool idField);

    const QSet<QString> &sqlKeywords();

    class SqlNameOrigin : protected QString
    {
    public:
        SqlNameOrigin() = default;
        SqlNameOrigin(const QString &name_);
        const QString &toString() const;
    };

    class SqlTableName
    {
    public:
        JsonName m_tableName;
        JsonName m_refName;
    };

    class SqlName : public SqlNameOrigin
    {
    public:
        SqlName() = default;
        SqlName(const SqlNameOrigin &name_);
        bool quoted() const;

    private:
        bool m_quoted;
    };

    class SqlType
    {
    public:
        SqlType() = default;
        SqlType(const layout::JsonTypesEn type_, bool isIdField_);
        layout::JsonTypesEn type() const;
        bool isIdField() const;

    private:
        layout::JsonTypesEn m_type = layout::JsonTypesEn::jt_undefined;
        bool m_isIdField = false;
    };

    class BindSqlName : public SqlNameOrigin
    {
    public:
        BindSqlName() = default;
        BindSqlName(const SqlNameOrigin &name_);
    };

    class RefSqlName : public SqlNameOrigin
    {
    public:
        RefSqlName() = default;
        RefSqlName(const SqlNameOrigin &name_);
    };

    class BindRefSqlName : public SqlNameOrigin
    {
    public:
        BindRefSqlName() = default;
        BindRefSqlName(const SqlNameOrigin &name_);
    };

    struct JsonSqlField
    {
        JsonSqlField(
                const QString &jsonName_,
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
    QStringList getSqlNameEqualBindSqlNameList(const JsonSqlFieldsList &fields_);
    QJsonObject getJsonObject(const QHash<QString, QVariant> &values_, const JsonSqlFieldsList &fields_);
    QStringList getSqlNameAndTypeList(const JsonSqlFieldsList &fields_);
    QString getCreateTableSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const JsonSqlFieldsList &fields_,
            const QStringList &refs_,
            const QStringList &extraRefs_
            );
}


class LocalDBRequest;
class DBRequestBase;


class ILocalDataAPI
{
public:
    virtual ~ILocalDataAPI() = default;
    virtual bool canProcess(const DBRequestBase *r_) const = 0;
    virtual bool getListImpl(DBRequestBase *r_) = 0;
    virtual bool addItemImpl(const QVariant &appId_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) = 0;
    virtual bool setItemImpl(const QVariant &id_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) = 0;
    virtual bool delItemImpl(const QVariant &id_, DBRequestBase *r_) = 0;
};


class DBRequestBase
{
public:
    DBRequestBase(const QString &apiName_);

    const QString &getTableName() const;
    const QString &getProcedureName() const;
    const QList<db::JsonSqlField> &getTableFieldsInfo() const;
    QStringList getRefs(bool transparent_ = false) const;
    QString getCurrentRef(bool transparent_ = false) const;
    QVariant getIdField(bool transparent_ = false) const;
    bool getReadonly() const;
    const QHash<QString, QVariant> &getExtraFields() const;
    void insertExtraField(const QString &key_, const QVariant &value_);
    const QString &getDBRequestName() const;
    void clearReferences();
    void setDefaultAPI(ILocalDataAPI *defaultAPI_);
    ILocalDataAPI *getDefaultAPI();
    bool isProcessed() const;
    void setProcessed(bool processed_);
    const QString &getAPIName() const;

    static QHash<QString, QVariant> apiExtraFields(const QHash<QString, QVariant> &extraFields_);
    static QHash<QString, QVariant> procedureExtraFields(const QHash<QString, QVariant> &extraFields_);

private:
    void setTableName(const QString &tableName_);
    void setTableFieldsInfo(const QList<db::JsonSqlField> &jsonFieldInfo_);
    void setRefs(const QStringList &refs_);
    void setCurrentRef(const QString &currentRef_);
    void setIdField(const QVariant &idField_);
    void setReadonly(bool readonly_);
    void setExtraFields(const QHash<QString, QVariant> &extraFields_);
    void setProcedureName(const QString &procedureName_);
    void setDBRequestName(const QString &requestName_);

private:
    QString m_apiName;
    QString m_requestName;
    QString m_tableName;
    QString m_procedureName;
    QList<db::JsonSqlField> m_tableFieldsInfo;
    QStringList m_refs;
    QString m_currentRef;
    QVariant m_idField;
    QHash<QString, QVariant> m_extraFields;
    bool m_readonly = true;
    ILocalDataAPI *m_defaultAPI = nullptr;
    bool m_processed = false;


    template<typename DataType_>
    friend void init(
            DBRequestBase &requestInfo_,
            const QString &requestName_,
            const QString &layoutName_,
            const QString &procedureName_,
            const QStringList &refs_,
            const QString &currentRef_,
            const QString &parentModel_,
            const QString &parentModelJsonFieldName_,
            const QVariant &refAppId_,
            const QVariant &refValue_,
            bool readonly_,
            const QHash<QString, QVariant> &extraFields_
            );
};


#endif // DBUTILS_H
