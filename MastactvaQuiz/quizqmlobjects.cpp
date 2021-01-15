#include "quizqmlobjects.h"
#include "mastactvaapi.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaModels/shadertype.h"
#include "../MastactvaModels/shaderargtype.h"
#include "../MastactvaModels/easingtype.h"
#include "../MastactvaModels/image.h"
#include "../MastactvaModels/quizuser.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/gallerystatistics.h"
#include "../MastactvaModels/questionanswer.h"
#include "../MastactvaModels/gallerymodelview.h"


static const char *g_quizUserModel = "QuizUserModel";
static const char *g_mastactvaAPI = "MastactvaAPI";


void QMLObjects::searchObjects()
{
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
    if(m_localDataAPIViews.isEmpty())
    {
        m_localDataAPIViews.push_back(new GalleryStatisticsModelView());
        m_localDataAPIViews.push_back(new UserQuestionAnswerModelView());
        m_localDataAPIViews.push_back(new GalleryModelView(m_root));
    }
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
    m = findListModel(g_shaderTypeModel);
    if(nullptr == m)
    {
        ShaderTypeModel *m1 = m_root->findChild<ShaderTypeModel *>(g_shaderTypeModel);
        registerModel(g_shaderTypeModel, m1);
    }
    m = findListModel(g_shaderArgTypeModel);
    if(nullptr == m)
    {
        ShaderArgTypeModel *m1 = m_root->findChild<ShaderArgTypeModel *>(g_shaderArgTypeModel);
        registerModel(g_shaderArgTypeModel, m1);
    }
    m = findListModel(g_easingTypeModel);
    if(nullptr == m)
    {
        EasingTypeModel *m1 = m_root->findChild<EasingTypeModel *>(g_easingTypeModel);
        registerModel(g_easingTypeModel, m1);
    }
    if(nullptr == m_mastactvaAPI)
    {
        MastactvaAPI *m1 = m_root->findChild<MastactvaAPI *>(g_mastactvaAPI);
        m_mastactvaAPI = m1;
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

void QMLObjects::setInitialized()
{
    if(nullptr == m_mastactvaAPI) { return; }
    m_mastactvaAPI->setInitialized();
}

void QMLObjects::modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
{
    if(nullptr == m_mastactvaAPI) { return; }
    m_mastactvaAPI->showModelError(errorCode_, errorCodeStr_, reply_.toJson(QJsonDocument::Indented));
}
