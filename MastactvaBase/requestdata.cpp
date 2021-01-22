#include "requestdata.h"
#include "../MastactvaBase/defines.h"


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

void RequestData::setItemData(void *itemData_)
{
    m_itemData = itemData_;
}

void *RequestData::getItemData()
{
    return m_itemData;
}

void RequestData::setSetCurrentItemIndex(bool setCurrentItemIndex_)
{
    m_setCurrentItemIndex = setCurrentItemIndex_;
}

bool RequestData::getSetCurrentItemIndex() const
{
    return m_setCurrentItemIndex;
}

bool RequestData::isRetried() const
{
    return m_retry;
}

void RequestData::setRetry()
{
    m_retry = true;
}

void RequestData::setRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

void RequestData::setItemId(const QVariant &itemId_)
{
    m_itemId = itemId_;
}

void RequestData::setItemAppId(const QVariant &itemAppId_)
{
    m_itemAppId = itemAppId_;
}

bool RequestData::processErrorInNetAPI() const
{
    return m_processErrorInNetAPI;
}
