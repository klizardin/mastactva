#include "localdataapinocache_default.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


SaveDBRequest::SaveDBRequest()
    :DBRequestBase(g_noCachAPI)
{
}

bool SaveDBRequest::operator == (const RequestData *request_) const
{
    return m_request && request_ && m_request == request_;
}

void SaveDBRequest::setRequest(const RequestData *request_)
{
    m_request = request_;
}


bool LocalDataAPINoCacheImpl::canProcess(const DBRequestBase *r_) const
{
    const DBRequestPtr<const SaveDBRequest> r(r_);
    return r.operator bool();
}

/// HOW_TO_TEST: test of the method by testing db::getCreateTableSqlRequest() function
bool LocalDataAPINoCacheImpl::getListImpl(DBRequestBase *r_)
{
    const DBRequestPtr<SaveDBRequest> r(r_);
    if(!r)
    {
        return false;
    }

#if defined(TRACE_DB_CREATION)
    qDebug() << "readonly " << r->getReadonly();
#endif

    const QString createSqlRequest = db::getCreateTableSqlRequest(
                r->getTableName(),
                r->getCurrentRef(),
                r->getTableFieldsInfo(),
                r->getRefs(),
                DBRequestBase::apiExtraFields(r_->getExtraFields()).keys()
                );

#if defined(TRACE_DB_CREATION)
    qDebug() << "create sql" << createSqlRequest;
#endif

    auto query = getRequest(r);
    if(!query->exec(createSqlRequest))
    {
        const QSqlError err = query->lastError();
        qDebug() << "sql error "  << err.text();
    }
    return true;
}


