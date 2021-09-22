#include "dualmodelconfig.h"


bool DualModelConfigBase::isInit() const
{
    return m_localDatAPI2;
}

void DualModelConfigBase::init(QObject *parent_, NetAPI *netAPI_)
{
    LocalDataAPI2::createInstance(parent_, netAPI_);
    m_localDatAPI2 = LocalDataAPI2::getInstance();
}

IListModel *DualModelConfigBase::getListModel(const QString &layoutName_)
{
    return QMLObjectsBase::getInstance().getListModel(layoutName_);
}

RequestData *DualModelConfigBase::emptyRequest(
        const QString &requestName_,
        const QVariant &itemAppId_,
        const QVariant &itemId_
        )
{
    Q_UNUSED(requestName_);
    Q_UNUSED(itemAppId_);
    Q_UNUSED(itemId_);
    Q_ASSERT(false); // should not be used
    return nullptr;
}

void DualModelConfigBase::freeRequest(RequestData *&r_)
{
    Q_UNUSED(r_);
    Q_ASSERT(false); // should not be used
}

DualModelConfigBase &DualModelConfigBase::instance()
{
    static DualModelConfigBase inst;
    return inst;
}

bool DualModelConfigBase::isDataAPIServer() const
{
    Q_ASSERT(false); // should not be used
    return false;
}

LocalDataAPINoCache *DualModelConfigBase::getDataAPIServer()
{
    LocalDataAPINoCache *res = dynamic_cast<LocalDataAPINoCache *>(QMLObjectsBase::getInstance().getDataAPI());
    if(!res)
    {
        res = dynamic_cast<LocalDataAPINoCache *>(m_localDatAPI2);
    }
    return res;
}

LocalDataAPICache *DualModelConfigBase::getDataAPIFile()
{
    LocalDataAPICache *res = dynamic_cast<LocalDataAPICache *>(QMLObjectsBase::getInstance().getDataAPI());
    if(!res)
    {
        res = dynamic_cast<LocalDataAPICache *>(m_localDatAPI2);
    }
    return res;
}

ChooseModelConfig::ChooseModelConfig(bool serverLocalDataAPI_)
    : m_serverLocalDataAPI(serverLocalDataAPI_)
{
}

IListModel *ChooseModelConfig::getListModel(const QString &layoutName_)
{
    return DualModelConfigBase::instance().getListModel(layoutName_);
}

RequestData *ChooseModelConfig::emptyRequest(
        const QString &requestName_,
        const QVariant &itemAppId_,
        const QVariant &itemId_
        )
{
    if(isDataAPIServer())
    {
        if(getDataAPIServer())
        {
            return getDataAPIServer()->emptyRequest(
                        requestName_,
                        itemAppId_,
                        itemId_
                        );
        }
    }
    else
    {
        if(getDataAPIFile())
        {
            return getDataAPIFile()->emptyRequest(
                        requestName_,
                        itemAppId_,
                        itemId_
                        );
        }
    }
    return nullptr;
}

void ChooseModelConfig::freeRequest(RequestData *&r_)
{
    if(isDataAPIServer())
    {
        if(getDataAPIServer())
        {
            getDataAPIServer()->freeRequest(
                        r_
                        );
        }
    }
    else
    {
        if(getDataAPIFile())
        {
            getDataAPIFile()->freeRequest(
                        r_
                        );
        }
    }
}

bool ChooseModelConfig::isDataAPIServer() const
{
    return m_serverLocalDataAPI;
}

LocalDataAPINoCache *ChooseModelConfig::getDataAPIServer()
{
    return DualModelConfigBase::instance().getDataAPIServer();
}

LocalDataAPICache *ChooseModelConfig::getDataAPIFile()
{
    return DualModelConfigBase::instance().getDataAPIFile();
}
