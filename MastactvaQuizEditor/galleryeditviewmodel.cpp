#include "galleryeditviewmodel.h"
#include <QJsonDocument>
#include <QJsonValue>
#include "netapi.h"
#include "qmlmainobjects.h"


/////////////////////////////////////////////////////////////////////////////////////////
/// class GalleryEditViewImagesModel
///

static const QString g_noImageDefault = QString("qrc:///resources/no-image.png");

/////////////////////////////////////////////////////////////////////////////////////////
/// \brief GalleryEditViewImagesModel::GalleryEditViewImagesModel
/// \param parent - parent QObject
///
GalleryImagesModel::GalleryImagesModel(QObject *parent /*= nullptr*/)
    :QAbstractListModel(parent)
{
    m_roleNames[ImageSourceRole] = "image";

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
    QString imageUrl = m_images[row];
    switch(role)
    {
    case ImageSourceRole:
        return imageUrl;
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

QObject* GalleryEditViewModel::getCurrentItem()
{
    if(m_currentIndex < 0 || m_currentIndex >= m_data.size())
    {
        return nullptr;
    }
    return m_data.at(m_currentIndex);
}
