#include "questionanswer.h"

QuestionAnswer::QuestionAnswer(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
}

int QuestionAnswer::id() const
{
    return m_id;
}

void QuestionAnswer::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int QuestionAnswer::userId() const
{
    return m_userId;
}

void QuestionAnswer::setUserId(const int &userId_)
{
    m_userId = userId_;

    emit userIdChanged();
}

int QuestionAnswer::questionId() const
{
    return m_questionId;
}

void QuestionAnswer::setQuestionId(const int &questionId_)
{
    m_questionId = questionId_;

    emit questionIdChanged();
}

QDateTime QuestionAnswer::t() const
{
    return m_t;
}

void QuestionAnswer::setT(const QDateTime &t_)
{
    m_t = t_;

    emit tChanged();
}

int QuestionAnswer::answerId() const
{
    return m_answerId;
}

void QuestionAnswer::setAnswerId(const int &answerId_)
{
    m_answerId = answerId_;

    emit answerIdChanged();
}

qreal QuestionAnswer::points() const
{
    return m_points;
}

void QuestionAnswer::setPoints(const qreal &points_)
{
    m_points = points_;

    emit pointsChanged();
}

QuestionAnswerModel::QuestionAnswerModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}

