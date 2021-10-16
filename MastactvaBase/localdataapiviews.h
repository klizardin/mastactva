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

#ifndef LOCALDATAAPIVIEWS_H
#define LOCALDATAAPIVIEWS_H


#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/qmlobjects.h"


template<class ModelType_>
class LocalDataAPIGelListByRefImpl : public ILocalDataGetAPI
{
public:
    LocalDataAPIGelListByRefImpl() = default;
    virtual ~LocalDataAPIGelListByRefImpl() = default;

    virtual bool canProcess(const DBRequestBase *r_) const
    {
        if(!r_) { return false; }
        const QString tableName = db::jsonToSql(
                    getDataLayout<typename ModelType_::DataType>().getLayoutJsonName()
                    );
        if(r_->getTableName() != tableName) { return false; }
        const QString requestNameGetList = RequestData::getListRequestName<typename ModelType_::DataType>();
        if(r_->getDBRequestName() != requestNameGetList) { return false; }
        if(r_->getCurrentRef().isEmpty()) { return false; }
        return true;
    }

    virtual bool getListImpl(DBRequestBase *r_) override
    {
        if(!r_) { return false; }
        LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
        if(!localDataAPI) { return false; }
        ILocalDataAPI *defaultAPI = r_->getDefaultAPI();
        if(!defaultAPI) { return false; }

        r_->insertExtraField(g_procedureExtraFieldName,
                             QVariant::fromValue(QHash<QString, QVariant>(
                                 {
                                     { QString(g_procedureConditionName),
                                       QVariant::fromValue(QString("%1=:%1").arg(r_->getCurrentRef()))
                                     },
                                     { QString(g_procedureArguments),
                                       QVariant::fromValue(
                                           QHash<QString, QVariant>(
                                           {
                                               {QString(":%1").arg(r_->getCurrentRef()), r_->getIdField()},
                                           }))
                                     }
                                 }))
                             );
        r_->clearReferences();

        DynamicCastPtr<ILocalDataGetAPI, ILocalDataAPI> defaultHandler(defaultAPI);
        if(!defaultHandler)
        {
            return false;
        }
        else
        {
            return defaultHandler->getListImpl(r_);
        }
    }
};


#endif // LOCALDATAAPIVIEWS_H
