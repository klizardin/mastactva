/*
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

#include "localdataset.h"
#include <math.h>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/image.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/easingtype.h"
#include "../MastactvaModels/userstep.h"
#include "../MastactvaModels/quizuser.h"


static const char *g_defaultSavePath = "./LocalData/";
static const int g_downloadStepsCount = 5;


LocalDataSet::LocalDataSet(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
    m_savePath = g_defaultSavePath;
    m_galleryIndex = -1;
    c_downloadStepsCount = g_downloadStepsCount;
}

LocalDataSet::~LocalDataSet()
{
    free();
}

void LocalDataSet::create()
{
    // TODO: GalleryStatisticsModel and UserQuestionAnswerModel
    // make view to not load data and create model from view
    // views should create DB tables for itselfs

    free();
    m_galleryModel = new GalleryModel(this);
    m_galleryModel->initResponse();
    m_galleryModel->setCurrentRef("");
    m_galleryModel->setLayoutQMLName("LocalData_GalleryModel");
    m_galleryModel->setLayoutIdFieldImpl("id");
    m_galleryModel->registerListModel();
    m_galleryModel->setAutoCreateChildrenModels(true);

    m_imageModel = new ImageModel(this);
    m_imageModel->initResponse();
    m_imageModel->setLayoutRefImpl("gallery", m_galleryModel->getQMLLayoutName(), "id", false);
    m_imageModel->setCurrentRef("gallery");
    m_imageModel->setLayoutQMLName(
                m_galleryModel->getQMLLayoutName() +
                QString("_Gallery_NNN_") +
                QString("_GalleryImageModel_")
                );
    m_imageModel->addModelParam("use_in_gallery_view", "0");
    m_imageModel->setLayoutIdFieldImpl("id");
    m_imageModel->registerListModel();
    m_imageModel->setAutoCreateChildrenModels(true);           // TODO: find way to upload images of gallery when it is required

    m_artefactTypeModel = new ArtefactTypeModel(this);
    m_artefactTypeModel->initResponse();
    m_artefactTypeModel->setCurrentRef("");
    m_artefactTypeModel->setLayoutQMLName("LocalData_ArtefactTypeModel");
    m_artefactTypeModel->setLayoutIdFieldImpl("id");
    m_artefactTypeModel->registerListModel();
    m_artefactTypeModel->setAutoCreateChildrenModels(true);

    m_artefactArgTypeModel = new ArtefactArgTypeModel(this);
    m_artefactArgTypeModel->initResponse();
    m_artefactArgTypeModel->setCurrentRef("");
    m_artefactArgTypeModel->setLayoutQMLName("LocalData_ArtefactArgTypeModel");
    m_artefactArgTypeModel->setLayoutIdFieldImpl("id");
    m_artefactArgTypeModel->registerListModel();
    m_artefactArgTypeModel->setAutoCreateChildrenModels(true);

    m_easingTypeModel = new EasingTypeModel(this);
    m_easingTypeModel->initResponse();
    m_easingTypeModel->setCurrentRef("");
    m_easingTypeModel->setLayoutQMLName("LocalData_EasingTypeModel");
    m_easingTypeModel->setLayoutIdFieldImpl("id");
    m_easingTypeModel->registerListModel();
    m_easingTypeModel->setAutoCreateChildrenModels(true);

    m_userStepModel = new UserStepModel(this);
    m_userStepModel->initResponse();
    m_userStepModel->setCurrentRef("");
    m_userStepModel->setLayoutQMLName("LocalData_UserStepModel");
    m_userStepModel->setLayoutIdFieldImpl("id");
    m_userStepModel->registerListModel();
    m_userStepModel->setAutoCreateChildrenModels(false);
    m_userStepModel->setStoreAfterSave(false);
    m_userStepModel->setReadonlyImpl(false);

    //m_quizUserModel = new QuizUserModel(this);
    //m_quizUserModel->setEmptyModel();
    //m_quizUserModel->initResponse();
    //m_quizUserModel->setCurrentRef("");
    //m_quizUserModel->setLayoutQMLName("LocalData_QuizUserModel");
    //m_quizUserModel->setLayoutIdFieldImpl("deviceid");
    //m_quizUserModel->registerListModel();
    //m_quizUserModel->setAutoCreateChildrenModels(false);
    //m_quizUserModel->setStoreAfterSave(true);
    //m_quizUserModel->setReadonlyImpl(false);
}

void LocalDataSet::free()
{
    m_galleryIndex = -1;
    c_downloadStepsCount = g_downloadStepsCount;

    delete m_galleryModel;
    m_galleryModel = nullptr;
    delete m_imageModel;
    m_imageModel = nullptr;
    delete m_artefactTypeModel;
    m_artefactTypeModel = nullptr;
    delete m_artefactArgTypeModel;
    m_artefactArgTypeModel = nullptr;
    delete m_easingTypeModel;
    m_easingTypeModel = nullptr;
    delete m_userStepModel;
    m_userStepModel = nullptr;
    //delete m_quizUserModel;
    //m_quizUserModel = nullptr;
}

void LocalDataSet::download()
{
    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr == localDataAPI) { return; }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr == sf) { return; }

    localDataAPI->startSave(m_savePath);
    m_serverFilesOldRootDir = sf->getRootDir();
    sf->setRootDir(m_savePath);
    sf->clean(QDateTime::currentDateTime());
    sf->reset();

    create();
    m_step = 0;
    downloadStep();
}

qreal LocalDataSet::stepProgress()
{
    const int i = m_step++;
    const qreal progress = floor((qreal)std::min(i, c_downloadStepsCount) / (qreal)c_downloadStepsCount * 1000.0) / 10.0;
    qDebug() << "progress =" << progress;
    return progress;
}

QString LocalDataSet::savePath() const
{
    return m_savePath;
}

void LocalDataSet::setSavePath(const QString &url_)
{
    QUrl url(url_);
    QFile f(url.toLocalFile());
    QFileInfo fi(f);
    m_savePath = fi.absoluteDir().absolutePath();
}

void LocalDataSet::downloadStep()
{
    if(nullptr != m_galleryModel && !m_galleryModel->isListLoaded())
    {
        QObject::connect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_galleryModel->procedureImpl("all_available", QHash<QString, QVariant>());
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_galleryModel && nullptr != m_imageModel && m_galleryIndex < m_galleryModel->sizeImpl() - 1)
    {
        ++m_galleryIndex;
        m_galleryModel->setCurrentIndex(m_galleryIndex);
        const QVariant appId = nullptr != m_galleryModel->dataItemAtImpl(m_galleryIndex)
                ? QVariant::fromValue(m_galleryModel->dataItemAtImpl(m_galleryIndex)->getAppId())
                : QVariant()
                ;
        m_imageModel->setRefAppId(appId);
        m_imageModel->clearListLoaded();
        m_imageModel->loadList();
        if(0 == m_galleryIndex)
        {
            if(nullptr != m_galleryModel)
            {
                QObject::disconnect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
            }
            c_downloadStepsCount += m_galleryModel->sizeImpl();
            QObject::connect(m_imageModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_artefactTypeModel && !m_artefactTypeModel->isListLoaded())
    {
        if(nullptr != m_imageModel)
        {
            QObject::disconnect(m_imageModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_artefactTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_artefactTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_artefactArgTypeModel && !m_artefactArgTypeModel->isListLoaded())
    {
        if(nullptr != m_artefactTypeModel)
        {
            QObject::disconnect(m_artefactTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_artefactArgTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_artefactArgTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_easingTypeModel && !m_easingTypeModel->isListLoaded())
    {
        if(nullptr != m_artefactArgTypeModel)
        {
            QObject::disconnect(m_artefactArgTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_easingTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_easingTypeModel->loadList();
        emit progress(stepProgress());
        return; // one model at time
    }
    if(nullptr != m_userStepModel && !m_userStepModel->isListLoaded())
    {
        if(nullptr != m_easingTypeModel)
        {
            QObject::disconnect(m_easingTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        }
        QObject::connect(m_userStepModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
        m_userStepModel->procedureImpl("empty_list", QHash<QString, QVariant>());
        emit progress(stepProgress());
        return; // one model at time
    }
    //if(nullptr != m_quizUserModel && !m_quizUserModel->isListLoaded())
    //{
    //    if(nullptr != m_userStepModel)
    //    {
    //        QObject::disconnect(m_userStepModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    //    }
    //    QObject::connect(m_quizUserModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    //    m_quizUserModel->loadList();
    //    emit progress(stepProgress());
    //    return; // one model at time
    //}

    if(nullptr != m_galleryModel)
    {
        QObject::disconnect(m_galleryModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_imageModel)
    {
        QObject::disconnect(m_imageModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_artefactTypeModel)
    {
        QObject::disconnect(m_artefactTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_artefactArgTypeModel)
    {
        QObject::disconnect(m_artefactArgTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_easingTypeModel)
    {
        QObject::disconnect(m_easingTypeModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    if(nullptr != m_userStepModel)
    {
        QObject::disconnect(m_userStepModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    }
    //if(nullptr != m_quizUserModel)
    //{
    //    QObject::disconnect(m_quizUserModel, SIGNAL(listReloaded()), this, SLOT(listReloadedSlot()));
    //}
    //free();

    LocalDataAPI *localDataAPI = QMLObjectsBase::getInstance().getDataAPI();
    if(nullptr != localDataAPI) { localDataAPI->endSave(); }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf) { sf->setRootDir(m_serverFilesOldRootDir); }

    emit progress(stepProgress());
    emit downloaded();
}

void LocalDataSet::listReloadedSlot()
{
    downloadStep();
}
