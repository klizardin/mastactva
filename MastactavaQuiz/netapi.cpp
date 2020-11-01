#include "netapi.h"


const QString &RequestData::getRequestName() const
{
    return m_requestName;
}

void RequestData::setRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

NetAPI::NetAPI(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{

}
