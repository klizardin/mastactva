#include "questionanswermodelview.h"


QuestionAnswerModelView::QuestionAnswerModelView(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
    m_userQuestionAnswerModel = new UserQuestionAnswerModel(parent_);
    m_userQuestionAnswerModel->initResponse();
    m_userQuestionAnswerModel->setCurrentRefImpl("");
    m_userQuestionAnswerModel->setLayoutQMLNameImpl("LocalData_UserQuestionAnswerModel");
    m_userQuestionAnswerModel->setLayoutIdFieldImpl("id");
    m_userQuestionAnswerModel->registerListModel();
    m_userQuestionAnswerModel->setAutoCreateChildrenModelsImpl(false);
    m_userQuestionAnswerModel->setReadonlyImpl(false);

    m_galleryStatisticsModel = new GalleryStatisticsModel(parent_);
    m_galleryStatisticsModel->initResponse();
    m_galleryStatisticsModel->setCurrentRefImpl("");
    m_galleryStatisticsModel->setLayoutQMLNameImpl("LocalData_GalleryStatisticsModel");
    m_galleryStatisticsModel->setLayoutIdFieldImpl("id");
    m_galleryStatisticsModel->registerListModel();
    m_galleryStatisticsModel->setAutoCreateChildrenModelsImpl(false);
    m_galleryStatisticsModel->setReadonlyImpl(false);

    m_imagePointToQuestionModel = new ImagePointToQuestionModel(parent_);
    m_imagePointToQuestionModel->initResponse();
    m_imagePointToQuestionModel->setCurrentRefImpl("image_point");
    m_imagePointToQuestionModel->setRefValueImpl(QVariant::fromValue(-1));
    m_imagePointToQuestionModel->setLayoutQMLNameImpl("LocalData_ImagePointToQuestionModel");
    m_imagePointToQuestionModel->setLayoutIdFieldImpl("id");
    m_imagePointToQuestionModel->registerListModel();
    m_imagePointToQuestionModel->setAutoCreateChildrenModelsImpl(false);
    m_imagePointToQuestionModel->setReadonlyImpl(true);

    m_imagePointModel = new ImagePointModel(parent_);
    m_imagePointModel->initResponse();
    m_imagePointModel->setCurrentRefImpl("image");
    m_imagePointModel->setRefValueImpl(QVariant::fromValue(-1));
    m_imagePointModel->setLayoutQMLNameImpl("LocalData_ImagePointModel");
    m_imagePointModel->setLayoutIdFieldImpl("id");
    m_imagePointModel->registerListModel();
    m_imagePointModel->setAutoCreateChildrenModelsImpl(false);
    m_imagePointModel->setReadonlyImpl(true);

    m_imageModel = new ImageModel(parent_);
    m_imageModel->initResponse();
    m_imageModel->setCurrentRefImpl("gallery");
    m_imageModel->addModelParamImpl("use_in_gallery_view", "1");
    m_imageModel->setRefValueImpl(QVariant::fromValue(-1));
    m_imageModel->setLayoutQMLNameImpl("LocalData_ImageModel");
    m_imageModel->registerListModel();
    m_imageModel->setAutoCreateChildrenModelsImpl(false);
}

QuestionAnswerModelView::~QuestionAnswerModelView()
{
    delete m_userQuestionAnswerModel;
    m_userQuestionAnswerModel = nullptr;
    delete m_galleryStatisticsModel;
    m_galleryStatisticsModel = nullptr;
    delete m_imagePointToQuestionModel;
    m_imagePointToQuestionModel = nullptr;
    delete m_imagePointModel;
    m_imagePointModel = nullptr;
    delete m_imageModel;
    m_imageModel = nullptr;
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
    if(r_->getCurrentRef().isEmpty()) { return false; }
    return true;
}

bool QuestionAnswerModelView::getListImpl(DBRequestInfo *r_)
{
    Q_UNUSED(r_);
    return false;
}

bool QuestionAnswerModelView::addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    Q_ASSERT(nullptr != r_ && r_->getAPIName() == g_cachAPI);
    LocalDBRequest *r = static_cast<LocalDBRequest *>(r_);
    r->setItemAppId(appId_);

    UserQuestionAnswer *userQuestionAnswer = m_userQuestionAnswerModel->createDataItemImpl();
    getDataLayout<UserQuestionAnswer>().setJsonValues(userQuestionAnswer, values_);

    m_requests.push_back({r_, userQuestionAnswer});

    loadStatisticsForGallery();
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

void QuestionAnswerModelView::loadStatisticsForGallery()
{
    const UserQuestionAnswer *userQuestionAnswer = m_requests.front().second;
    Q_ASSERT(nullptr != userQuestionAnswer);

    QObject::connect(m_imagePointToQuestionModel, SIGNAL(listReloaded()), this, SLOT(imagePointToQuestionModelListReloaded()));

    m_imagePointToQuestionModel->clearListLoaded();
    m_imagePointToQuestionModel->loadListImpl(QString(),
                QHash<QString, QVariant>(
                {{
                    QString(g_procedureExtraFieldName),
                     QVariant::fromValue(QHash<QString, QVariant>(
                     {
                         {QString(g_procedureConditionName), QVariant::fromValue(QString("question=:question"))},
                         {QString(g_procedureFilterConditionsName),
                            QVariant::fromValue(
                                QList<QVariant>(
                                    {
                                        QVariant::fromValue(QString("")),
                                    }
                                )
                            )
                         },
                         {QString(g_procedureArguments),
                          QVariant::fromValue(
                            QHash<QString, QVariant>(
                            {
                                {QString(":question"),
                                 QVariant::fromValue(userQuestionAnswer->questionId())},
                            })
                          )
                         }
                     }))
                 },}));
}

void QuestionAnswerModelView::imagePointToQuestionModelListReloaded()
{
    QObject::disconnect(m_imagePointToQuestionModel, SIGNAL(listReloaded()), this, SLOT(imagePointToQuestionModelListReloaded()));
    m_imagePointToQuestion = m_imagePointToQuestionModel->getCurrentDataItem();
    if(nullptr == m_imagePointToQuestion)
    {
        return;
    }

    QObject::connect(m_imagePointModel, SIGNAL(listReloaded()), this, SLOT(imagePointModelListReloaded()));
    m_imagePointModel->clearListLoaded();
    m_imagePointModel->loadListImpl(QString(),
                QHash<QString, QVariant>(
                {{
                    QString(g_procedureExtraFieldName),
                     QVariant::fromValue(QHash<QString, QVariant>(
                     {
                         {QString(g_procedureConditionName), QVariant::fromValue(QString("id=:id"))},
                         {QString(g_procedureFilterConditionsName),
                            QVariant::fromValue(
                                QList<QVariant>(
                                    {
                                        QVariant::fromValue(QString("")),
                                    }
                                )
                            )
                         },
                         {QString(g_procedureArguments),
                          QVariant::fromValue(
                            QHash<QString, QVariant>(
                            {
                                {QString(":id"),
                                 QVariant::fromValue(m_imagePointToQuestion->imagePointId())},
                            })
                          )
                         }
                     }))
                 },}));
}

void QuestionAnswerModelView::imagePointModelListReloaded()
{
    QObject::disconnect(m_imagePointModel, SIGNAL(listReloaded()), this, SLOT(imagePointModelListReloaded()));
    m_imagePoint = m_imagePointModel->getCurrentDataItem();
    if(nullptr == m_imagePoint)
    {
        return;
    }


}
