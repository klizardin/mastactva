#include "galleryeditviewmodel.h"
#include <QJsonDocument>
#include <QJsonValue>
#include "netapi.h"


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

    QObject::connect(NetAPI::getSingelton(), SIGNAL(void onJsonRequestFinished(RequestData, const QJsonDocument &)),
                     this, SLOT(void onJsonRequestFinished(RequestData *, const QJsonDocument &)));

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
    beginRemoveRows(QModelIndex(), 0, m_images.size());
    m_images.clear();
    endRemoveRows();

    m_galleryId = galleryId;
    m_request = nullptr;
    startLoadImages();
}

void GalleryEditViewImagesModel::onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_)
{
    Q_ASSERT(m_request == request_);
    m_request = nullptr;
    Q_ASSERT(reply_.isArray());
    if(!(reply_.isArray()))
    {
        return;
    }
    QList<QString> images;
    for(int i = 0; ; i++)
    {
        QJsonValue val = reply_[i];
        if(QJsonValue::Undefined == val.type())
        {
            break;
        }
        QJsonValue fn = val["filename"];
        if(QJsonValue::Undefined != fn.type() && fn.isString())
        {
            images.push_back(fn.toString());
        }
    }
    beginRemoveRows(QModelIndex(), 0, m_images.size());
    m_images.clear();
    endRemoveRows();
    beginInsertRows(QModelIndex(), m_images.size(), m_images.size() + images.size() - 1);
    std::copy(std::begin(images), std::end(images),
              std::inserter(m_images, std::end(m_images)));
    endInsertRows();
}

void GalleryEditViewImagesModel::startLoadImages()
{
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton()))
    {
        return;
    }

    m_request = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get(QString("images/%1/").arg(m_galleryId), m_request);
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
GalleryItemData::GalleryItemData(int id_ /*= -1*/,
            const QString &description_ /*= QString()*/,
            const QDateTime &created_ /*= QDateTime()*/,
            double pointsToPass_ /*= 1.0*/)
    :id(id_),
    description(description_),
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

GalleryEditViewImagesModel *GalleryItemData::getImagesModel(QObject *parent_)
{
    Q_ASSERT(nullptr != parent_);
    if(nullptr == images)
    {
        images = new GalleryEditViewImagesModel(parent_);
        images->setGalleryID(id);
    }
    return images;
}

GalleryItemData GalleryItemData::fromJson(const QJsonValue& jsonValue_, bool &anyError)
{
    int id = -1;
    QString description;
    QDateTime created;
    double pointsToPass = 1.0;

    QJsonValue idVal = jsonValue_["id"];
    if(QJsonValue::Undefined != idVal.type())
    {
        idVal = idVal.toInt();
    }
    else
    {
        anyError = true;
    }

    QJsonValue descriptionVal = jsonValue_["description"];
    if(QJsonValue::Undefined != descriptionVal.type() && descriptionVal.isString())
    {
        description = descriptionVal.toString();
    }
    else
    {
        anyError = true;
    }

    QJsonValue createdVal = jsonValue_["created"];
    if(QJsonValue::Undefined != createdVal.type() && createdVal.isString())
    {
        created = QDateTime::fromString(createdVal.toString());
    }
    else
    {
        anyError = true;
    }

    QJsonValue pointsToPassVal = jsonValue_["points_to_pass"];
    if(QJsonValue::Undefined != pointsToPassVal.type() && pointsToPassVal.isDouble())
    {
        pointsToPass = pointsToPassVal.toDouble(1.0);
    }
    else
    {
        anyError = true;
    }

    return GalleryItemData(id, description, created, pointsToPass);
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


    QObject::connect(NetAPI::getSingelton(), SIGNAL(void onJsonRequestFinished(RequestData, const QJsonDocument &)),
                     this, SLOT(void onJsonRequestFinished(RequestData *, const QJsonDocument &)));

    // TODO: indicate in view that there is no data
    startLoadGalleries();
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

void GalleryEditViewModel::onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_)
{
    Q_ASSERT(m_request == request_);
    m_request = nullptr;
    Q_ASSERT(reply_.isArray());
    if(!(reply_.isArray()))
    {
        return;
    }
    QList<GalleryItemData> galleries;
    for(int i = 0; ; i++)
    {
        QJsonValue val = reply_[i];
        if(QJsonValue::Undefined == val.type())
        {
            break;
        }
        bool anyError = false;
        GalleryItemData data = GalleryItemData::fromJson(val, anyError);
        if(anyError)
        {
            continue;
        }
        galleries.push_back(data);
    }
    beginRemoveRows(QModelIndex(), 0, m_data.size());
    m_data.clear();
    endRemoveRows();
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + galleries.size() - 1);
    std::copy(std::begin(galleries), std::end(galleries),
              std::inserter(m_data, std::end(m_data)));
    endInsertRows();
}

void GalleryEditViewModel::startLoadGalleries()
{
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton()))
    {
        return;
    }

    m_request = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get("galleries/", m_request);
}
