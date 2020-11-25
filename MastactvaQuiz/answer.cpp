#include "answer.h"
#include "../MastactvaBase/utils.h"


Answer::Answer(QObject *parent_)
    : QObject(parent_)
{
}

int Answer::id() const
{
    return m_id;
}

void Answer::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int Answer::question() const
{
    return m_questionId;
}

void Answer::setQuestion(const int &questionId_)
{
    m_questionId = questionId_;

    emit questionChanged();
}

QString Answer::answer() const
{
    return m_answer;
}

void Answer::setAnswer(const QString &answer_)
{
    m_answer = leftDoubleCR(answer_);

    emit answerChanged();
}

qreal Answer::points() const
{
    return m_points;
}

void Answer::setPoints(const qreal &points_)
{
    m_points = points_;

    emit pointsChanged();
}

AnswerModel::AnswerModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
    init(this);
}
