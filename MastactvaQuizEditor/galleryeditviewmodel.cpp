#include "galleryeditviewmodel.h"
#include <QJsonDocument>
#include <QJsonValue>
#include "netapi.h"
#include "qmlmainobjects.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// class ImageData
///

ImageData::ImageData(int id_ /*= -1*/, const QString& source_ /*= QString()*/)
    :m_id(id_), m_source(source_)
{
}

int ImageData::getId() const
{
    return m_id;
}

void ImageData::setId(int id_)
{
    m_id = id_;
}

const QString& ImageData::getSource() const
{
    return m_source;
}

void ImageData::setSource(const QString& source_)
{
    m_source = source_;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// class GalleryEditViewImagesModel
///

static const ImageData g_noImageDefault = ImageData(-1, "qrc:///resources/no-image.png");

/////////////////////////////////////////////////////////////////////////////////////////
/// \brief GalleryEditViewImagesModel::GalleryEditViewImagesModel
/// \param parent - parent QObject
///
GalleryImagesModel::GalleryImagesModel(QObject *parent /*= nullptr*/)
    :QAbstractListModel(parent)
{
    m_roleNames[IdRole] = "image_id";
    m_roleNames[ImageSourceRole] = "image_source";

    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(RequestData *, const QJsonDocument &)));

    m_images.push_back(g_noImageDefault);
}

GalleryImagesModel::~GalleryImagesModel()
{
}

int GalleryImagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_images.size();
}

QVariant GalleryImagesModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if(row < 0 || row >= m_images.size())
    {
        return QVariant();
    }
    const auto &image = m_images[row];
    switch(role)
    {
    case IdRole:
        return image.getId();
    case ImageSourceRole:
        return image.getSource();
    }
    return QVariant();
}

QHash<int, QByteArray> GalleryImagesModel::roleNames() const
{
    return m_roleNames;
}

bool GalleryImagesModel::galleryViewImages() const
{
    return m_showGalleryViewImages;
}

void GalleryImagesModel::setGalleryViewImages(bool modeShowGalleryViewImages_)
{
    m_showGalleryViewImages = modeShowGalleryViewImages_;

    startLoadImages();

    emit galleryViewImagesChanged();
}

int GalleryImagesModel::galleryId() const
{
    return m_galleryId;
}

void GalleryImagesModel::setGalleryId(int galleryId_)
{
    beginRemoveRows(QModelIndex(), 0, m_images.size());
    m_images.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), m_images.size(), m_images.size() + 1 - 1);
    m_images.push_back(g_noImageDefault);
    endInsertRows();

    m_galleryId = galleryId_;
    m_request = nullptr;
    startLoadImages();

    emit galleryIdChanged();
}

int GalleryImagesModel::galleryIndex() const
{
    return m_galleryIndex;
}

void GalleryImagesModel::setGalleryIndex(int galleryIndex_)
{
    if(m_galleryIndex == galleryIndex_)
    {
        return;
    }

    m_galleryIndex = galleryIndex_;
    emit galleryIndexChanged();

    GalleryEditViewModel *model = QMLMainObjects::getSingelton()->getGalleryViewModel();
    if(!(nullptr != model))
    {
        return;
    }

    m_galleryId = model->getIdOfIndex(m_galleryIndex);
    m_request = nullptr;
    startLoadImages();

    emit galleryIdChanged();
}

void GalleryImagesModel::onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_)
{
    if(!(m_request == request_))
    {
        return;
    }
    m_request = nullptr;
    Q_ASSERT(reply_.isArray());
    if(!(reply_.isArray()))
    {
        return;
    }
    QList<ImageData> images;
    for(int i = 0; ; i++)
    {
        QJsonValue val = reply_[i];
        if(QJsonValue::Undefined == val.type())
        {
            break;
        }
        int id = -1;
        QString filename;
        QJsonValue idVal = val["id"];
        if(QJsonValue::Undefined != idVal.type())
        {
            id = idVal.toInt(-1);
        }
        QJsonValue fn = val["filename"];
        if(QJsonValue::Undefined != fn.type() && fn.isString())
        {
            filename = fn.toString();
        }
        images.push_back({id, filename});
    }
    beginRemoveRows(QModelIndex(), 0, m_images.size());
    m_images.clear();
    endRemoveRows();
    if(images.size() > 0)
    {
        beginInsertRows(QModelIndex(), m_images.size(), m_images.size() + images.size() - 1);
        std::copy(std::begin(images), std::end(images),
                  std::inserter(m_images, std::end(m_images)));
        endInsertRows();
    }
    else
    {
        beginInsertRows(QModelIndex(), m_images.size(), m_images.size() + 1 - 1);
        m_images.push_back(g_noImageDefault);
        endInsertRows();
    }
}

void GalleryImagesModel::ongalleryIdChanged()
{
    startLoadImages();
}

void GalleryImagesModel::startLoadImages()
{
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton()))
    {
        return;
    }

    m_request = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get(
                m_showGalleryViewImages
                    ?QString("images/%1/gallery_view/").arg(m_galleryId)
                    :QString("images/%1/gallery_all/").arg(m_galleryId)
                    ,
                m_request);
}

int GalleryImagesModel::getIdOfIndex(int index_)
{
    if(index_ < 0 || index_ > m_images.size())
    {
        return -1;
    }
    return m_images.at(index_).getId();
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
GalleryItemData::GalleryItemData(QObject* parent_,
            int id_ /*= -1*/,
            const QString &description_ /*= QString()*/,
            const QString &keywords_ /*= QString()*/,
            const QDateTime &created_ /*= QDateTime()*/,
            qreal pointsToPass_ /*= 1.0*/)
    : QObject(parent_),
    id(id_),
    description(description_),
    keywords(keywords_),
    created(created_),
    pointsToPass(pointsToPass_)
{
}

GalleryItemData::~GalleryItemData()
{
    //delete images;
    //images = nullptr;
}

int GalleryItemData::getId() const
{
    return id;
}

const QString &GalleryItemData::getDescription() const
{
    return description;
}

const QString &GalleryItemData::getKeywords() const
{
    return keywords;
}

const QDateTime &GalleryItemData::getCreated() const
{
    return created;
}

QString GalleryItemData::getPointsToPass() const
{
    QVariant v(pointsToPass);
    return v.toString();
}

GalleryImagesModel *GalleryItemData::getImagesModel()
{
    if(nullptr == images)
    {
        images = new GalleryImagesModel(this);
        images->setGalleryId(id);
    }
    return images;
}

QObject* GalleryItemData::getImages()
{
    return getImagesModel();
}

void GalleryItemData::setId(int id_)
{
    id = id_;
}

void GalleryItemData::setDescription(const QString& description_)
{
    description = description_;
}

void GalleryItemData::setKeywords(const QString& keywords_)
{
    keywords = keywords_;
}

void GalleryItemData::setCreated(const QDateTime& created_)
{
    created = created_;
}

void GalleryItemData::setPointsToPass(const QString &pointsToPass_)
{
    QVariant v(pointsToPass_);
    pointsToPass = v.toDouble();
}

GalleryItemData* GalleryItemData::fromJson(QObject* parent_, const QJsonValue& jsonValue_, bool &anyError)
{
    int id = -1;
    QString description;
    QString keywords;
    QDateTime created;
    qreal pointsToPass = 1.0;

    QJsonValue idVal = jsonValue_["id"];
    if(QJsonValue::Undefined != idVal.type())
    {
        id = idVal.toInt();
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

    QJsonValue keywordsVal = jsonValue_["description"];
    if(QJsonValue::Undefined != keywordsVal.type() && keywordsVal.isString())
    {
        keywords = keywordsVal.toString();
    }
    else
    {
        anyError = true;
    }

    QJsonValue createdVal = jsonValue_["created"];
    if(QJsonValue::Undefined != createdVal.type() && createdVal.isString())
    {
        QString str = createdVal.toString();
        QTextStream s(&str);
        int year = 0, month = 0, day = 0, hours = 0, minites = 0, seconds = 0;
        s.setPadChar('-');
        char tmp;
        s >> year >> tmp >> month >> tmp >> day >> tmp;
        s >> hours >> tmp >> minites >> tmp >> seconds;
        created = QDateTime(QDate(year, month, day), QTime(hours, minites, seconds));
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

    return new GalleryItemData(parent_, id, description, keywords, created, pointsToPass);
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
    m_roleNames[GalleryIDRole] = "galleryid";
    m_roleNames[DescriptionRole] = "description";
    m_roleNames[CreatedRole] = "created";
    m_roleNames[PointsToPassRole] = "pointsToPass";
    m_roleNames[ImagesRole] = "images";


    // TODO: indicate in view that there is no data
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(RequestData *, const QJsonDocument &)));
    startLoadGalleries();
}

GalleryEditViewModel::~GalleryEditViewModel()
{
    clearData();
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
    const GalleryItemData *item = m_data[row];
    switch(role)
    {
    case GalleryIDRole:
        return item->getId();
    case DescriptionRole:
        return item->getDescription();
    case CreatedRole:
        return item->getCreated();
    case PointsToPassRole:
        return item->getPointsToPass();
    case ImagesRole:
        return QVariant::fromValue<QObject *>(
                const_cast<GalleryItemData *>(item)
                    ->getImagesModel()
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
    if(!(m_request == request_))
    {
        return;
    }
    m_request = nullptr;
    Q_ASSERT(reply_.isArray());
    if(!(reply_.isArray()))
    {
        return;
    }
    QVector<GalleryItemData*> galleries;
    for(int i = 0; ; i++)
    {
        QJsonValue val = reply_[i];
        if(QJsonValue::Undefined == val.type())
        {
            break;
        }
        bool anyError = false;
        GalleryItemData *data = GalleryItemData::fromJson(this, val, anyError);
        if(anyError)
        {
            continue;
        }
        galleries.push_back(data);
    }
    beginRemoveRows(QModelIndex(), 0, m_data.size());
    clearData();
    endRemoveRows();
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + galleries.size() - 1);
    std::copy(std::begin(galleries), std::end(galleries),
              std::inserter(m_data, std::end(m_data)));
    endInsertRows();

    emit galleryRealoded();
}

void GalleryEditViewModel::clearData()
{
    for(auto *&p : m_data)
    {
        delete p;
        p = nullptr;
    }
    m_data.clear();
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

int GalleryEditViewModel::getIdOfIndex(int index_) const
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return -1;
    }
    return m_data.at(index_)->getId();
}

int GalleryEditViewModel::currentIndex() const
{
    return m_currentIndex;
}

void GalleryEditViewModel::setCurrentIndex(int index_)
{
    m_currentIndex = index_;

    emit currentIndexChanged();
}

int GalleryEditViewModel::currentId() const
{
    return getIdOfIndex(m_currentIndex);
}

void GalleryEditViewModel::setCurrentId(int id_)
{
    auto fit = std::find_if(std::begin(m_data),
                            std::end(m_data),
                            [id_] (const GalleryItemData* item) -> bool
    {
       return nullptr != item && item->getId() == id_;
    });
    if(std::end(m_data) == fit)
    {
        return;
    }

    m_currentIndex = std::distance(std::begin(m_data), fit);

    emit currentIndexChanged();
    emit currentIdChanged();
}

QObject* GalleryEditViewModel::getCurrentItem()
{
    if(m_currentIndex < 0 || m_currentIndex >= m_data.size())
    {
        return nullptr;
    }
    return m_data.at(m_currentIndex);
}

ImagePointData::ImagePointData(QObject *parent_,
               int sourceImageId_ /*= -1*/,
               int pointId_ /*= -1*/,
               qreal x_ /*= 0.5*/,
               qreal y_ /*= 0.5*/,
               qreal weight_ /*= 1.0*/)
    : QObject(parent_),
    m_sourceImageId(sourceImageId_),
    m_pointId(pointId_),
    m_x(x_),
    m_y(y_),
    m_weight(weight_)
{
}

int ImagePointData::getSourceImageId() const
{
    return m_sourceImageId;
}

void ImagePointData::setSourceImageId(int sourceImageId_)
{
    m_sourceImageId = sourceImageId_;
}

int ImagePointData::pointId() const
{
    return m_pointId;
}

void ImagePointData::setPointId(int id_)
{
    m_pointId = id_;

    emit pointIdChanged();
}

qreal ImagePointData::xCoord() const
{
    return m_x;
}

void ImagePointData::setXCoord(qreal x_)
{
    m_x = x_;

    emit xCoordChanged();
}

qreal ImagePointData::yCoord() const
{
    return m_y;
}

void ImagePointData::setYCoord(qreal y_)
{
    m_y = y_;

    emit yCoordChanged();
}

qreal ImagePointData::weight() const
{
    return m_weight;
}

void ImagePointData::setWeight(qreal weight_)
{
    m_weight = weight_;

    emit weightChanged();
}


ImagePointsModel::ImagePointsModel(QObject *parent_)
    :QAbstractListModel(parent_)
{
    m_roleNames[IDRole] = "imagePoint_id";
    m_roleNames[XRole] = "imagePoint_x";
    m_roleNames[YRole] = "imagePoint_y";
    m_roleNames[WeightRole] = "imagePoint_weight";
}

ImagePointsModel::~ImagePointsModel()
{
    clearData();
}

int ImagePointsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

QVariant ImagePointsModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if(row < 0 || row >= m_data.size())
    {
        return QVariant();
    }
    const ImagePointData *item = m_data[row];
    switch(role)
    {
    case IDRole:
        return item->pointId();
    case XRole:
        return item->xCoord();
    case YRole:
        return item->yCoord();
    case WeightRole:
        return item->weight();
    }
    return QVariant();
}

bool ImagePointsModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    const int row = index.row();
    if(row < 0 || row >= m_data.size())
    {
        return false;
    }
    ImagePointData *item = m_data[row];
    switch(role)
    {
    case IDRole:
        item->setPointId(value.toInt());
        break;
    case XRole:
        item->setXCoord(value.toDouble());
        break;
    case YRole:
        item->setYCoord(value.toDouble());
        break;
    case WeightRole:
        item->setWeight(value.toDouble());
        break;
    default:
        return false;
    }
    return true;
}

void ImagePointsModel::clearData()
{
    for(auto *&p : m_data)
    {
        delete p;
        p = nullptr;
    }
    m_data.clear();
}

QHash<int, QByteArray> ImagePointsModel::roleNames() const
{
    return m_roleNames;
}
