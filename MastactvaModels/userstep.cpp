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

#include "userstep.h"
#include "../MastactvaBase/defines.h"


UserStep::UserStep(QObject *parent_)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "UserStep::UserStep()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "UserStep::UserStep()" << this;
#endif

}

UserStep::~UserStep()
{
}

int UserStep::id() const
{
    return m_id;
}

void UserStep::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int UserStep::userId() const
{
    return m_userId;
}

void UserStep::setUserId(const int &userId_)
{
    m_userId = userId_;

    emit userIdChanged();
}

int UserStep::galleryId() const
{
    return m_galleryId;
}

void UserStep::setGalleryId(const int &galleryId_)
{
    m_galleryId = galleryId_;

    emit galleryIdChanged();
}

int UserStep::imageId() const
{
    return m_imageId;
}

void UserStep::setImageId(const int &imageId_)
{
    m_imageId = imageId_;

    emit imageIdChanged();
}

int UserStep::nextImageId() const
{
    return m_nextImageId;
}

void UserStep::setNextImageId(const int &nextImageId_)
{
    m_nextImageId = nextImageId_;

    emit nextImageIdChanged();
}

int UserStep::descriptionId() const
{
    return m_descriptionId;
}

void UserStep::setDescriptionId(const int &descriptionId_)
{
    m_descriptionId = descriptionId_;

    emit descriptionIdChanged();
}

int UserStep::questionId() const
{
    return m_questionId;
}

void UserStep::setQuestionId(const int &questionId_)
{
    m_questionId = questionId_;

    emit questionIdChanged();
}

int UserStep::answerId() const
{
    return m_answerId;
}

void UserStep::setAnswerId(const int &answerId_)
{
    m_answerId = answerId_;

    emit answerIdChanged();
}

QDateTime UserStep::t() const
{
    return m_t;
}

void UserStep::setT(const QDateTime &t_)
{
    m_t = t_;

    emit tChanged();
}


UserStepModel::UserStepModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "UserStepModel::UserStepModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "UserStepModel::UserStepModel()" << this;
#endif

    init(this);
}
