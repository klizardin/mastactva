#include "IModel.h"

void IListModelInfoObjectImpl::startLoadChildModel()
{
    ++m_loadingChildenModels;
}

void IListModelInfoObjectImpl::endLoadChildModel()
{
    --m_loadingChildenModels;
    Q_ASSERT(m_loadingChildenModels >= 0);
}

bool IListModelInfoObjectImpl::isListLoadedImpl() const
{
    return 0 == m_loadingChildenModels;
}
