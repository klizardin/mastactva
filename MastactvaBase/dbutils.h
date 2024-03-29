/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DBUTILS_H
#define DBUTILS_H


#include <type_traits>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QSqlQuery>
#include <QSqlError>
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
    QString toBindName(const QString &sqlName_);
    QStringList filterNames(const QStringList &sqlNames_, const QStringList &leftNames_);
    QString jsonToSql(const QString &jsonName_);
    QStringList jsonToSql(const QStringList &jsonNames_);
    QString tableName(const JsonName &jsonLayoutName_, const JsonName &refName_);
    QString getSqlType(layout::JsonTypesEn type, bool idField);
    void checkSqlName(const QString &sqlName_);

    const QSet<QString> &sqlKeywords();

    class SqlNameOrigin : protected QString
    {
    public:
        SqlNameOrigin() = default;
        SqlNameOrigin(const QString &name_);
        const QString &sqlNameOriginToString() const;
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
                const QString &jsonName_ = QString(),
                const layout::JsonTypesEn type_ = layout::JsonTypesEn::jt_undefined,
                bool idField_ = false
                );

        // TODO: return JsonName
        QString getJsonName() const;
        // TODO: mark [[deprecated]]
        QString getSqlName() const;
        // TODO: mark [[deprecated]]
        QString getSqlType() const;
        // TODO: mark [[deprecated]]
        QString getBindSqlName() const;
        // TODO: mark [[deprecated]]
        QString sqlValueName() const;
        layout::JsonTypesEn getType() const;
        bool isIdField() const;
        QJsonValue jsonValue(const QVariant &val_) const;

    private:
        JsonName jsonName;
        SqlNameOrigin sqlNameOrigin;
        layout::JsonTypesEn type;
        bool idField = false;
    };

    struct JsonSqlFieldAndValue : public JsonSqlField
    {
        JsonSqlFieldAndValue(
                const QString &jsonName_,
                const layout::JsonTypesEn type_,
                bool idField_,
                const QVariant &value_
                );
        QJsonValue jsonValue() const;

    private:
        QVariant m_value;
    };

    class ISqlQuery
    {
    public:
        virtual ~ISqlQuery() = default;
        virtual bool prepare(const QString &request_) = 0;
        virtual bool exec(const QString &request_) = 0;
        virtual bool exec() = 0;
        virtual bool first() = 0;
        virtual bool next() = 0;
        virtual QSqlError lastError() const = 0;
        virtual void bindValue(const QString& placeholder_, const QVariant& val_) = 0;
        virtual QVariant value(const QString& name_) const = 0;
        virtual QMap<QString, QVariant> boundValues() const = 0;
    };

    void bind(const JsonSqlField &field_, ISqlQuery *query_, const QJsonValue &jv_);
    void bind(const JsonSqlField &field_, ISqlQuery *query_, const QVariant &val_);

    using JsonSqlFieldsList = QList<JsonSqlField>;
    using JsonSqlFieldAndValuesList = QList<JsonSqlFieldAndValue>;

    QStringList toStringList(const QList<QVariant> &fields_);
    QJsonObject getJsonObject(const QHash<QString, QVariant> &values_, const JsonSqlFieldsList &fields_);
    QJsonObject getJsonObject(const JsonSqlFieldsList &fields_, ISqlQuery *query_);
    JsonSqlFieldsList::const_iterator findIdField(const JsonSqlFieldsList &fields_);
    bool idFieldExist(JsonSqlFieldsList::const_iterator it_, const JsonSqlFieldsList &fields_);
    void bind(const JsonSqlFieldsList &fields_, const QJsonValue &item_, ISqlQuery *query_);
    void bind(const JsonSqlFieldsList &fields_, QHash<QString, QVariant> values_, ISqlQuery *query_);
    JsonSqlFieldsList filter(const JsonSqlFieldsList &fields_, std::function<bool(const JsonSqlField &)> func_);
    JsonSqlFieldsList filter(const JsonSqlFieldsList &fields_, const QList<QVariant> &leftFields_);
    JsonSqlFieldAndValuesList filter(const JsonSqlFieldAndValuesList &fields_, std::function<bool(const JsonSqlFieldAndValue &)> func_);
    JsonSqlFieldAndValuesList filter(const JsonSqlFieldAndValuesList &fields_, const QList<QVariant> &leftFields_);
    void bind(const JsonSqlFieldAndValuesList &fields_, ISqlQuery *query_);
    void bind(const QHash<QString, QVariant> procedureArgs_, ISqlQuery *query_);
    void setIdField(const JsonSqlFieldsList &fields_, QHash<QString, QVariant> &values_, int newIdValue_);
    JsonSqlFieldAndValuesList createRefValuesList(
            const QStringList &refs_,
            const QStringList &extraRefs_,
            const QHash<QString, QVariant> &extraFields_,
            const QString &currentRef_,
            const QVariant &idValue_
            );
    QString getCreateTableSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const JsonSqlFieldsList &fields_,
            const QStringList &refs_,
            const QStringList &extraRefs_
            );
    QString getFindSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const db::JsonSqlFieldsList &fields_,
            const QStringList &refs_,
            const QStringList &extraRefs_
            );
    QString getInsertSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const db::JsonSqlFieldsList &fields_,
            const QStringList &refs_,
            const QStringList &extraRefs_
            );
    QString getNextIdSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const db::JsonSqlFieldsList &fields_
            );
    QString getDeleteSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const db::JsonSqlFieldsList &fields_
            );
    QString getUpdateSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const db::JsonSqlFieldsList &fields_
            );
    QString getSelectSqlRequest(
            const QString &jsonLayoutName_,
            const QString &jsonRefName_,
            const db::JsonSqlFieldsList &fields_,
            const QStringList &refs_,
            const QStringList &extraFields_,
            const QHash<QString, QVariant> &procedureFields_
            );

    class SqlQueryRAII : public ISqlQuery
    {
    public:
        SqlQueryRAII(const QSqlDatabase &db_);
        ~SqlQueryRAII();

        bool prepare(const QString &request_) override;
        bool exec(const QString &request_) override;
        bool exec() override;
        bool first() override;
        bool next() override;
        QSqlError lastError() const override;
        void bindValue(const QString& placeholder_, const QVariant& val_) override;
        QVariant value(const QString& name_) const override;
        QMap<QString, QVariant> boundValues() const override;

    private:
        bool m_prepared = false;
        QSqlQuery m_query;
    };
}


class LocalDBRequest;
class DBRequestBase;


class ISqlQueryFactory
{
public:
    virtual ~ISqlQueryFactory() = default;
    virtual std::unique_ptr<db::ISqlQuery> getRequest(const DBRequestBase *r_) = 0;
    virtual std::pair<std::unique_ptr<db::ISqlQuery>,
        std::unique_ptr<db::ISqlQuery>> getRequestsPair(const DBRequestBase *r_) = 0;
};

class DBSqlQueryFactory : public ISqlQueryFactory
{
public:
    std::unique_ptr<db::ISqlQuery> getRequest(const DBRequestBase *r_) override;
    std::pair<std::unique_ptr<db::ISqlQuery>,
        std::unique_ptr<db::ISqlQuery>> getRequestsPair(const DBRequestBase *r_) override;
};

///////////////////////////////////////////////////////////////////////////////
///
/// interfaces for getting/setting data from/to storage
///

// base interface. check request structure to process
class ILocalDataAPI
{
private:
    std::shared_ptr<ISqlQueryFactory> m_queryFactory;

public:
    ILocalDataAPI();
    virtual ~ILocalDataAPI() = default;
    virtual bool canProcess(const DBRequestBase *r_) const;

    void setQueryFactory(std::shared_ptr<ISqlQueryFactory> queryFactory_);
    // helper method to get base class for the request
    std::unique_ptr<db::ISqlQuery> getRequest(const DBRequestBase *r_);
    std::pair<std::unique_ptr<db::ISqlQuery>,
        std::unique_ptr<db::ISqlQuery>> getRequestsPair(const DBRequestBase *r_);
};


/// interface to get list of data items
/// result returns to the request itself
///
/// CHECK:
///  * maybe result may delayed ?
///  -- No data can be delayed as it is a local data
///  * But we can get answer on next cycle to deal with asynch signals?
class ILocalDataGetAPI : public virtual ILocalDataAPI
{
public:
    virtual bool getListImpl(DBRequestBase *r_) = 0;
};


/// interface to add/set/delete data from the list of data at the storage
class ILocalDataUpdateAPI : public virtual ILocalDataAPI
{
public:
    virtual bool addItemImpl(const QVariant &appId_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) = 0;
    virtual bool setItemImpl(const QVariant &id_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) = 0;
    virtual bool delItemImpl(const QVariant &id_, DBRequestBase *r_) = 0;
};


/// interface for get/add/set/delete of data item of the data list
class ILocalDataGetUpdateAPI : public ILocalDataGetAPI, public ILocalDataUpdateAPI
{
};


///////////////////////////////////////////////////////////////////////////////
/// \brief The DBRequestBase class
/// structure to hold request datas
class DBRequestBase
{
public:
    DBRequestBase(const QString &apiName_);
    virtual ~DBRequestBase() = default;

    const QString &getTableName() const;
    const QString &getProcedureName() const;
    const QList<db::JsonSqlField> &getTableFieldsInfo() const;
    QStringList getRefs(bool transparent_ = false) const;           /// TODO: transparent_ unused ?
    QString getCurrentRef(bool transparent_ = false) const;         /// TODO: transparent_ unused ?
    QVariant getIdField(bool transparent_ = false) const;           /// TODO: transparent_ unused ?
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

    friend void initForTest(
            DBRequestBase &requestInfo_,
            const QString &jsonLayoutName_,
            const QString &jsonReferenceName_,
            const QList<db::JsonSqlField> &jsonFieldInfo_,
            const QStringList &refs_,
            const QStringList &extraRefKeys_
            );
};

template<typename DBRequestType_,
        typename std::enable_if<
             std::is_base_of<
                 DBRequestBase,
                 typename std::remove_cv<DBRequestType_>::type
                 >::value,
             void*>::type = nullptr
        >
class DBRequestPtr
{
private:
    using DBRequestType = typename std::remove_cv<DBRequestType_>::type;
    using DBRequestBaseType =
        typename std::conditional<
            std::is_const<DBRequestType_>::value,
            typename std::add_const<DBRequestBase>::type,
            DBRequestBase
            >::type;

public:
    template<typename DBRequestBaseType_,
            typename std::enable_if<
                 (std::is_const<DBRequestType_>::value == std::is_const<DBRequestBaseType_>::value)
                 && std::is_same<DBRequestBase,typename std::remove_cv<DBRequestBaseType_>::type>::value,
                 void*>::type = nullptr
            >
    DBRequestPtr(DBRequestBaseType_ *ptr_)
    {
        m_ptr = dynamic_cast<DBRequestType_ *>(ptr_);
    }

    ~DBRequestPtr()
    {
        post(std::is_const<DBRequestType_>());
    }

    explicit operator bool () const
    {
        return nullptr != m_ptr;
    }

    bool operator ! () const
    {
        return !operator bool();
    }

    DBRequestType_ * operator -> () const
    {
        Q_ASSERT(operator bool());
        return m_ptr;
    }

    operator DBRequestType_ * () const
    {
        Q_ASSERT(operator bool());
        return m_ptr;
    }

    operator DBRequestBaseType * () const
    {
        Q_ASSERT(operator bool());
        return static_cast<DBRequestBaseType *>(m_ptr);
    }

private:
    void post(std::false_type) const
    {
        static_cast<DBRequestBase *>(m_ptr)->setProcessed(true);
    }

    void post(std::true_type) const
    {
    }

private:
    DBRequestType_ *m_ptr = nullptr;
};


#endif // DBUTILS_H
