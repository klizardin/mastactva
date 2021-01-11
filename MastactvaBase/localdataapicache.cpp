#include "localdataapicache.h"


LocalDataAPICache::LocalDataAPICache(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
}

LocalDataAPICache::~LocalDataAPICache()
{
    freeRequests();
}


LocalDataAPICache *LocalDataAPICache::g_localDataAPI = nullptr;


void LocalDataAPICache::createInstance(QObject *parent_)
{
    if(nullptr == g_localDataAPI)
    {
        g_localDataAPI = new LocalDataAPICache(parent_);
    }
}

LocalDataAPICache *LocalDataAPICache::getInstance()
{
    return g_localDataAPI;
}

void LocalDataAPICache::freeRequests()
{
}

RequestData *LocalDataAPICache::getList(const QString &layoutName_, const QHash<QString, QVariant> &extraFields_, LocalDBRequest *r_)
{
    return nullptr;
}
