#ifndef LOCALDATAAPINOCACHE_H
#define LOCALDATAAPINOCACHE_H


#include <QObject>
#include <QtSql>
#include <QVector>
#include <QList>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/netapi.h"


class LocalDataAPINoCache : public QObject
{
    Q_OBJECT

private:
    struct JsonFieldInfo
    {
        QString jsonName;
        QString sqlName;
        layout::JsonTypesEn type;

        QString getSqlType() const;
        QString getBindName() const;
        void bind(QSqlQuery &query_, const QJsonValue &jv_) const;
    };

    class SaveDBRequest
    {
    public:
        explicit SaveDBRequest(const RequestData *request_);
        bool operator == (const RequestData *request_) const;

        const QString &getTableName() const;
        void setTableName(const QString &tableName_);
        const QList<JsonFieldInfo> &getTableFieldsInfo() const;
        void setTableFieldsInfo(const QList<JsonFieldInfo> &jsonFieldInfo_);
        const QStringList &getRefs() const;
        void setRefs(const QStringList &refs_);
        const QString &getCurrentRef() const;
        void setCurrentRef(const QString &currentRef_);
        const QVariant &getIdField() const;
        void setIdField(const QVariant &idField_);
        bool getReadonly() const;
        void setReadonly(bool readonly_);

    private:
        const RequestData *m_request = nullptr;
        QString m_tableName;
        QList<JsonFieldInfo> m_tableFieldsInfo;
        QStringList m_refs;
        QString m_currentRef;
        QVariant m_idField;
        bool m_readonly = true;
    };

public:
    explicit LocalDataAPINoCache(QObject *parent_ = nullptr);
    virtual ~LocalDataAPINoCache() override;

    static void createInstance(QObject *parent_, NetAPI *netAPI_);
    static LocalDataAPINoCache *getInstance();

    RequestData *emptyRequest(const QString &requestName_, const QVariant &itemAppId_, const QVariant &itemId_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->emptyRequest(requestName_, itemAppId_, itemId_);
    }

    void freeRequest(RequestData *&r_)
    {
        if(nullptr == m_netAPI) { return; }
        m_netAPI->freeRequest(r_);
    }

    template<class DataType_>
    RequestData *getList(const QString &layoutName_,
                         const QString &procedureName_,
                         const QStringList &refs_,
                         const QString &currentRef_,
                         const QString &parentModel_,
                         const QString &parentModelJsonFieldName_,
                         const QVariant &refAppId_,
                         const QVariant &refValue_,
                         bool jsonParams_,
                         const QHash<QString, QVariant> &extraFields_,
                         bool readonly_
                         )
    {
        QString tableName;
        QList<JsonFieldInfo> tableFieldsInfo;
        QStringList refs;
        QVariant idField;
        if(isSaveToDBMode())
        {
            const QString jsonLayoutName = getDataLayout<DataType_>().getLayoutJsonName();
            tableName = namingConversion(jsonLayoutName);
            QList<QPair<QString, layout::JsonTypesEn>> fieldsInfo;
            getDataLayout<DataType_>().getJsonFieldsInfo(fieldsInfo);
            tableFieldsInfo.clear();
            for(const auto &jsonFieldName: fieldsInfo)
            {
                const QString sqlFieldName = namingConversion(jsonFieldName.first);
                tableFieldsInfo.push_back({jsonFieldName.first, sqlFieldName, jsonFieldName.second});
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

            createTable(tableName, tableFieldsInfo, refs, procedureName_.isEmpty() ? currentRef_ : QString(), readonly_);
        }
        if(nullptr == m_netAPI) { return nullptr; }
        RequestData *resRequest = m_netAPI->getList<DataType_>(layoutName_,
                                             procedureName_,
                                             refs_,
                                             currentRef_,
                                             parentModel_,
                                             parentModelJsonFieldName_,
                                             refAppId_,
                                             refValue_,
                                             jsonParams_,
                                             extraFields_,
                                             readonly_
                                             );
        if(nullptr != resRequest && isSaveToDBMode())
        {
            SaveDBRequest *r = new SaveDBRequest(resRequest);
            r->setTableName(tableName);
            r->setTableFieldsInfo(tableFieldsInfo);
            r->setRefs(refs);
            r->setCurrentRef(procedureName_.isEmpty() ? currentRef_ : QString());
            r->setIdField(procedureName_.isEmpty() ? idField : QVariant());
            r->setReadonly(readonly_);
            m_requests.push_back(r);
        }
        return resRequest;
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->addItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_, const DataType_ *item_, const QHash<QString, QVariant> &extraFields_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->setItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_, const DataType_ *item_)
    {
        if(nullptr == m_netAPI) { return nullptr; }
        return m_netAPI->delItem(layoutName_, item_);
    }

    void startSave(const QString &savePath_);
    void endSave();

signals:
    void response(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_);
    void error(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_);

protected slots:
    void responseFromStartegySlot(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_);
    void errorFromStartegySlot(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_);

protected:
    void cleanRequests();
    void cleanPath();
    void createDB();
    bool isSaveToDBMode() const;
    void createTable(const QString &tableName_,
                     const QList<JsonFieldInfo> &tableFieldsInfo_,
                     const QStringList &refs_,
                     const QString &currentRef_,
                     bool readonly_);
    void fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_);
    QStringList getSqlNames(const QList<JsonFieldInfo> &tableFieldsInfo_) const;
    QStringList getSqlBindNames(const QList<JsonFieldInfo> &tableFieldsInfo_) const;

protected:
    static QString namingConversion(const QString &name_);

private:
    NetAPI *m_netAPI = nullptr;
    QString m_savePath;
    QString m_dbNameRW;
    QString m_dbNameRO;
    QSqlDatabase m_databaseRW;
    QSqlDatabase m_databaseRO;
    QList<SaveDBRequest *> m_requests;
    static LocalDataAPINoCache *g_localDataAPI;
};


#endif // LOCALDATAAPINOCACHE_H
