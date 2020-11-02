#include "netapi.h"


const QString &RequestData::getRequestName() const
{
    return m_requestName;
}

QVariant RequestData::getItemId() const
{
    return m_itemId;
}

QVariant RequestData::getItemAppId() const
{
    return m_itemAppId;
}

void RequestData::setRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

void RequestData::setItenId(const QVariant &itemId_)
{
    m_itemId = itemId_;
}

void RequestData::setItenIndex(const QVariant &itemAppId_)
{
    m_itemAppId = itemAppId_;
}


NetAPI::NetAPI(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{

}
