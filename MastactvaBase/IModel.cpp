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
