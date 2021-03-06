#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"
#include "mastactvaapi.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/netapi.h"


QMLMainObjects::QMLMainObjects(QObject *parent_) : QObject(parent_)
{
}

QMLMainObjects *QMLMainObjects::g_singelton = nullptr;

void QMLMainObjects::createSingelton(QObject *parent_ /*= nullptr*/)
{
    Q_ASSERT(nullptr == g_singelton);
    if(!(nullptr == g_singelton))
    {
        return;
    }
    g_singelton = new QMLMainObjects(parent_);
}

QMLMainObjects *QMLMainObjects::getSingelton()
{
    return g_singelton;
}

GalleryEditViewModel *QMLMainObjects::getGalleryViewModel()
{
    if(nullptr == m_galleryViewModel)
    {
        searchQMLObjects();
    }
    return m_galleryViewModel;
}

GalleryImagesModel *QMLMainObjects::getGalleryAllImagesModel()
{
    if(nullptr == m_galleryAllImagesModel)
    {
        searchQMLObjects();
    }
    return m_galleryAllImagesModel;
}

DescriptionModel *QMLMainObjects::getImageOfGalleryDescriptionModel()
{
    if(nullptr == m_imageOfGalleryDescriptionModel)
    {
        searchQMLObjects();
    }
    return m_imageOfGalleryDescriptionModel;
}

QuestionAnswersModel *QMLMainObjects::getQuestionAnswersModel()
{
    if(nullptr == m_questionAnswersModel)
    {
        searchQMLObjects();
    }
    return m_questionAnswersModel;
}

MastactvaAPI *QMLMainObjects::getMastactvaAPI()
{
    if(nullptr == m_mastactvaAPI)
    {
        searchQMLObjects();
    }
    return m_mastactvaAPI;
}

QGuiApplication *QMLMainObjects::getApp()
{
    return m_app;
}

void QMLMainObjects::setApp(QGuiApplication *app_)
{
    m_app = app_;
}

void QMLMainObjects::searchQMLObjects()
{
    if(nullptr == m_galleryViewModel)
    {
        m_galleryViewModel = m_root->findChild<GalleryEditViewModel *>("GalleryModel");
    }
    if(nullptr == m_galleryAllImagesModel)
    {
        m_galleryAllImagesModel = m_root->findChild<GalleryImagesModel *>("AllGalleryImagesModel");
    }
    if(nullptr == m_imageOfGalleryDescriptionModel)
    {
        m_imageOfGalleryDescriptionModel = m_root->findChild<DescriptionModel *>("ImageOfGalleryDescriptionModel");
    }
    if(nullptr == m_questionAnswersModel)
    {
        m_questionAnswersModel = m_root->findChild<QuestionAnswersModel *>("ImageOfGalleryQuestionAnswersModel");
    }
    if(nullptr == m_mastactvaAPI)
    {
        m_mastactvaAPI = m_root->findChild<MastactvaAPI *>("MastactvaAPI");
    }
}

void QMLMainObjects::setRootQMLObject(QObject* root_)
{
    m_root = root_;
}


#include "../MastactvaModels/effect.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefact.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaModels/easingtype.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/image.h"
#include "../MastactvaModels/gallerystatistics.h"
#include "../MastactvaModels/questionanswer.h"
#include "../MastactvaBase/names.h"


void QMLObjects::setInitialized()
{
    searchObjects();
    if(nullptr == m_mastactvaAPI) { return; }
    m_mastactvaAPI->setInitialized();
}

void QMLObjects::modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
{
    Q_UNUSED(errorCode_);
    searchObjects();
    if(nullptr == m_mastactvaAPI) { return; }
    m_mastactvaAPI->showErrorMessage(errorCodeStr_, reply_.toJson(QJsonDocument::Indented));
}

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
    if(nullptr == m_mastactvaAPI)
    {
        m_mastactvaAPI = m_root->findChild<MastactvaAPI *>("MastactvaAPI");
    }
    if(m_localDataAPIViews.isEmpty())
    {
        m_localDataAPIViews.push_back(new GalleryStatisticsModelView(m_root));
        m_localDataAPIViews.push_back(new UserQuestionAnswerModelView(m_root));
    }
    IListModel *m = nullptr;
    m = findListModel(g_effectModel);
    if(nullptr == m)
    {
        EffectModel *m1 = m_root->findChild<EffectModel *>(g_effectModel);
        registerModel(g_effectModel, m1);
    }
    m = findListModel(g_artefactTypeModel);
    if(nullptr == m)
    {
        ArtefactTypeModel *m1 = m_root->findChild<ArtefactTypeModel *>(g_artefactTypeModel);
        registerModel(g_artefactTypeModel, m1);
    }
    m = findListModel(g_artefactModel);
    if(nullptr == m)
    {
        ArtefactModel *m1 = m_root->findChild<ArtefactModel *>(g_artefactModel);
        registerModel(g_artefactModel, m1);
    }
    m = findListModel(g_artefactArgTypeModel);
    if(nullptr == m)
    {
        ArtefactArgTypeModel *m1 = m_root->findChild<ArtefactArgTypeModel *>(g_artefactArgTypeModel);
        registerModel(g_artefactArgTypeModel, m1);
    }
    m = findListModel(g_artefactArgStorageModel);
    if(nullptr == m)
    {
        ArtefactArgStorageModel *m1 = m_root->findChild<ArtefactArgStorageModel *>(g_artefactArgStorageModel);
        registerModel(g_artefactArgStorageModel, m1);
    }
    m = findListModel(g_easingTypeModel);
    if(nullptr == m)
    {
        EasingTypeModel *m1 = m_root->findChild<EasingTypeModel *>(g_easingTypeModel);
        registerModel(g_easingTypeModel, m1);
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
