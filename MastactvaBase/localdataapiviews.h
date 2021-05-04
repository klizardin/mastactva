#ifndef LOCALDATAAPIVIEWS_H
#define LOCALDATAAPIVIEWS_H


#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/qmlobjects.h"


template<class ModelType_>
class LocalDataAPIGelListByRefImpl : public ILocalDataAPI
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

        return defaultAPI->getListImpl(r_);
    }

    virtual bool addItemImpl(const QVariant &appId_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override
    {
        Q_UNUSED(appId_);
        Q_UNUSED(values_);
        Q_UNUSED(r_);
        return false;
    }

    virtual bool setItemImpl(const QVariant &id_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override
    {
        Q_UNUSED(id_);
        Q_UNUSED(values_);
        Q_UNUSED(r_);
        return false;
    }

    virtual bool delItemImpl(const QVariant &id_, DBRequestBase *r_) override
    {
        Q_UNUSED(id_);
        Q_UNUSED(r_);
        return false;
    }
};


#endif // LOCALDATAAPIVIEWS_H
