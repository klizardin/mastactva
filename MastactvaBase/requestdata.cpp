/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

void RequestData::setItemData(IListModelItem *itemData_)
{
    m_itemData = itemData_;
}

IListModelItem *RequestData::getItemData()
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
