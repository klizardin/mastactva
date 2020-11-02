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

void RequestData::allowModelProccessError()
{
    m_processErrorInNetAPI = false;
}

void RequestData::setRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

void RequestData::setItenId(const QVariant &itemId_)
{
    m_itemId = itemId_;
}

void RequestData::setItemAppId(const QVariant &itemAppId_)
{
    m_itemAppId = itemAppId_;
}

void RequestData::setReplay(QNetworkReply *reply_)
{
    m_reply = reply_;
}

bool RequestData::compare(QNetworkReply *reply_) const
{
    return nullptr != m_reply && m_reply == reply_;
}


NetAPI::NetAPI(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{

}
