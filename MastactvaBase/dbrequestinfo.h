/*
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

#ifndef DBREQUESTINFO_H
#define DBREQUESTINFO_H


#include <QString>
#include <QtSql>
#include <QVector>
#include <QList>
#include <QSet>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/requestdata.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/names.h"


template<typename DataType_> inline
void init(
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
        )
{
    QString tableName;
    QList<db::JsonSqlField> tableFieldsInfo;
    QVariant idField;


    const QString jsonLayoutName = layoutName_;
    tableName = db::jsonToSql(jsonLayoutName);
    const QString idFieldName = getDataLayout<DataType_>().getIdFieldJsonName();
    QList<QPair<QString, layout::JsonTypesEn>> fieldsInfo;
    getDataLayout<DataType_>().getJsonFieldsInfo(fieldsInfo);
    tableFieldsInfo.clear();
    for(const auto &jsonFieldName: qAsConst(fieldsInfo))
    {
        tableFieldsInfo.push_back(
                    db::JsonSqlField(jsonFieldName.first,
                                  jsonFieldName.second,
                                  idFieldName == jsonFieldName.first)
                    );
    }

    IListModel *parentModelPtr = QMLObjectsBase::getInstance().getListModel(parentModel_);
    if(parentModelPtr || (!refAppId_.isValid() && refValue_.isValid()))
    {
        idField =
            refValue_.isValid()
                ? refValue_
                : parentModelJsonFieldName_.isEmpty()
                  ? parentModelPtr->getIdFieldValueForAppId(refAppId_)
                  : parentModelPtr->getFieldValueForAppId(refAppId_, parentModelJsonFieldName_)
            ;
    }

    requestInfo_.setTableName(tableName);
    requestInfo_.setTableFieldsInfo(tableFieldsInfo);
    requestInfo_.setRefs(refs_);
    requestInfo_.setCurrentRef(currentRef_);
    requestInfo_.setIdField(idField);
    requestInfo_.setReadonly(readonly_);
    requestInfo_.setExtraFields(extraFields_);
    requestInfo_.setProcedureName(procedureName_);
    requestInfo_.setDBRequestName(requestName_);
}

template<typename DataType_> inline
void init(
        DBRequestBase &requestInfo_,
        const QString &requestName_,
        const QString &layoutName_,
        bool readonly_,
        const QHash<QString, QVariant> &extraFields_
        )
{
    init<DataType_>(requestInfo_,
                    requestName_,
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


class LocalDBRequest :
        public DBRequestBase,
        public RequestData
{
public:
    LocalDBRequest();
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
