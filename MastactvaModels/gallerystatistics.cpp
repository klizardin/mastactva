#include "gallerystatistics.h"
#include "../MastactvaBase/defines.h"


GalleryStatistics::GalleryStatistics(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "GalleryStatistics::GalleryStatistics()" << QThread::currentThread() << QThread::currentThreadId();
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
    init(this);
}


GalleryStatisticsModelView::GalleryStatisticsModelView(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
}
