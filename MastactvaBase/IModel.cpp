#include "IModel.h"
#include "qmlobjects.h"


void IListModelInfoObjectImpl::startLoadChildModel()
{
    ++m_loadingChildenModels;
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void IListModelInfoObjectImpl::endLoadChildModel()
{
    --m_loadingChildenModels;
    Q_ASSERT(m_loadingChildenModels >= 0);
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }
    if(isListLoadedImpl())
    {
        listLoadedVF();
    }
}

bool IListModelInfoObjectImpl::isListLoadedImpl() const
{
    return 0 == m_loadingChildenModels && m_childrenLoaded && !m_childrenLoading;
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
    QMLObjectsBase::getInstance().modelError(errorCode_, errorCodeStr_, reply_);
}

void IListModelInfoObjectImpl::setParentModelInfo(IListModelInfo *parentListModelInfo_)
{
    m_parentListModelInfo = parentListModelInfo_;
}

void IListModelInfoObjectImpl::loadChildrenVF()
{
    m_childrenLoading = true;
}

void IListModelInfoObjectImpl::objectLoadedVF()
{
    m_childrenLoading = false;
    m_childrenLoaded = true;

    if(isListLoadedImpl())
    {
        listLoadedVF();
    }
}
