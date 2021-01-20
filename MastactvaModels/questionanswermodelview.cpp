#include "questionanswermodelview.h"
#include <QHash>
#include <QString>
#include <QVariant>
#include <QList>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaBase/defines.h"


QuestionAnswerModelView::QuestionAnswerModelView(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "QuestionAnswerModelView::QuestionAnswerModelView()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    m_userQuestionAnswerModel = new UserQuestionAnswerModel(this);
    m_userQuestionAnswerModel->initResponse();
    m_userQuestionAnswerModel->setCurrentRefImpl("");
    m_userQuestionAnswerModel->setLayoutQMLNameImpl("LocalData_UserQuestionAnswerModel");
    m_userQuestionAnswerModel->setLayoutIdFieldImpl("id");
    m_userQuestionAnswerModel->registerListModel();
    m_userQuestionAnswerModel->setAutoCreateChildrenModelsImpl(false);
    m_userQuestionAnswerModel->setReadonlyImpl(false);

    GalleryModel *galleryModel = static_cast<GalleryModel *>(
                QMLObjectsBase::getInstance().getListModel(g_galleryModel)
                );

    m_galleryStatisticsModel = new GalleryStatisticsModel(this);
    m_galleryStatisticsModel->initResponse();
    m_galleryStatisticsModel->setLayoutRefImpl("gallery", galleryModel->getQMLLayoutName(), "id", false);
    m_galleryStatisticsModel->setCurrentRefImpl("gallery");
    m_galleryStatisticsModel->setLayoutQMLNameImpl("LocalData_GalleryStatisticsModel");
    m_galleryStatisticsModel->setLayoutIdFieldImpl("id");
    m_galleryStatisticsModel->registerListModel();
    m_galleryStatisticsModel->setAutoCreateChildrenModelsImpl(false);
    m_galleryStatisticsModel->setReadonlyImpl(false);
}

QuestionAnswerModelView::~QuestionAnswerModelView()
{
    delete m_userQuestionAnswerModel;
    m_userQuestionAnswerModel = nullptr;
    delete m_galleryStatisticsModel;
    m_galleryStatisticsModel = nullptr;
}

bool QuestionAnswerModelView::canProcess(const DBRequestInfo *r_) const
{
    if(nullptr == r_) { return false; }
    const QString tableName = DBRequestInfo::namingConversion(
                getDataLayout<typename UserQuestionAnswerModel::DataType>().getLayoutJsonName()
                );
    if(r_->getTableName() != tableName) { return false; }
    const QString requestNameAddItem = RequestData::addItemRequestName<typename UserQuestionAnswerModel::DataType>();
    if(r_->getDBRequestName() != requestNameAddItem) { return false; }
    if(r_->getAPIName() != g_cachAPI) { return false; }
    return true;
}

bool QuestionAnswerModelView::getListImpl(DBRequestInfo *r_)
{
    Q_UNUSED(r_);
    return false;
}

bool QuestionAnswerModelView::addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    if(nullptr == r_ || r_->getAPIName() != g_cachAPI) { return false; }
    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemAppId(appId_);

    UserQuestionAnswer *userQuestionAnswer = m_userQuestionAnswerModel->createDataItemImpl();
    getDataLayout<UserQuestionAnswer>().setJsonValues(userQuestionAnswer, values_);
    userQuestionAnswer->setUserId(g_userId);

    m_requests.push_back({r_, userQuestionAnswer});

    addUserQuestionAnswer();
    return true;
}

bool QuestionAnswerModelView::setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool QuestionAnswerModelView::delItemImpl(const QVariant &id_, DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(r_);
    return false;
}

void QuestionAnswerModelView::addUserQuestionAnswer()
{
    if(m_requests.isEmpty())
    {
        return;
    }

    Q_ASSERT(nullptr != m_userQuestionAnswerModel);

    QObject::connect(m_galleryStatisticsModel, SIGNAL(listReloaded()), this, SLOT(galleryStatisticsModelListReloaded()));
    m_galleryStatisticsModel->clearListLoaded();
    m_galleryStatisticsModel->loadList();
}

void QuestionAnswerModelView::error()
{
    if(m_requests.isEmpty())
    {
        return;
    }

    DBRequestInfo *request = m_requests.front().first;
    if(nullptr != request && request->getAPIName() == g_cachAPI)
    {
        LocalDBRequest *r = static_cast<LocalDBRequest *>(request);

        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj.insert(QString(g_errorDetailTag), QJsonValue("Internal error"));
        jsonArray.push_back(jsonObj);
        r->addJsonResult(QJsonDocument(jsonArray));
        r->setError(true);
        r->setProcessed(true);
    }

    m_requests.pop_front();
    addUserQuestionAnswer();
}

void QuestionAnswerModelView::galleryStatisticsModelListReloaded()
{
    Q_ASSERT(nullptr != m_userQuestionAnswerModel);
    Q_ASSERT(nullptr != m_galleryStatisticsModel);

    QObject::disconnect(m_galleryStatisticsModel, SIGNAL(listReloaded()), this, SLOT(galleryStatisticsModelListReloaded()));

    if(m_requests.isEmpty())
    {
        error();
        return;
    }

    UserQuestionAnswer *userQuestionAnswer = m_requests.front().second;
    if(nullptr == userQuestionAnswer)
    {
        error();
        return;
    }

    m_galleryStatistics = m_galleryStatisticsModel->getCurrentDataItem();
    if(nullptr == m_galleryStatistics)
    {
        m_galleryStatistics = m_galleryStatisticsModel->createDataItemImpl();

        GalleryModel *galleryModel = static_cast<GalleryModel *>(
                    QMLObjectsBase::getInstance().getListModel(g_galleryModel)
                    );
        Gallery *gallery = galleryModel->getCurrentDataItem();
        if(nullptr == gallery)
        {
            error();
            return;
        }

        m_galleryStatistics->setGalleryId(nullptr != gallery ? gallery->id() : -1 );
        m_galleryStatistics->setUserId(g_userId);
        m_galleryStatistics->setPoints(userQuestionAnswer->points());

        QObject::connect(m_galleryStatisticsModel, SIGNAL(itemAdded()), this, SLOT(galleryStatisticsModelItemAdded()));
        m_galleryStatisticsModel->addDataItemImpl(m_galleryStatistics);
    }
    else
    {

        QObject::connect(m_userQuestionAnswerModel, SIGNAL(listReloaded()), this, SLOT(userQuestionAnswerModelListReloaded()));
        m_userQuestionAnswerModel->clearListLoaded();
        m_userQuestionAnswerModel->loadListImpl(
            QString(),
            QHash<QString, QVariant>(
            {
                 {g_procedureExtraFieldName,
                 QHash<QString, QVariant>
                 ({
                    {g_procedureConditionName, QVariant::fromValue(QString("question=:question"))},
                    {g_procedureOrderByName, QVariant::fromValue(QString("t DESC"))},
                    {g_procedureLimitName, QVariant::fromValue(1)},
                    {g_procedureArguments,
                    QVariant::fromValue(QHash<QString, QVariant>
                    ({
                      {QString(":question"), QVariant::fromValue(userQuestionAnswer->questionId())},
                    }))},
                 })
                },
            }));
    }
}

void QuestionAnswerModelView::userQuestionAnswerModelListReloaded()
{
    Q_ASSERT(nullptr != m_userQuestionAnswerModel);
    QObject::disconnect(m_userQuestionAnswerModel, SIGNAL(listReloaded()), this, SLOT(userQuestionAnswerModelListReloaded()));

    if(m_requests.isEmpty())
    {
        error();
        return;
    }

    UserQuestionAnswer *userQuestionAnswer = m_requests.front().second;
    if(nullptr == userQuestionAnswer)
    {
        error();
        return;
    }

    UserQuestionAnswer *oldUserQuestionAnswer = m_userQuestionAnswerModel->getCurrentDataItem();
    if(nullptr == oldUserQuestionAnswer)
    {
        m_galleryStatistics->setPoints(m_galleryStatistics->points() + userQuestionAnswer->points());

    }
    else
    {
        m_galleryStatistics->setPoints(m_galleryStatistics->points() - oldUserQuestionAnswer->points() + userQuestionAnswer->points());
    }

    QObject::connect(m_galleryStatisticsModel, SIGNAL(itemSet()), this, SLOT(galleryStatisticsModelItemSet()));
    const int index = m_galleryStatisticsModel->indexOfDataItemImpl(m_galleryStatistics);
    m_galleryStatisticsModel->setDataItemImpl(index, m_galleryStatistics);
}

void QuestionAnswerModelView::galleryStatisticsModelItemAdded()
{
    addUserQuestionAnswerItem();
}

void QuestionAnswerModelView::galleryStatisticsModelItemSet()
{
    addUserQuestionAnswerItem();
}

void QuestionAnswerModelView::addUserQuestionAnswerItem()
{
    if(m_requests.isEmpty())
    {
        error();
        return;
    }

    UserQuestionAnswer *userQuestionAnswer = m_requests.front().second;
    if(nullptr == userQuestionAnswer)
    {
        error();
        return;
    }

    QHash<QString, QVariant> values;
    getDataLayout<UserQuestionAnswer>().getJsonValues(userQuestionAnswer, values);

    DBRequestInfo *request = m_requests.front().first;
    if(nullptr == request || request->getAPIName() != g_cachAPI)
    {
        error();
        return;
    }

    LocalDBRequest *r = static_cast<LocalDBRequest *>(request);
    request->getDefaultAPI()->addItemImpl(r->getItemAppId(), values, request);

    m_requests.pop_front();
    addUserQuestionAnswer();
}
