#include "IModel.h"

void IListModelInfoObjectImpl::startLoadChildModel()
{
    ++m_loadingChildenModels;
}

void IListModelInfoObjectImpl::endLoadChildModel()
{
    --m_loadingChildenModels;
    Q_ASSERT(m_loadingChildenModels >= 0);
    if(isListLoadedImpl())
    {
        listLoadedVF();
    }
}

bool IListModelInfoObjectImpl::isListLoadedImpl() const
{
    return 0 == m_loadingChildenModels;
}

void IListModelInfoObjectImpl::listLoadedVF()
{
}

void IListModelInfoObjectImpl::itemAddedVF()
{
}

void IListModelInfoObjectImpl::itemSetVF()
{
}

void IListModelInfoObjectImpl::itemDeletedVF()
{
}

void IListModelInfoObjectImpl::errorVF(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
{
    Q_UNUSED(errorCode_);
    Q_UNUSED(errorCodeStr_);
    Q_UNUSED(reply_);
}
