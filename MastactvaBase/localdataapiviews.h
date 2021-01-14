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

    virtual bool canProcess(const LocalDBRequest *r_) const
    {
        if(nullptr == r_) { return false; }
        const QString tableName = DBRequestInfo::namingConversion(
                    getDataLayout<typename ModelType_::DataType>().getLayoutJsonName()
                    );
        if(r_->getTableName() != tableName) { return false; }
        const QString requestNameGetList = RequestData::getListRequestName<typename ModelType_::DataType>();
        if(r_->getDBRequestName() != requestNameGetList) { return false; }
        return true;
    }

    virtual RequestData *getListImpl(LocalDBRequest *r_) override
    {
        if(nullptr == r_) { return nullptr; }
        LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
        if(nullptr == localDataAPI) { return nullptr; }
        ILocalDataAPI *defaultAPI = r_->getDefaultAPI();
        if(nullptr == defaultAPI) { return nullptr; }

        r_->insertExtraField(g_procedureExtraFieldName,
                             QVariant::fromValue(
                                 QHash<QString, QVariant>(
                                     {
                                         { g_procedureDefaultAPI, QVariant::fromValue(true) },
                                         { g_procedureConditionName, QVariant::fromValue(QString("%1=:%1").arg(r_->getCurrentRef())) },
                                         { g_procedureArguments, QVariant::fromValue(QList<QVariant>({r_->getIdField(),})) }
                                     })
                                 )
                             );
        r_->clearReferences();

        return defaultAPI->getListImpl(r_);
    }

    virtual RequestData *addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_) override
    {
        Q_UNUSED(appId_);
        Q_UNUSED(values_);
        Q_UNUSED(r_);
        return nullptr;
    }

    virtual RequestData *setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_) override
    {
        Q_UNUSED(id_);
        Q_UNUSED(values_);
        Q_UNUSED(r_);
        return nullptr;
    }

    virtual RequestData *delItemImpl(const QVariant &id_, LocalDBRequest *r_) override
    {
        Q_UNUSED(id_);
        Q_UNUSED(r_);
        return nullptr;
    }
};


#endif // LOCALDATAAPIVIEWS_H
