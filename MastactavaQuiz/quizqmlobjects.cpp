#include "quizqmlobjects.h"
#include "image.h"
#include "quizuser.h"
#include "gallery.h"


static QString g_netAPIQMLName = "NetAPI";
static QString g_quizUserModel = "QuizUserModel";
static QString g_galleryModel = "GalleryModel";
static QString g_allImagesOfGalleryModel = "AllImagesOfGallery";


void QMLObjects::searchObjects()
{
    if(nullptr == m_netAPI) { m_netAPI = m_root->findChild<NetAPI *>(g_netAPIQMLName); }
    IListModel *m = nullptr;
    m = findListModel(g_quizUserModel);
    if(nullptr == m)
    {
        QuizUserModel *m1 = m_root->findChild<QuizUserModel *>(g_quizUserModel);
        registerModel(g_quizUserModel, m1);
    }
    m = findListModel(g_galleryModel);
    if(nullptr == m)
    {
        GalleryModel *m1 = m_root->findChild<GalleryModel *>(g_galleryModel);
        registerModel(g_galleryModel, m1);
    }
    m = findListModel(g_allImagesOfGalleryModel);
    if(nullptr == m)
    {
        ImageModel *m1 = m_root->findChild<ImageModel *>(g_allImagesOfGalleryModel);
        registerModel(g_allImagesOfGalleryModel, m1);
    }
    for(IListModel *m : m_models)
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


