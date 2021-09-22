#ifndef DUALMODELCONFIG_H
#define DUALMODELCONFIG_H

#include <type_traits>
#include <QString>
#include <QVariant>
#include <QHash>
#include "../MastactvaBase/modelconfig.h"


class LocalDataAPINoCache;
class LocalDataAPICache;
class RequestData;
#if defined(LOCALDATAAPICACHE)
using LocalDataAPI = LocalDataAPICache;
using LocalDataAPI2 = LocalDataAPINoCache;
#else
using LocalDataAPI = LocalDataAPINoCache;
using LocalDataAPI2 = LocalDataAPICache;
#endif


class DualModelConfig;


class DualModelConfigBase : public IModelConfig
{
public:
    DualModelConfigBase() = default;
    virtual ~DualModelConfigBase() = default;

    bool isInit() const;
    void init(QObject *parent_, NetAPI *netAPI_);
    IListModel *getListModel(const QString &layoutName_) override;
    RequestData *emptyRequest(
            const QString &requestName_,
            const QVariant &itemAppId_,
            const QVariant &itemId_
            ) override;
    void freeRequest(RequestData *&r_) override;
    static DualModelConfigBase &instance();

protected:
    bool isDataAPIServer() const override;
    LocalDataAPINoCache *getDataAPIServer() override;
    LocalDataAPICache *getDataAPIFile() override;

private:
    LocalDataAPI2 *m_localDatAPI2 = nullptr;

    friend class DualModelConfig;
};


class DualModelConfig : public IModelConfig
{
public:
    DualModelConfig(bool serverLocalDataAPI_);
    IListModel *getListModel(const QString &layoutName_) override;
    RequestData *emptyRequest(
            const QString &requestName_,
            const QVariant &itemAppId_,
            const QVariant &itemId_
            ) override;
    void freeRequest(RequestData *&r_) override;

protected:
    bool isDataAPIServer() const override;
    LocalDataAPINoCache *getDataAPIServer() override;
    LocalDataAPICache *getDataAPIFile() override;

private:
    bool m_serverLocalDataAPI = false;
};


#endif // DUALMODELCONFIG_H
