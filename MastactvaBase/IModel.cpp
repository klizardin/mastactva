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

#include "IModel.h"
#include <QDebug>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/defines.h"


void IListModelInfoObjectImpl::startLoadChildModel()
{
#if defined(TRACE_MODEL_LOADING) || defined(TRACE_MODEL_LOADED)
    qDebug() << "-" << this << m_objectName << "startLoadChildModel()" << m_loadingChildenModels;
#endif

    ++m_loadingChildenModels;
    if(m_parentListModelInfo)
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
    if(m_parentListModelInfo)
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

void IListModelInfoObjectImpl::errorVF(int errorCode_,
                                       const QString &errorCodeStr_,
                                       const QJsonDocument &reply_)
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
    if(m_parentListModelInfo)
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
    if(m_parentListModelInfo)
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
