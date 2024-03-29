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


class ChooseModelConfig;


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
    QString getNamespace() const override;
    static DualModelConfigBase &instance();

    bool isDataAPIServer() const override;
    LocalDataAPINoCache *getDataAPIServer() override;
    LocalDataAPICache *getDataAPIFile() override;

private:
    LocalDataAPI2 *m_localDatAPI2 = nullptr;

    friend class ChooseModelConfig;
};


class ChooseModelConfig : public IModelConfig
{
public:
    ChooseModelConfig(bool serverLocalDataAPI_, const QString &serverFilesNamespace_);
    IListModel *getListModel(const QString &layoutName_) override;
    RequestData *emptyRequest(
            const QString &requestName_,
            const QVariant &itemAppId_,
            const QVariant &itemId_
            ) override;
    void freeRequest(RequestData *&r_) override;
    QString getNamespace() const override;

    bool isDataAPIServer() const override;
    LocalDataAPINoCache *getDataAPIServer() override;
    LocalDataAPICache *getDataAPIFile() override;

private:
    bool m_serverLocalDataAPI = false;
    QString m_serverFilesNamespace;
};


#endif // DUALMODELCONFIG_H
