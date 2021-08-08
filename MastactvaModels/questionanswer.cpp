#include "questionanswer.h"
#include "../MastactvaBase/defines.h"


UserQuestionAnswer::UserQuestionAnswer(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "UserQuestionAnswer::UserQuestionAnswer()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "UserQuestionAnswer::UserQuestionAnswer()" << this;
#endif

}

int UserQuestionAnswer::id() const
{
    return m_id;
}

void UserQuestionAnswer::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int UserQuestionAnswer::userId() const
{
    return m_userId;
}

void UserQuestionAnswer::setUserId(const int &userId_)
{
    m_userId = userId_;

    emit userIdChanged();
}

int UserQuestionAnswer::questionId() const
{
    return m_questionId;
}

void UserQuestionAnswer::setQuestionId(const int &questionId_)
{
    m_questionId = questionId_;

    emit questionIdChanged();
}

QDateTime UserQuestionAnswer::t() const
{
    return m_t;
}

void UserQuestionAnswer::setT(const QDateTime &t_)
{
    m_t = t_;

    emit tChanged();
}

int UserQuestionAnswer::answerId() const
{
    return m_answerId;
}

void UserQuestionAnswer::setAnswerId(const int &answerId_)
{
    m_answerId = answerId_;

    emit answerIdChanged();
}

qreal UserQuestionAnswer::points() const
{
    return m_points;
}

void UserQuestionAnswer::setPoints(const qreal &points_)
{
    m_points = points_;

    emit pointsChanged();
}


UserQuestionAnswerModel::UserQuestionAnswerModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "UserQuestionAnswerModel::UserQuestionAnswerModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "UserQuestionAnswerModel::UserQuestionAnswerModel()" << this;
#endif

    init(this);
}


UserQuestionAnswerModelView::UserQuestionAnswerModelView(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
}
