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

#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"
#include "mastactvaapi.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/netapi.h"


QMLMainObjects::QMLMainObjects(QObject *parent_) : QObject(parent_)
{
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "QMLMainObjects::QMLMainObjects()" << this;
#endif
}

QMLMainObjects::~QMLMainObjects()
{
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "QMLMainObjects::~QMLMainObjects()" << this;
#endif
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


#include "../MastactvaBase/dualmodelconfig.h"
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
#include "../MastactvaModels/objectinfo.h"
#include "../MastactvaModels/effectobjects.h"
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
    if(!DualModelConfigBase::instance().isInit())
    {
        DualModelConfigBase::instance().init(m_root, m_netAPI);
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
    registerModels(
                modelArg<EffectModel>(g_effectModel),
                modelArg<ArtefactModel>(g_artefactModel),
                modelArg<ArtefactTypeModel>(g_artefactTypeModel),
                modelArg<ArtefactArgTypeModel>(g_artefactArgTypeModel),
                modelArg<ArtefactArgStorageModel>(g_artefactArgStorageModel),
                modelArg<EasingTypeModel>(g_easingTypeModel),
                modelArg<GalleryModel>(g_galleryModel),
                modelArg<ImageModel>(g_allImagesOfGalleryModel),
                modelArg<EffectObjectsModel>(g_effectObjectsModel),
                modelArg<ObjectInfoModel>(g_objectInfoModel)
                );
    postRegisterModel();
}

QMLObjectsBase *QMLObjectsBase::getInstancePtr()
{
    static std::unique_ptr<QMLObjects> instance = std::make_unique<QMLObjects>();
    return instance.get();
}

QMLObjectsBase &QMLObjectsBase::getInstance()
{
    return *getInstancePtr();
}

bool QMLObjectsBase::hasInstance()
{
    return getInstancePtr();
}
