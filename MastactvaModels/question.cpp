#include "question.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


Question::Question(QuestionModel *parent_)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Question::Question()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_questionModel = parent_;
}

int Question::id() const
{
    return m_id;
}

void Question::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString Question::question() const
{
    return m_question;
}

void Question::setQuestion(const QString &question_)
{
    m_question = leftDoubleCR(question_);

    emit questionChanged();
}

qreal Question::pointsToPass() const
{
    return m_pointsToPass;
}

void Question::setPointsToPass(const qreal &pointsToPass_)
{
    m_pointsToPass = pointsToPass_;

    emit pointsToPassChanged();
}

QDateTime Question::created() const
{
    return m_created;
}

void Question::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QVariant Question::answers() const
{
    if(nullptr == m_answerModel)
    {
        const_cast<Question *>(this)->m_answerModel = const_cast<Question *>(this)
                ->createAnswerModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<AnswerModel *>(
                                       m_answerModel)
                                   )
                               );
}

void Question::setAnswers(const QVariant &answers_)
{
    if(answers_.isNull() && nullptr != m_answerModel)
    {
        delete m_answerModel;
        m_answerModel = nullptr;

        emit answersChanged();
    }
}

QVariant Question::userQuestionAnswer() const
{
    if(nullptr == m_userQuestionAnswerModel)
    {
        const_cast<Question *>(this)->m_userQuestionAnswerModel = const_cast<Question *>(this)
                ->createUserQuestionAnswerModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<UserQuestionAnswerModel *>(
                                       m_userQuestionAnswerModel)
                                   )
                               );
}

AnswerModel *Question::createAnswerModel()
{
    AnswerModel *m = new AnswerModel(this);
    m->initResponse();
    m->setLayoutRefImpl("question", m_questionModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("question");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_questionModel->getQMLLayoutName() + QString("_Question_") +
                        QVariant::fromValue(m_appId).toString() + QString("_AnswerModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    //m->setSortFieldsImpl({"?",});
    m->loadList();
    return m;
}

UserQuestionAnswerModel *Question::createUserQuestionAnswerModel()
{
    UserQuestionAnswerModel *m = new UserQuestionAnswerModel(this);
    m->initResponse();
#if !defined(LOCALDATAAPICACHE)
    m->addLayoutExtraFieldsImpl("QuizUserModel", QVariant());
    m->addExtraFieldRename("deviceid", "user");
#endif
    m->setLayoutRefImpl("question", m_questionModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("question");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_questionModel->getQMLLayoutName() + QString("_Question_") +
                        QVariant::fromValue(m_appId).toString() + QString("_UserQuestionAnswerModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setOutputModel(true);
    m->setReadonlyImpl(false);
    m->loadList();
    return m;
}


QuestionModel::QuestionModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "QuestionModel::QuestionModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
}
