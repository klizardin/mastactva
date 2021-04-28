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
