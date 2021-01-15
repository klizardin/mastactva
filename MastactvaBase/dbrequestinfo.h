#ifndef DBREQUESTINFO_H
#define DBREQUESTINFO_H


#include <QString>
#include <QtSql>
#include <QVector>
#include <QList>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/qmlobjects.h"
//#include "../MastactvaBase/netapi.h"


static const char *g_procedureExtraFieldName = "";
static const char *g_procedureConditionName = "WHERE";
static const char *g_procedureLimitName = "LIMIT";
static const char *g_procedureOrderByName = "ORDER BY";
static const char *g_procedureArguments = "ARGS";
static const char *g_procedureDefaultAPI = "DEFAULTAPI";
static const char *g_procedureArgFunctionName = "ARGFUNCTION";
static const char *g_procedureSelectFunctionName = "SELECTFUNCTION";
static const char *g_procedureFilterNamesName = "FILTERNAMES";


class LocalDBRequest;
class DBRequestInfo;


class ILocalDataAPI
{
public:
    virtual ~ILocalDataAPI() = default;
    virtual bool canProcess(const DBRequestInfo *r_) const = 0;
    virtual bool getListImpl(DBRequestInfo *r_) = 0;
    virtual bool addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_) = 0;
    virtual bool setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_) = 0;
    virtual bool delItemImpl(const QVariant &id_, DBRequestInfo *r_) = 0;
};


class DBRequestInfo
{
public:
    struct JsonFieldInfo
    {
        QString jsonName;
        QString sqlName;
        layout::JsonTypesEn type;
        bool idField = false;

        QString getSqlType() const;
        static QString toBindName(const QString &sqlName_);
        QString getBindName() const;
        void bind(QSqlQuery &query_, const QJsonValue &jv_) const;
        void bind(QSqlQuery &query_, const QVariant &val_) const;
        QJsonValue jsonValue(const QVariant &val_) const;
        static QString toString(const QJsonValue &jv_, layout::JsonTypesEn type_);
        static int toInt(const QJsonValue &jv_, layout::JsonTypesEn type_);
    };

public:
    DBRequestInfo() = default;

    template<typename DataType_>
    void init(
            const QString &requestName_,
            const QString &layoutName_,
            bool readonly_,
            const QHash<QString, QVariant> &extraFields_
            )
    {
        init<DataType_>(requestName_,
                        layoutName_,
                        QString(),
                        QStringList(),
                        QString(),
                        QString(),
                        QString(),
                        QVariant(),
                        QVariant(),
                        readonly_,
                        extraFields_
                        );
    }

    template<typename DataType_>
    void init(
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
            )
    {
        QString tableName;
        QList<JsonFieldInfo> tableFieldsInfo;
        QVariant idField;


        const QString jsonLayoutName = layoutName_;
        tableName = namingConversion(jsonLayoutName);
        const QString idFieldName = getDataLayout<DataType_>().getIdFieldJsonName();
        QList<QPair<QString, layout::JsonTypesEn>> fieldsInfo;
        getDataLayout<DataType_>().getJsonFieldsInfo(fieldsInfo);
        tableFieldsInfo.clear();
        for(const auto &jsonFieldName: qAsConst(fieldsInfo))
        {
            const QString sqlFieldName = namingConversion(jsonFieldName.first);
            tableFieldsInfo.push_back({jsonFieldName.first, sqlFieldName, jsonFieldName.second, idFieldName == jsonFieldName.first});
        }

        IListModel *parentModelPtr = QMLObjectsBase::getInstance().getListModel(parentModel_);
        if(nullptr != parentModelPtr || (!refAppId_.isValid() && refValue_.isValid()))
        {
            idField =
                refValue_.isValid()
                    ? refValue_
                    : parentModelJsonFieldName_.isEmpty()
                      ? parentModelPtr->getIdFieldValueForAppId(refAppId_)
                      : parentModelPtr->getFieldValueForAppId(refAppId_, parentModelJsonFieldName_)
                ;
        }

        setTableName(tableName);
        setTableFieldsInfo(tableFieldsInfo);
        setRefs(refs_);
        setCurrentRef(currentRef_);
        setIdField(idField);
        setReadonly(readonly_);
        setExtraFields(extraFields_);
        setProcedureName(procedureName_);
        setDBRequestName(requestName_);
    }

    const QString &getTableName() const;
    const QString &getProcedureName() const;
    const QList<JsonFieldInfo> &getTableFieldsInfo() const;
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

    QJsonObject getJsonObjectFromValues(const QHash<QString, QVariant> &values_) const;

    static QString namingConversion(const QString &name_);
    static QStringList getSqlNames(const QList<JsonFieldInfo> &tableFieldsInfo_);
    static QStringList getSqlBindNames(const QList<JsonFieldInfo> &tableFieldsInfo_);
    static QStringList getSetNames(const QList<JsonFieldInfo> &tableFieldsInfo_);
    static QHash<QString, QVariant> apiExtraFields(const QHash<QString, QVariant> &extraFields_);
    static QHash<QString, QVariant> procedureExtraFields(const QHash<QString, QVariant> &extraFields_);

protected:
    void setTableName(const QString &tableName_);
    void setTableFieldsInfo(const QList<JsonFieldInfo> &jsonFieldInfo_);
    void setRefs(const QStringList &refs_);
    void setCurrentRef(const QString &currentRef_);
    void setIdField(const QVariant &idField_);
    void setReadonly(bool readonly_);
    void setExtraFields(const QHash<QString, QVariant> &extraFields_);
    void setProcedureName(const QString &procedureName_);
    void setDBRequestName(const QString &requestName_);

private:
    QString m_requestName;
    QString m_tableName;
    QString m_procedureName;
    QList<JsonFieldInfo> m_tableFieldsInfo;
    QStringList m_refs;
    QString m_currentRef;
    QVariant m_idField;
    QHash<QString, QVariant> m_extraFields;
    bool m_readonly = true;
    ILocalDataAPI *m_defaultAPI = nullptr;
    bool m_processed = false;
};


class LocalDBRequest :
        public DBRequestInfo,
        public RequestData
{
public:
    LocalDBRequest() = default;
    void addJsonResult(const QJsonDocument &doc_);
    void addJsonResult(const QHash<QString, QVariant> &values_);
    const QJsonDocument &reply() const;
    void setError(bool error_);
    bool error() const;

private:
    QJsonDocument m_doc;
    bool m_error = false;
};


#endif // DBREQUESTINFO_H
