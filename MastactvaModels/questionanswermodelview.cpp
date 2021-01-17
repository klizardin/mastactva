#include "questionanswermodelview.h"


QuestionAnswerModelView::QuestionAnswerModelView(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
    m_userQuestionAnswerModel = new UserQuestionAnswerModel(parent_);
    m_userQuestionAnswerModel->initResponse();
    m_userQuestionAnswerModel->setCurrentRef("");
    m_userQuestionAnswerModel->setLayoutQMLName("LocalData_UserQuestionAnswerModel");
    m_userQuestionAnswerModel->setLayoutIdFieldImpl("id");
    m_userQuestionAnswerModel->registerListModel();
    m_userQuestionAnswerModel->setAutoCreateChildrenModels(false);
    m_userQuestionAnswerModel->setReadonlyImpl(false);

    m_galleryStatisticsModel = new GalleryStatisticsModel(parent_);
    m_galleryStatisticsModel->initResponse();
    m_galleryStatisticsModel->setCurrentRef("");
    m_galleryStatisticsModel->setLayoutQMLName("LocalData_GalleryStatisticsModel");
    m_galleryStatisticsModel->setLayoutIdFieldImpl("id");
    m_galleryStatisticsModel->registerListModel();
    m_galleryStatisticsModel->setAutoCreateChildrenModels(false);
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
    //UserQuestionAnswer *newItem = m_userQuestionAnswerModel->createDataItemImpl();
    Q_UNUSED(appId_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
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
