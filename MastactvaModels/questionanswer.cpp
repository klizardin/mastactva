/*
    Copyright 2021

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
