#include "gallery.h"
#include "../MastactvaBase/utils.h"


Gallery::Gallery(GalleryModel *parent_)
    : QObject(parent_)
{
    m_galleryModel =  parent_;
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
    m->addLayoutExtraFieldsImpl("QuizUserModel", QVariant());
    m->setLayoutRefImpl("gallery", m_galleryModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("gallery");
    m->addModelParam("use_in_gallery_view", "1");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_galleryModel->getQMLLayoutName() + QString("_Gallery_") + QVariant::fromValue(m_appId).toString() + QString("_GalleryImageModel_"));
    m->setLayoutIdFieldImpl("id");
    m->registerListModel();
    m->setParentListModelInfo(m_galleryModel);
    m->setAutoCreateChildrenModels(true);           // TODO: find way to upload images of gallery when it is required
    //m->setAutoCreateChildrenModelsOnSelect(true);
    m->loadList();
    return m;
}

QVariant Gallery::images()
{
    if(nullptr == m_images)
    {
        m_images = createImages();
    }
    return QVariant::fromValue(static_cast<QObject *>(m_images));
}

QVariant Gallery::statistics() const
{
    if(nullptr == m_galleryStatisticsModel)
    {
        const_cast<Gallery *>(this)->m_galleryStatisticsModel = const_cast<Gallery *>(this)->createGalleryStatistics();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<GalleryStatisticsModel *>(m_galleryStatisticsModel)));
}

void Gallery::setStatistics(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_galleryStatisticsModel)
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
    m->addLayoutExtraFieldsImpl("QuizUserModel", QVariant());
    m->addExtraFieldRenameImpl("deviceid", "user");
    m->setLayoutRefImpl("gallery", m_galleryModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("gallery");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_galleryModel->getQMLLayoutName() + QString("_Gallery_") + QVariant::fromValue(m_appId).toString() + QString("_GalleryStatisticsModel_"));
    m->setLayoutIdFieldImpl("id");
    m->registerListModel();
    //m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


GalleryModel::GalleryModel(QObject *parent_ /*= nullptr*/)
    : ListModelBaseOfData<Gallery, GalleryModel>(parent_)
{
    init(this);
}
