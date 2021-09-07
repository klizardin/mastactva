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

#include "gallerystatistics.h"
#include "../MastactvaBase/defines.h"


GalleryStatistics::GalleryStatistics(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "GalleryStatistics::GalleryStatistics()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "GalleryStatistics::GalleryStatistics()" << this;
#endif

}

int GalleryStatistics::id() const
{
    return m_id;
}

void GalleryStatistics::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int GalleryStatistics::userId() const
{
    return m_userId;
}

void GalleryStatistics::setUserId(const int &userId_)
{
    m_userId = userId_;

    emit userIdChanged();
}

int GalleryStatistics::galleryId() const
{
    return m_galleryId;
}

void GalleryStatistics::setGalleryId(const int &galleryId_)
{
    m_galleryId = galleryId_;

    emit galleryIdChanged();
}

qreal GalleryStatistics::points() const
{
    return m_points;
}

void GalleryStatistics::setPoints(const qreal &points_)
{
    m_points = points_;

    emit pointsChanged();
}


GalleryStatisticsModel::GalleryStatisticsModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "GalleryStatisticsModel::GalleryStatisticsModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "GalleryStatisticsModel::GalleryStatisticsModel()" << this;
#endif

    init(this);
}


GalleryStatisticsModelView::GalleryStatisticsModelView(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
}
