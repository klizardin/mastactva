#include "IModel.h"
#include <QDebug>
#include "../MastactvaBase/qmlobjects.h"


#define TRACE_MODEL_LOADING
#define TRACE_MODEL_LOADED


void IListModelInfoObjectImpl::startLoadChildModel()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-" << this << m_objectName << "startLoadChildModel()" << m_loadingChildenModels;
#endif

    ++m_loadingChildenModels;
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void IListModelInfoObjectImpl::endLoadChildModel()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << this << m_objectName << "endLoadChildModel()" << m_loadingChildenModels;
#endif

    --m_loadingChildenModels;

#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    if(m_loadingChildenModels < 0)
    {
        qDebug() << "m_loadingChildenModels < 0" << m_objectName << "endLoadChildModel()" << m_loadingChildenModels;
    }
#endif

    Q_ASSERT(m_loadingChildenModels >= 0);
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }

    if(isListLoadedImpl())
    {
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << this << m_objectName << "endLoadChildModel(), listLoadedVF()";
#endif
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

void IListModelInfoObjectImpl::setObjectName(const QString &objName_)
{
    m_objectName = objName_;
}

void IListModelInfoObjectImpl::trace()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << "trace()" << this << m_objectName << "m_childrenLoading =" << m_childrenLoading << "m_childrenLoaded = " << m_childrenLoaded << "m_loadingChildenModels =" << m_loadingChildenModels;
#endif
}

void IListModelInfoObjectImpl::loadChildrenVF()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << this << m_objectName << "loadChildrenVF()";
#endif

    m_childrenLoading = true;
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->startLoadChildModel();
    }
}

void IListModelInfoObjectImpl::objectLoadedVF()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << this << m_objectName << "objectLoadedVF()";
#endif

    m_childrenLoading = false;
    m_childrenLoaded = true;
    if(nullptr != m_parentListModelInfo)
    {
        m_parentListModelInfo->endLoadChildModel();
    }

    if(isListLoadedImpl())
    {
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-"  << this << m_objectName << "objectLoadedVF() listLoadedVF()";
#endif
        listLoadedVF();
    }
}
