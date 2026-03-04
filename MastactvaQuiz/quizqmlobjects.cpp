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


#include "quizqmlobjects.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/localdata.h"


void QMLObjects::searchObjects()
{
#if defined(TRACE_THREADS)
    qDebug() << "QMLObjects::searchObjects()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    if(nullptr == m_netAPI)
    {
        NetAPI::createInstance(m_root);
        m_netAPI = NetAPI::getInstance();
    }
    if(nullptr == m_dataAPI && nullptr != m_netAPI)
    {
        LocalDataAPI::createInstance(m_root, m_netAPI);
        m_dataAPI = LocalDataAPI::getInstance();
    }
//    IListModel *m = nullptr;
//    if(nullptr == m_mastactvaAPI)
//    {
//        MastactvaAPI *m1 = m_root->findChild<MastactvaAPI *>(g_mastactvaAPI);
//        m_mastactvaAPI = m1;
//    }
    for(IListModel *m : qAsConst(m_models))
    {
        if(nullptr == m) { continue; }
        m->initResponse();
    }
}

QMLObjectsBase &QMLObjectsBase::getInstance()
{
    static QMLObjects instance;
    return instance;
}

void QMLObjects::setInitialized()
{
//    if(nullptr == m_mastactvaAPI) { return; }
//    m_mastactvaAPI->setInitialized();
}

void QMLObjects::modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
{
    Q_UNUSED(errorCode_);
    Q_UNUSED(errorCodeStr_);
    Q_UNUSED(reply_);

//    if(nullptr == m_mastactvaAPI) { return; }
//    m_mastactvaAPI->showModelError(errorCode_, errorCodeStr_, reply_.toJson(QJsonDocument::Indented));
}
