#include "quizqmlobjects.h"
#include "mastactvaapi.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaModels/easingtype.h"
#include "../MastactvaModels/image.h"
#include "../MastactvaModels/quizuser.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/gallerystatistics.h"
#include "../MastactvaModels/questionanswer.h"
#include "../MastactvaModels/gallerymodelview.h"
#include "../MastactvaModels/questionanswermodelview.h"
#include "../MastactvaModels/effectobjects.h"
#include "../MastactvaModels/objectinfo.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


void QMLObjects::searchObjects()
{
#if defined(TRACE_THREADS)
    qDebug() << "QMLObjects::searchObjects()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    if(!m_netAPI)
    {
        NetAPI::createInstance(m_root);
        m_netAPI = NetAPI::getInstance();
    }
    if(!m_dataAPI && m_netAPI)
    {
        LocalDataAPI::createInstance(m_root, m_netAPI);
        m_dataAPI = LocalDataAPI::getInstance();
    }
    if(m_localDataAPIViews.isEmpty())
    {
        m_localDataAPIViews.push_back(new GalleryStatisticsModelView(m_root));
        m_localDataAPIViews.push_back(new UserQuestionAnswerModelView(m_root));
        m_localDataAPIViews.push_back(new GalleryModelView(m_root));
        m_localDataAPIViews.push_back(new QuestionAnswerModelView(m_root));
    }
    if(!m_mastactvaAPI)
    {
        MastactvaAPI *m1 = m_root->findChild<MastactvaAPI *>(g_mastactvaAPI);
        m_mastactvaAPI = m1;
    }
    registerModels(
                modelArg<QuizUserModel>(g_quizUserModel),
                modelArg<GalleryModel>(g_galleryModel),
                modelArg<ImageModel>(g_allImagesOfGalleryModel),
                modelArg<ArtefactTypeModel>(g_artefactTypeModel),
                modelArg<ArtefactArgTypeModel>(g_artefactArgTypeModel),
                modelArg<ArtefactArgStorageModel>(g_artefactArgStorageModel),
                modelArg<EasingTypeModel>(g_easingTypeModel)
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

void QMLObjects::setInitialized()
{
    if(!m_mastactvaAPI) { return; }
    m_mastactvaAPI->setInitialized();
}

void QMLObjects::modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_)
{
    if(!m_mastactvaAPI) { return; }
    m_mastactvaAPI->showModelError(errorCode_, errorCodeStr_, reply_.toJson(QJsonDocument::Indented));
}
