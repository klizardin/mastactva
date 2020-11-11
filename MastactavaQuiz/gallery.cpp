#include "gallery.h"
#include "utils.h"


Gallery::Gallery(GalleryModel *parent_)
    : QObject(parent_)
{
    m_galleryModel =  parent_;
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
    m->addLayoutExtraGetFieldsImpl("QuizUserModel", QVariant());
    m->setLayoutRefImpl("gallery", m_galleryModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("gallery");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(QString("GalleryImageModel") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
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


GalleryModel::GalleryModel(QObject *parent_ /*= nullptr*/)
    : ListModelBaseOfData<Gallery, GalleryModel>(parent_)
{
    init(this);
}
