#include "question.h"

Question::Question(QObject *parent_)
    : QObject(parent_)
{

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
    m_question = question_;

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


QuestionModel::QuestionModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
    init(this);
}
