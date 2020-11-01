#include "netapi.h"


const QString &RequestData::getRequestName() const
{
    return m_requestName;
}

int RequestData::getItemId() const
{
    return m_itemId;
}

int RequestData::getItemIndex() const
{
    return m_itemIndex;
}

void RequestData::setRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

void RequestData::setItenId(int itemId_)
{
    m_itemId = itemId_;
}

void RequestData::setItenIndex(int itemIndex_)
{
    m_itemIndex = itemIndex_;
}


NetAPI::NetAPI(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{

}
