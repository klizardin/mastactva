#include "galleryeditviewmodel.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// class GalleryEditViewImagesModel
///

/////////////////////////////////////////////////////////////////////////////////////////
/// \brief GalleryEditViewImagesModel::GalleryEditViewImagesModel
/// \param parent - parent QObject
///
GalleryEditViewImagesModel::GalleryEditViewImagesModel(QObject *parent /*= nullptr*/)
    :QAbstractListModel(parent)
{
    m_roleNames[ImageSourceRole] = "image";

    // TODO: indicate in view that there is no data
}

GalleryEditViewImagesModel::~GalleryEditViewImagesModel()
{
}

int GalleryEditViewImagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_images.size();
}

QVariant GalleryEditViewImagesModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if(row < 0 || row >= m_images.size())
    {
        return QVariant();
    }
    QString imageUrl = m_images[row];
    switch(role)
    {
    case ImageSourceRole:
        return imageUrl;
    }
    return QVariant();
}

QHash<int, QByteArray> GalleryEditViewImagesModel::roleNames() const
{
    return m_roleNames;
}

void GalleryEditViewImagesModel::setGalleryID(long long galleryId)
{
    // TODO: inform that empty images list
    m_images.clear();

    m_galleryId = galleryId;
    // TODO: inform that it is starting images list loading
}


/////////////////////////////////////////////////////////////////////////////////////////
/// class GalleryItemData
///

/////////////////////////////////////////////////////////////////////////////////////////
/// \brief GalleryItemData::GalleryItemData - data type constructor
/// \param description_ - description
/// \param created_ - created date time
/// \param pointsToPass_ - point to pass gallery (level)
///
GalleryItemData::GalleryItemData(const QString &description_ /*= QString()*/,
            const QDateTime &created_ /*= QDateTime()*/,
            double pointsToPass_ /*= 1.0*/)
    :description(description_),
    created(created_),
    pointsToPass(pointsToPass_)
{
}

GalleryItemData::~GalleryItemData()
{
    //delete images;
    //images = nullptr;
}

const QString &GalleryItemData::getDescription() const
{
    return description;
}

const QDateTime &GalleryItemData::getCreated() const
{
    return created;
}

double GalleryItemData::getPointsToPass() const
{
    return pointsToPass;
}

GalleryEditViewImagesModel *GalleryItemData::getImagesModel(QObject *parent)
{
    Q_ASSERT(nullptr != parent);
    if(nullptr == images)
    {
        images = new GalleryEditViewImagesModel(parent);
    }
    return images;
}


///////////////////////////////////////////////////////////////////////////////////////////
/// class GalleryEditViewModel
///

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief GalleryEditViewModel::GalleryEditViewModel
/// \param parent
///
GalleryEditViewModel::GalleryEditViewModel(QObject *parent)
    :QAbstractListModel(parent)
{
    m_roleNames[DescriptionRole] = "description";
    m_roleNames[CreatedRole] = "created";
    m_roleNames[PointsToPassRole] = "pointsToPass";
    m_roleNames[ImagesRole] = "images";


    // TODO: indicate in view that there is no data
    // TODO: start to download gallery view data
}

GalleryEditViewModel::~GalleryEditViewModel()
{
}

int GalleryEditViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

QVariant GalleryEditViewModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if(row < 0 || row >= m_data.size())
    {
        return QVariant();
    }
    const GalleryItemData &item = m_data[row];
    switch(role)
    {
    case DescriptionRole:
        return item.getDescription();
    case CreatedRole:
        return item.getCreated();
    case PointsToPassRole:
        return item.getPointsToPass();
    case ImagesRole:
        return QVariant::fromValue<QObject *>(
                const_cast<GalleryItemData &>(item)
                    .getImagesModel(
                        const_cast<QObject *>(
                            static_cast<const QObject *>(this)
                            )
                        )
                    );
    }
    return QVariant();
}

QHash<int, QByteArray> GalleryEditViewModel::roleNames() const
{
    return m_roleNames;
}
