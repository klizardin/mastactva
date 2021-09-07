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

#include "imagedescription.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


ImageDescription::ImageDescription(QObject *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImageDescription::ImageDescription()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImageDescription::ImageDescription()" << this;
#endif

}

int ImageDescription::id() const
{
    return m_id;
}

void ImageDescription::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImageDescription::image() const
{
    return m_imageId;
}

void ImageDescription::setImage(const int &imageId_)
{
    m_imageId = imageId_;

    emit imageChanged();
}

QDateTime ImageDescription::fromDate() const
{
    return m_fromDate;
}

void ImageDescription::setFromDate(const QDateTime &fromDate_)
{
    m_fromDate = fromDate_;

    emit fromDateChanged();
}

QString ImageDescription::description() const
{
    return m_descriptionText;
}

void ImageDescription::setDescription(const QString &descriptionText_)
{
    m_descriptionText = leftDoubleCR(descriptionText_);

    emit descriptionChanged();
}


ImageDescriptionModel::ImageDescriptionModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImageDescriptionModel::ImageDescriptionModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImageDescriptionModel::ImageDescriptionModel()" << this;
#endif

    init(this);
}
