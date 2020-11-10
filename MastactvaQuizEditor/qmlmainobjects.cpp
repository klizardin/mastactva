#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"
#include "mastactvaapi.h"


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
    if(nullptr == m_mastactavaAPI)
    {
        searchQMLObjects();
    }
    return m_mastactavaAPI;
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
    if(nullptr == m_mastactavaAPI)
    {
        m_mastactavaAPI = m_root->findChild<MastactvaAPI *>("MastactvaAPI");
    }
}

void QMLMainObjects::setRootQMLObject(QObject* root_)
{
    m_root = root_;
}
