#ifndef LOCALDATAAPIVIEWS_H
#define LOCALDATAAPIVIEWS_H


#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


template<class ModelType_>
class LocalDataAPIGelListByRefImpl : public ILocalDataAPI
{
public:
    LocalDataAPIGelListByRefImpl() = default;
    virtual ~LocalDataAPIGelListByRefImpl() = default;

    virtual bool canProcess(const LocalDBRequest *r_) const
    {
        Q_UNUSED(r_);
        return false;
    }

    virtual RequestData *getListImpl(const QString& requestName_, LocalDBRequest *r_)
    {
        Q_UNUSED(requestName_);
        Q_UNUSED(r_);
        return nullptr;
    }

    virtual RequestData *addItemImpl(const QString& requestName_, const QVariant &appId_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
    {
        Q_UNUSED(requestName_);
        Q_UNUSED(appId_);
        Q_UNUSED(values_);
        Q_UNUSED(r_);
        return nullptr;
    }

    virtual RequestData *setItemImpl(const QString& requestName_, const QVariant &id_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
    {
        Q_UNUSED(requestName_);
        Q_UNUSED(id_);
        Q_UNUSED(values_);
        Q_UNUSED(r_);
        return nullptr;
    }

    virtual RequestData *delItemImpl(const QString& requestName_, const QVariant &id_, LocalDBRequest *r_)
    {
        Q_UNUSED(requestName_);
        Q_UNUSED(id_);
        Q_UNUSED(r_);
        return nullptr;
    }
};


#endif // LOCALDATAAPIVIEWS_H
