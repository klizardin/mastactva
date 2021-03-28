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
