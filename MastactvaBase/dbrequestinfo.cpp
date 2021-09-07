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

#include "dbrequestinfo.h"
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


LocalDBRequest::LocalDBRequest()
    :DBRequestBase(g_cachAPI)
{
}

void LocalDBRequest::addJsonResult(const QJsonDocument &doc_)
{
    m_doc = doc_;
}

void LocalDBRequest::addJsonResult(const QHash<QString, QVariant> &values_)
{
    QJsonArray array;
    QJsonObject obj = db::getJsonObject(values_, getTableFieldsInfo());
    array.push_back(obj);
    m_doc = QJsonDocument(array);
}

const QJsonDocument &LocalDBRequest::reply() const
{
    return m_doc;
}

void LocalDBRequest::setError(bool error_)
{
    m_error = error_;
}

bool LocalDBRequest::error() const
{
    return m_error;
}
