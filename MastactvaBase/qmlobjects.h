#ifndef QMLOBJECTS_H
#define QMLOBJECTS_H


#include <QVector>
#include "../MastactvaBase/IModel.h"


class QObject;
class NetAPI;
class ILocalDataAPI;
class LocalDataAPINoCache;
class LocalDataAPICache;
#if defined(LOCALDATAAPICACHE)
using LocalDataAPI = LocalDataAPICache;
#else
using LocalDataAPI = LocalDataAPINoCache;
#endif
class ServerFiles;


template<typename ModelType_>
class ModelArg
{
public:
    using ModelType = ModelType_;

    ModelArg(const char *modelName_)
        : m_ModelName(modelName_)
    {
    }
    const char *m_ModelName;
};

template<typename ModelType_> inline
ModelArg<ModelType_> modelArg(const char *modelName_)
{
    return ModelArg<ModelType_>(modelName_);
}

class QMLObjectsBase
{
protected:
    QMLObjectsBase() = default;

private:
    static QMLObjectsBase *getInstancePtr();

public:
    static QMLObjectsBase &getInstance();
    static bool hasInstance();

    void setRoot(QObject *root_);
    NetAPI *getNetAPI();
    LocalDataAPI *getDataAPI();
    ServerFiles *getServerFiles();
    IListModel *getListModel(const QString &layoutName_);
    const QVector<ILocalDataAPI *> &getLocalDataAPIViews();
    void registerModel(const QString &layoutName_, IListModel *m_);
    void unregisterModel(const QString &layoutName_);

    virtual void setInitialized() = 0;
    virtual void modelError(
            int errorCode_,
            const QString &errorCodeStr_,
            const QJsonDocument &reply_) = 0;

protected:
    virtual void searchObjects() = 0;
    IListModel *findListModel(const QString &layoutName_) const;
    virtual void clearDependedFromRoot();

    template<typename ModelType_, typename ... ModelArgs_>
    void registerModels(const ModelArg<ModelType_>& arg_, const ModelArgs_& ... args_);
    template<typename ModelType_>
    void registerModels(const ModelArg<ModelType_>& arg_);
    void postRegisterModel();

protected:
    QObject *m_root = nullptr;
    QVector<IListModel *> m_models;
    NetAPI *m_netAPI = nullptr;
    LocalDataAPI *m_dataAPI = nullptr;
    ServerFiles *m_serverFiles = nullptr;
    QVector<ILocalDataAPI *> m_localDataAPIViews;
};

template<typename ModelType_, typename ... ModelArgs_> inline
void QMLObjectsBase::registerModels(const ModelArg<ModelType_>& arg_, const ModelArgs_& ... args_)
{
    registerModels(arg_);
    registerModels(args_ ...);
}

template<typename ModelType_> inline
void QMLObjectsBase::registerModels(const ModelArg<ModelType_>& arg_)
{
    if(!m_root) { return; }
    IListModel *m = nullptr;
    m = findListModel(arg_.m_ModelName);
    if(!m)
    {
        ModelType_ *m1 = m_root->findChild<ModelType_ *>(arg_.m_ModelName);
        registerModel(arg_.m_ModelName, m1);
    }
}

#endif // QMLOBJECTS_H
