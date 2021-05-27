#ifndef LOCALDATAAPINOCACHE_DEFAULT_H
#define LOCALDATAAPINOCACHE_DEFAULT_H


#include "../MastactvaBase/dbutils.h"


class RequestData;


class SaveDBRequest : public DBRequestBase
{
public:
    SaveDBRequest();
    bool operator == (const RequestData *request_) const;
    void setRequest(const RequestData *request_);

private:
    const RequestData *m_request = nullptr;
};


class LocalDataAPINoCacheImpl : public ILocalDataGetAPI
{
public:
    bool canProcess(const DBRequestBase *r_) const override;
    bool getListImpl(DBRequestBase *r_) override;
};


#endif // LOCALDATAAPINOCACHE_DEFAULT_H
