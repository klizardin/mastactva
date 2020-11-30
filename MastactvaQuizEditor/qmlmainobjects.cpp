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


#include "effect.h"
#include "shadertype.h"


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

static const QString g_effectModel = "EffectModel";
static const QString g_shaderTypeModel = "ShaderTypeModel";


void QMLObjects::searchObjects()
{
    if(nullptr == m_netAPI)
    {
        NetAPI::createInstance(m_root);
        m_netAPI = NetAPI::getInstance();
    }
    if(nullptr == m_mastactvaAPI)
    {
        m_mastactvaAPI = m_root->findChild<MastactvaAPI *>("MastactvaAPI");
    }
    IListModel *m = nullptr;
    m = findListModel(g_effectModel);
    if(nullptr == m)
    {
        EffectModel *m1 = m_root->findChild<EffectModel *>(g_effectModel);
        registerModel(g_effectModel, m1);
    }
    m = findListModel(g_shaderTypeModel);
    if(nullptr == m)
    {
        ShaderTypeModel *m1 = m_root->findChild<ShaderTypeModel *>(g_shaderTypeModel);
        registerModel(g_shaderTypeModel, m1);
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
