#ifndef DBREQUESTINFO_H
#define DBREQUESTINFO_H


#include <QString>
#include <QtSql>
#include <QVector>
#include <QList>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/netapi.h"


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
        QJsonValue jsonValue(const QVariant &val_) const;
        static QString toString(const QJsonValue &jv_);
    };

public:
    DBRequestInfo() = default;

    template<typename DataType_>
    void init(
            const QString &layoutName_,
            bool readonly_
            )
    {
        init<DataType_>(layoutName_,
                        QString(),
                        QStringList(),
                        QString(),
                        QString(),
                        QString(),
                        QVariant(),
                        QVariant(),
                        readonly_,
                        QHash<QString, QVariant>()
                        );
    }

    template<typename DataType_>
    void init(
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
        QStringList refs;
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

        if(!procedureName_.isEmpty())
        {
            refs.clear();
        }
        else
        {
            refs = refs_;
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
        }
        setTableName(tableName);
        setTableFieldsInfo(tableFieldsInfo);
        setRefs(refs);
        setCurrentRef(procedureName_.isEmpty() ? currentRef_ : QString());
        setIdField(procedureName_.isEmpty() ? idField : QVariant());
        setReadonly(readonly_);
        setExtraFields(extraFields_);
    }

    const QString &getTableName() const;
    const QList<JsonFieldInfo> &getTableFieldsInfo() const;
    const QStringList &getRefs() const;
    const QString &getCurrentRef() const;
    const QVariant &getIdField() const;
    bool getReadonly() const;
    const QHash<QString, QVariant> &getExtraFields() const;

    static QString namingConversion(const QString &name_);
    static QStringList getSqlNames(const QList<JsonFieldInfo> &tableFieldsInfo_);
    static QStringList getSqlBindNames(const QList<JsonFieldInfo> &tableFieldsInfo_);

protected:
    void setTableName(const QString &tableName_);
    void setTableFieldsInfo(const QList<JsonFieldInfo> &jsonFieldInfo_);
    void setRefs(const QStringList &refs_);
    void setCurrentRef(const QString &currentRef_);
    void setIdField(const QVariant &idField_);
    void setReadonly(bool readonly_);
    void setExtraFields(const QHash<QString, QVariant> &extraFields_);

private:
    QString m_tableName;
    QList<JsonFieldInfo> m_tableFieldsInfo;
    QStringList m_refs;
    QString m_currentRef;
    QVariant m_idField;
    QHash<QString, QVariant> m_extraFields;
    bool m_readonly = true;
};


#endif // DBREQUESTINFO_H
