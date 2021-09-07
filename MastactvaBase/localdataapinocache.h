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

#ifndef LOCALDATAAPINOCACHE_H
#define LOCALDATAAPINOCACHE_H


#include <QObject>
#include <QtSql>
#include <QVector>
#include <QList>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaBase/localdataapinocache_default.h"


// TODO : add pinup test for implamentation
// TODO : split to 2 interfaces ILocalDataReadAPI ILocalDataWriteAPI
class LocalDataAPINoCache : public QObject
{
    Q_OBJECT

public:
    explicit LocalDataAPINoCache(QObject *parent_ = nullptr);
    virtual ~LocalDataAPINoCache() override;

    static void createInstance(QObject *parent_, NetAPI *netAPI_);
    static LocalDataAPINoCache *getInstance();

    RequestData *emptyRequest(const QString &requestName_,
                              const QVariant &itemAppId_,
                              const QVariant &itemId_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->emptyRequest(requestName_, itemAppId_, itemId_);
    }

    void freeRequest(RequestData *&r_)
    {
        if(!m_netAPI) { return; }
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
        SaveDBRequest *r = nullptr;
        if(isSaveToDBMode())
        {
            r = new SaveDBRequest();
            init<DataType_>(*r, RequestData::getListRequestName<DataType_>(),
                               layoutName_,
                               procedureName_,
                               refs_, currentRef_,
                               parentModel_, parentModelJsonFieldName_,
                               refAppId_, refValue_,
                               readonly_,
                               extraFields_
                               );
            DynamicCastPtr<LocalDataAPINoCacheImpl,ILocalDataAPI> view(chooseAPI(r));
            if(!view)
            {
                m_defaultAPIImpl.getListImpl(r);
            }
            else
            {
                view->getListImpl(r);
            }
        }
        if(!m_netAPI) { return nullptr; }
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
        if(resRequest && isSaveToDBMode())
        {
            r->setRequest(resRequest);
            m_requests.push_back(r);
        }
        else
        {
            delete r;
            r = nullptr;
        }
        return resRequest;
    }

    template<class DataType_>
    RequestData *addItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->addItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *setItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->setItem(layoutName_, item_, extraFields_);
    }

    template<class DataType_>
    RequestData *delItem(const QString &layoutName_,
                         const DataType_ *item_,
                         const QHash<QString, QVariant> &extraFields_)
    {
        if(!m_netAPI) { return nullptr; }
        return m_netAPI->delItem(layoutName_, item_, extraFields_);
    }

    void startSave(const QString &savePath_);
    void endSave();

signals:
    void response(int errorCode_,
                  const QString &errorCodeStr_,
                  RequestData *request_,
                  const QJsonDocument &reply_);
    void error(int errorCode_,
               const QString &errorCodeStr_,
               const QJsonDocument &reply_);

protected slots:
    void responseFromStartegySlot(int errorCode_,
                                  const QString &errorCodeStr_,
                                  RequestData *request_,
                                  const QJsonDocument &reply_);
    void errorFromStartegySlot(int errorCode_,
                               const QString &errorCodeStr_,
                               const QJsonDocument &reply_);

protected:
    void cleanRequests();
    void cleanPath();
    void createDB();
    bool isSaveToDBMode() const;
    void fillTable(const SaveDBRequest * r_, const QJsonDocument &reply_);
    ILocalDataAPI *chooseAPI(DBRequestBase *r_);

private:
    static bool hasDuplicate(
            db::ISqlQuery *findQuery_,
            QJsonValue &replayItem_,
            const db::JsonSqlFieldsList &fields_,
            const db::JsonSqlFieldAndValuesList refsValues_,
            const db::JsonSqlFieldsList::const_iterator &idField_
            );
    static void insertItem(
            db::ISqlQuery *insertQuery_,
            QJsonValue &replayItem_,
            const db::JsonSqlFieldsList &fields_,
            const db::JsonSqlFieldAndValuesList &refsValues_
            );

private:
    NetAPI *m_netAPI = nullptr;
    QString m_savePath;
    QString m_dbNameRW;
    QString m_dbNameRO;
    QSqlDatabase m_databaseRW;
    QSqlDatabase m_databaseRO;
    QList<SaveDBRequest *> m_requests;
    LocalDataAPINoCacheImpl m_defaultAPIImpl;
    static LocalDataAPINoCache *g_localDataAPI;

    friend class LocalDataAPINoCacheImpl;
};


#endif // LOCALDATAAPINOCACHE_H
