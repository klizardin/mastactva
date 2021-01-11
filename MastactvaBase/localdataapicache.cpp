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

RequestData *LocalDataAPICache::getListImpl(const QString& requestName_, const QString &layoutName_, const QHash<QString, QVariant> &extraFields_, LocalDBRequest *r_)
{
    return nullptr;
}

RequestData *LocalDataAPICache::addItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
{
    return nullptr;
}

RequestData *LocalDataAPICache::setItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_, LocalDBRequest *r_)
{
    return nullptr;
}

RequestData *LocalDataAPICache::delItemImpl(const QString& requestName_, const QString &layoutName_, const QVariant &id_, LocalDBRequest *r_)
{
    return nullptr;
}

QHash<QString, QVariant> LocalDataAPICache::merge(const QHash<QString, QVariant> &v1_, const QHash<QString, QVariant> &v2_)
{
    QHash<QString, QVariant> res;
    for(QHash<QString, QVariant>::const_iterator it = v2_.begin(); it != v2_.end(); ++it)
    {
        if(v1_.contains(it.key()))
        {
            res.insert(it.key(), v1_.value(it.key())); // for common prefere v1_
        }
        else
        {
            res.insert(it.key(), it.value());
        }
    }
    for(QHash<QString, QVariant>::const_iterator it = v1_.begin(); it != v1_.end(); ++it)
    {
        if(!v2_.contains(it.key()))
        {
            res.insert(it.key(), it.value());
        }
    }
    return res;
}
