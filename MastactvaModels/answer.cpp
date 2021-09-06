/*
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

#include "answer.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


Answer::Answer(QObject *parent_)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Answer::Answer()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "Answer::Answer()" << this;
#endif
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
#if defined(TRACE_THREADS)
    qDebug() << "AnswerModel::AnswerModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "AnswerModel::AnswerModel()" << this;
#endif

    init(this);
}
