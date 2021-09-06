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

#include "gallery.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


Gallery::Gallery(GalleryModel *parent_)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Gallery::Gallery()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "Gallery::Gallery()" << this;
#endif

    m_galleryModel =  parent_;
}

Gallery::~Gallery()
{
    if(m_imagesModel)
    {
        m_imagesModel->deleteLater();
    }
    m_imagesModel = nullptr;
    if(m_galleryStatisticsModel)
    {
        m_galleryStatisticsModel->deleteLater();
    }
    m_galleryStatisticsModel = nullptr;
}

int Gallery::getAppId() const
{
    return m_appId;
}

int Gallery::id() const
{
    return m_id;
}

void Gallery::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString Gallery::description() const
{
    return m_description;
}

void Gallery::setDescription(const QString &description_)
{
    m_description = leftDoubleCR(description_);

    emit descriptionChanged();
}

QString Gallery::keyword() const
{
    return m_keywords;
}

void Gallery::setKeyword(const QString &keyword_)
{
    m_keywords = keyword_;

    emit keywordChanged();
}

QDateTime Gallery::created() const
{
    return m_created;
}

void Gallery::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

qreal Gallery::pointsToPass() const
{
    return m_pointsToPass;
}

void Gallery::setPointsToPass(const qreal &pointToPass_)
{
    m_pointsToPass = pointToPass_;

    emit pointsToPassChanged();
}

int Gallery::ownGallery() const
{
    return m_ownGallery;
}

void Gallery::setOwnGallery(const int &ownGallery_)
{
    m_ownGallery = ownGallery_;

    emit ownGalleryChanged();
}

ImageModel *Gallery::createImages()
{
    ImageModel *m = new ImageModel(this);
    m->initResponse();
#if !defined(LOCALDATAAPICACHE)
    m->addLayoutExtraFieldsImpl("QuizUserModel", QVariant());
#endif
    m->setLayoutRefImpl("gallery", m_galleryModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("gallery");
    m->addModelParam("use_in_gallery_view", "1");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_galleryModel->getQMLLayoutName() + QString("_Gallery_") +
                        QVariant::fromValue(m_appId).toString() + QString("_GalleryImageModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_galleryModel);
    m->setAutoCreateChildrenModels(true);           // TODO: find way to upload images of gallery when it is required
    //m->setAutoCreateChildrenModelsOnSelect(true);
    m->loadList();
    return m;
}

QVariant Gallery::images()
{
    if(!m_imagesModel)
    {
        m_imagesModel = createImages();
    }
    return QVariant::fromValue(static_cast<QObject *>(m_imagesModel));
}

QVariant Gallery::statistics() const
{
    if(!m_galleryStatisticsModel)
    {
        const_cast<Gallery *>(this)->m_galleryStatisticsModel = const_cast<Gallery *>(this)
                ->createGalleryStatistics();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<GalleryStatisticsModel *>(
                                       m_galleryStatisticsModel)
                                   )
                               );
}

void Gallery::setStatistics(const QVariant &obj_)
{
    if(obj_.isNull() && m_galleryStatisticsModel)
    {
        delete m_galleryStatisticsModel;
        m_galleryStatisticsModel = nullptr;

        emit statisticsChanged();
    }
}

GalleryStatisticsModel *Gallery::createGalleryStatistics()
{
    GalleryStatisticsModel *m = new GalleryStatisticsModel(this);
    m->initResponse();
#if !defined(LOCALDATAAPICACHE)
    m->addLayoutExtraFieldsImpl("QuizUserModel", QVariant());
    m->addExtraFieldRenameImpl("deviceid", "user");
#endif
    m->setLayoutRefImpl("gallery", m_galleryModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("gallery");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_galleryModel->getQMLLayoutName() + QString("_Gallery_") +
                        QVariant::fromValue(m_appId).toString() + QString("_GalleryStatisticsModel_"));
    m->registerListModel();
    //m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setReadonlyImpl(false);
    m->loadList();
    return m;
}


GalleryModel::GalleryModel(QObject *parent_ /*= nullptr*/)
    : ListModelBaseOfData<Gallery, GalleryModel>(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "GalleryModel::GalleryModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "GalleryModel::GalleryModel()" << this;
#endif

    init(this);
}
