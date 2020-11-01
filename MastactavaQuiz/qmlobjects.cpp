#include "qmlobjects.h"
#include "netapi.h"
#include "Model.h"
#include <QObject>


static QString g_netAPIQMLName = "NetAPI";


void QMLObjects::setRoot(QObject *root_)
{
    m_root = root_;
}

IListModel *QMLObjects::getListModel(const QString &layoutName_)
{
    IListModel *model = findListModel(layoutName_);
    if(nullptr != model) { return model; }
    searchObjects(layoutName_);
    return findListModel(layoutName_);
}

IListModel *QMLObjects::findListModel(const QString &layoutName_) const
{
    const auto fit = std::find_if(std::begin(m_models), std::end(m_models),
                                  [&layoutName_](IListModel *model_)->bool
    {
        return nullptr != model_ && model_->getLayoutName() == layoutName_;
    });
    if(std::end(m_models) == fit) { return nullptr; }
    return *fit;
}

NetAPI *QMLObjects::getNetAPI()
{
    if(nullptr == m_netAPI) { searchObjects(); }
    return m_netAPI;
}

void QMLObjects::searchObjects(const QString &layoutName_ /*= QString()*/)
{
    if(nullptr == m_netAPI) { m_netAPI = m_root->findChild<NetAPI *>(g_netAPIQMLName); }
    if(!layoutName_.isEmpty())
    {
        IListModel *model = m_root->findChild<IListModel *>(layoutName_);
        if(nullptr != model) { m_models.push_back(model); }
    }
}

QMLObjects &QMLObjects::getInstance()
{
    static QMLObjects instance;
    return instance;
}

