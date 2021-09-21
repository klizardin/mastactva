#ifndef MODELCONFIG_H
#define MODELCONFIG_H


#include <type_traits>
#include <QString>
#include <QVariant>
#include <QHash>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaBase/qmlobjects.h"


class LocalDataAPINoCache;
class LocalDataAPICache;
#if defined(LOCALDATAAPICACHE)
using LocalDataAPI = LocalDataAPICache;
#else
using LocalDataAPI = LocalDataAPINoCache;
#endif


class IModelConfig
{
public:
    virtual ~IModelConfig() = default;
    virtual IListModel *getListModel(const QString &layoutName_) = 0;

protected:
    virtual bool isDataAPIServer() const = 0;
    virtual LocalDataAPINoCache *getDataAPIServer() = 0;
    virtual LocalDataAPICache *getDataAPIFile() = 0;

public:
    template<class DataType_>
    RequestData *getList(
            const QString &layoutName_,
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
        if(isDataAPIServer())
        {
            if(getDataAPIServer())
            {
                return getDataAPIServer()->getList<DataType_>(
                            layoutName_,
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
            }
        }
        else
        {
            if(getDataAPIFile())
            {
                return getDataAPIFile()->getList<DataType_>(
                            layoutName_,
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
            }
        }
        return nullptr;
    }

    template<class DataType_>
    RequestData *addItem(
            const QString &layoutName_,
            const DataType_ *item_,
            const QHash<QString, QVariant> &extraFields_
            )
    {
        if(isDataAPIServer())
        {
            if(getDataAPIServer())
            {
                return getDataAPIServer()->addItem<DataType_>(
                            layoutName_,
                            item_,
                            extraFields_
                            );
            }
        }
        else
        {
            if(getDataAPIFile())
            {
                return getDataAPIFile()->addItem<DataType_>(
                            layoutName_,
                            item_,
                            extraFields_
                            );
            }
        }
        return nullptr;
    }

    template<class DataType_>
    RequestData *setItem(
            const QString &layoutName_,
            const DataType_ *item_,
            const QHash<QString, QVariant> &extraFields_
            )
    {
        if(isDataAPIServer())
        {
            if(getDataAPIServer())
            {
                return getDataAPIServer()->setItem<DataType_>(
                            layoutName_,
                            item_,
                            extraFields_
                            );
            }
        }
        else
        {
            if(getDataAPIFile())
            {
                return getDataAPIFile()->setItem<DataType_>(
                            layoutName_,
                            item_,
                            extraFields_
                            );
            }
        }
        return nullptr;
    }

    template<class DataType_>
    RequestData *delItem(
            const QString &layoutName_,
            const DataType_ *item_,
            const QHash<QString, QVariant> &extraFields_
            )
    {
        if(isDataAPIServer())
        {
            if(getDataAPIServer())
            {
                return getDataAPIServer()->delItem<DataType_>(
                            layoutName_,
                            item_,
                            extraFields_
                            );
            }
        }
        else
        {
            if(getDataAPIFile())
            {
                return getDataAPIFile()->delItem<DataType_>(
                            layoutName_,
                            item_,
                            extraFields_
                            );
            }
        }
        return nullptr;
    }

    template<class ModelType_>
    bool initResponse(ModelType_ *model_)
    {
        if(isDataAPIServer())
        {
            if(getDataAPIServer())
            {
                QObject::connect(
                            getDataAPIServer(),
                            SIGNAL(response(int, const QString &, RequestData *, const QJsonDocument &)),
                            model_,
                            SLOT(jsonResponseSlot(int, const QString &, RequestData *, const QJsonDocument &))
                            );
                return true;
            }
        }
        else
        {
            if(getDataAPIFile())
            {
                QObject::connect(
                            getDataAPIFile(),
                            SIGNAL(response(int, const QString &, RequestData *, const QJsonDocument &)),
                            model_,
                            SLOT(jsonResponseSlot(int, const QString &, RequestData *, const QJsonDocument &))
                            );
                return true;
            }
        }
        return false;
    }

    template<class ModelType_>
    bool clearResponse(ModelType_ *model_)
    {
        if(isDataAPIServer())
        {
            if(getDataAPIServer())
            {
                QObject::disconnect(
                            getDataAPIServer(),
                            SIGNAL(response(int, const QString &, RequestData *, const QJsonDocument &)),
                            model_,
                            SLOT(jsonResponseSlot(int, const QString &, RequestData *, const QJsonDocument &))
                            );
                return true;
            }
        }
        else
        {
            if(getDataAPIFile())
            {
                QObject::disconnect(
                            getDataAPIFile(),
                            SIGNAL(response(int, const QString &, RequestData *, const QJsonDocument &)),
                            model_,
                            SLOT(jsonResponseSlot(int, const QString &, RequestData *, const QJsonDocument &))
                            );
                return true;
            }
        }
        return false;
    }
};


class DefaultModelConfig : public IModelConfig
{
public:
    DefaultModelConfig() = default;
    virtual ~DefaultModelConfig() = default;

    virtual IListModel *getListModel(const QString &layoutName_)
    {
        return QMLObjectsBase::getInstance().getListModel(layoutName_);
    }

protected:
    virtual bool isDataAPIServer() const
    {
        return dynamic_cast<LocalDataAPINoCache *>(QMLObjectsBase::getInstance().getDataAPI());
    }

    virtual LocalDataAPINoCache *getDataAPIServer()
    {
        return dynamic_cast<LocalDataAPINoCache *>(QMLObjectsBase::getInstance().getDataAPI());
    }

    virtual LocalDataAPICache *getDataAPIFile()
    {
        return dynamic_cast<LocalDataAPICache *>(QMLObjectsBase::getInstance().getDataAPI());
    }

    static DefaultModelConfig &instance()
    {
        static DefaultModelConfig inst;
        return inst;
    }
};


#endif // MODELCONFIG_H
