#include "qmlobjects.h"
#include <QObject>
#include "imagesource.h"
#include "image.h"
#include "quizuser.h"
#include "IModel.h"
#include "gallery.h"
#include "netapi.h"
#include "Model.h"


static QString g_netAPIQMLName = "NetAPI";
static QString g_quizUserModel = "QuizUserModel";
static QString g_galleryModel = "GalleryModel";


void QMLObjects::setRoot(QObject *root_)
{
    m_root = root_;
    searchObjects();
    NetAPI *netAPI = getNetAPI();
    if(nullptr != netAPI)
    {
        netAPI->setInitialized();
    }
}

IListModel *QMLObjects::getListModel(const QString &layoutName_)
{
    IListModel *model = findListModel(layoutName_);
    if(nullptr != model) { return model; }
    searchObjects();
    return findListModel(layoutName_);
}

IListModel *QMLObjects::findListModel(const QString &layoutName_) const
{
    const auto fit = std::find_if(std::begin(m_models), std::end(m_models),
                                  [&layoutName_](IListModel *model_)->bool
    {
        return nullptr != model_ && model_->getQMLLayoutName() == layoutName_;
    });
    if(std::end(m_models) == fit) { return nullptr; }
    return *fit;
}

NetAPI *QMLObjects::getNetAPI()
{
    if(nullptr == m_netAPI) { searchObjects(); }
    return m_netAPI;
}

void QMLObjects::searchObjects()
{
    if(nullptr == m_netAPI) { m_netAPI = m_root->findChild<NetAPI *>(g_netAPIQMLName); }
    IListModel *m = nullptr;
    m = findListModel(g_quizUserModel);
    if(nullptr == m)
    {
        QuizUserModel *m1 = m_root->findChild<QuizUserModel *>(g_quizUserModel);
        if(nullptr != m1) m_models.push_back(static_cast<IListModel *>(m1));
    }
    m = findListModel(g_galleryModel);
    if(nullptr == m)
    {
        GalleryModel *m1 = m_root->findChild<GalleryModel *>(g_galleryModel);
        if(nullptr != m1) m_models.push_back(static_cast<IListModel *>(m1));
    }
}

QMLObjects &QMLObjects::getInstance()
{
    static QMLObjects instance;
    return instance;
}

