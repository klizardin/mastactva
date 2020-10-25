#include "galleryeditviewmodel.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QSGRendererInterface>
#include <QQuickWindow>
#include <QAbstractItemModel>
#include <QRunnable>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QtGui/QColor>
#include <QtQuick/QSGSimpleMaterial>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QDebug>
#include "netapi.h"
#include "qmlmainobjects.h"


QDateTime dateTimeFromJsonString(const QString& dateTimeZ)
{
    QString dateTimeZ1(dateTimeZ);
    QTextStream s(&dateTimeZ1);
    int year = 0, month = 0, day = 0, hours = 0, minites = 0, seconds = 0;
    char tmp;
    s >> year >> tmp >> month >> tmp >> day >> tmp;
    s >> hours >> tmp >> minites >> tmp >> seconds;
    if(seconds > 100)
    {
        seconds /= 1000;
    }
    return QDateTime(QDate(year, month, day), QTime(hours, minites, seconds));
}

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

void ImageData::initImagePointModel(QObject *parent_)
{
    if(nullptr == imagePointsModel)
    {
        imagePointsModel = new ImagePointsModel(parent_);
        imagePointsModel->setSourceImageId(m_id);
        imagePointsModel->startLoadImagePoints();
    }
}

ImagePointsModel *ImageData::getImagePoints(QObject *parent_) const
{
    const_cast<ImageData *>(this)->initImagePointModel(parent_);
    return imagePointsModel;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// class GalleryEditViewImagesModel
///

static const QString g_noImageSource = "qrc:///resources/no-image.png";
static const ImageData g_noImageDefault = ImageData(-1, g_noImageSource);

/////////////////////////////////////////////////////////////////////////////////////////
/// \brief GalleryEditViewImagesModel::GalleryEditViewImagesModel
/// \param parent - parent QObject
///
GalleryImagesModel::GalleryImagesModel(QObject *parent /*= nullptr*/)
    :QAbstractListModel(parent)
{
    m_roleNames[IdRole] = "image_id";
    m_roleNames[ImageSourceRole] = "image_source";
    m_roleNames[ImagePointsRole] = "image_points";

    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)));

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
    case ImagePointsRole:
        if(m_showGalleryViewImages)
            return QVariant();
        else
            return QVariant::fromValue<QObject *>(
                        image.getImagePoints(
                            static_cast<QObject *>(
                                const_cast<GalleryImagesModel *>(this))
                            )
                        );
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

void GalleryImagesModel::onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(!(m_request == request_))
    {
        return;
    }
    m_request = nullptr;
    if(errorCode_ != 0) { return; }
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
    if(!(nullptr != NetAPI::getSingelton())) { return; }

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

const ImageData *GalleryImagesModel::dataItemAt(int index_) const
{
    if(index_ < 0 || index_ > m_images.size())
    {
        return nullptr;
    }
    return &(m_images.at(index_));
}

QString GalleryImagesModel::currentImageSource() const
{
    auto imgd = dataItemAt(currentIndex());
    if(nullptr == imgd)
    {
        return g_noImageSource;
    }
    return imgd->getSource();
}

QVariant GalleryImagesModel::currentImagePoints() const
{
    auto imgd = dataItemAt(currentIndex());
    if(nullptr == imgd)
    {
        return QVariant();
    }
    return QVariant::fromValue(imgd->getImagePoints(static_cast<QObject *>(const_cast<GalleryImagesModel *>(this))));
}

int GalleryImagesModel::currentIndex() const
{
    return m_currentIndex;
}

void GalleryImagesModel::setCurrentIndex(int index_)
{
    m_currentIndex = index_;

    emit currentIndexChanged();
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
            qreal pointsToPass_ /*= 1.0*/,
            bool ownGallery_ /*= false*/)
    : QObject(parent_),
    id(id_),
    description(description_),
    keywords(keywords_),
    created(created_),
    pointsToPass(pointsToPass_),
    m_ownGallery(ownGallery_)
{
}

GalleryItemData::~GalleryItemData()
{
    delete images;
    images = nullptr;
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

bool GalleryItemData::ownGallery() const
{
    return m_ownGallery;
}

void GalleryItemData::setId(int id_)
{
    id = id_;

    emit idChanged();
}

void GalleryItemData::setDescription(const QString& description_)
{
    description = description_;

    emit descriptionChanged();
}

void GalleryItemData::setKeywords(const QString& keywords_)
{
    keywords = keywords_;

    emit keywordsChanged();
}

void GalleryItemData::setCreated(const QDateTime& created_)
{
    created = created_;

    emit createdChanged();
}

void GalleryItemData::setPointsToPass(const QString &pointsToPass_)
{
    QVariant v(pointsToPass_);
    pointsToPass = v.toDouble();

    emit pointsToPassChanged();
}

void GalleryItemData::setOwnGallery(bool ownGallery_)
{
    m_ownGallery = ownGallery_;

    emit ownGalleryChanged();
}

template<typename JsonType>
GalleryItemData* GalleryItemData_fromJsonT(QObject *parent_, const JsonType &jsonValue_, bool &anyError)
{
    int id = -1;
    QString description;
    QString keywords;
    QDateTime created;
    qreal pointsToPass = 1.0;
    bool ownGallery = false;

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
        created = dateTimeFromJsonString(createdVal.toString());
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
    QJsonValue ownGalleryJV = jsonValue_["owner"];
    if(!ownGalleryJV.isUndefined()) { ownGallery = ownGalleryJV.toInt(0) != 0; } else { anyError = true; }

    return new GalleryItemData(parent_, id, description, keywords, created, pointsToPass, ownGallery);
}


GalleryItemData* GalleryItemData::fromJson(QObject *parent_, const QJsonDocument &jsonValue_, bool &anyError_)
{
    return GalleryItemData_fromJsonT(parent_, jsonValue_, anyError_);
}

GalleryItemData* GalleryItemData::fromJson(QObject *parent_, const QJsonValue &jsonValue_, bool &anyError_)
{
    return GalleryItemData_fromJsonT(parent_, jsonValue_, anyError_);
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
    m_roleNames[OwnRole] = "own_gallery";


    // TODO: indicate in view that there is no data
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)));
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
    case OwnRole:
        return item->ownGallery();
    }
    return QVariant();
}

bool GalleryEditViewModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    const int row = index.row();
    if(row < 0 || row >= m_data.size())
    {
        return false;
    }
    GalleryItemData *item = m_data[row];
    switch(role)
    {
    case GalleryIDRole:
        item->setId(value.toInt());
        break;
    case DescriptionRole:
        item->setDescription(value.toString());
        break;
    case CreatedRole:
        item->setCreated(value.toDateTime());
        break;
    case PointsToPassRole:
        item->setPointsToPass(value.toString());
        break;
    //case ImagesRole:
    //    item->setImagesModel(value);
    //    break;
    case OwnRole:
        return item->ownGallery();
    default:
        return false;
    }
    return true;
}

QHash<int, QByteArray> GalleryEditViewModel::roleNames() const
{
    return m_roleNames;
}

void GalleryEditViewModel::onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(!(m_request == request_) && !(static_cast<RequestData *>(m_refreshRequest) == request_))
    {
        return;
    }
    if(m_request == request_)
    {
        m_request = nullptr;
        if(errorCode_ != 0) { return; }
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
                delete data;
                data = nullptr;
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
    else if(static_cast<RequestData *>(m_refreshRequest) == request_)
    {
        m_refreshRequest = nullptr;
        if(errorCode_ != 0) { return; }
        Q_ASSERT(reply_.isObject());
        if(!(reply_.isObject()))
        {
            return;
        }
        bool anyError = false;
        GalleryItemData *data = GalleryItemData::fromJson(this, reply_, anyError);
        if(anyError)
        {
            delete data;
            data = nullptr;
            return;
        }
        int i0 = getIndexOfId(data->getId());
        auto fit = std::begin(m_data);
        std::advance(fit, i0);
        beginRemoveRows(QModelIndex(), i0, i0);
        delete m_data[i0];
        m_data[i0] = nullptr;
        m_data.erase(fit);
        endRemoveRows();
        beginInsertRows(QModelIndex(), i0, i0);
        m_data.insert(i0, data);
        endInsertRows();
    }
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
    if(!(nullptr != NetAPI::getSingelton())) { return; }

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

    m_currentIndex = getIndexOfId(id_);

    emit currentIndexChanged();
    emit currentIdChanged();
}

int GalleryEditViewModel::getIndexOfId(int id_) const
{
    auto fit = std::find_if(std::begin(m_data),
                            std::end(m_data),
                            [id_] (const GalleryItemData* item) -> bool
    {
       return nullptr != item && item->getId() == id_;
    });
    if(std::end(m_data) == fit)
    {
        return -1;
    }
    return std::distance(std::begin(m_data), fit);
}

void GalleryEditViewModel::refreshItemAtIndex(int index_)
{
    if(index_ < 0 || index_ >= m_data.size()) { return; }

    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }

    m_refreshRequest = netAPI->startJsonRequest();
    netAPI->get(QString("galleries/%1/").arg(m_data.at(index_)->getId()), m_refreshRequest);
}

QObject* GalleryEditViewModel::getCurrentItem()
{
    if(m_currentIndex < 0 || m_currentIndex >= m_data.size())
    {
        return nullptr;
    }
    return m_data.at(m_currentIndex);
}

ImagePointToNextImage::ImagePointToNextImage(QObject* parent_ /*= nullptr*/, int imagePointId_ /*= -1*/)
    :QObject(parent_),
      m_imagePointId(imagePointId_),
      m_imageSource(g_noImageSource),
      m_noImageSource(true)
{
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)));

    startLoad();
}

const QString &ImagePointToNextImage::imageSource()
{
    return m_imageSource;
}

void ImagePointToNextImage::setImageSource(const QString &imageSource_)
{
    m_imageSource = imageSource_;

    emit imageSourceChanged();
}

bool ImagePointToNextImage::noImageSource() const
{
    return m_noImageSource;
}

void ImagePointToNextImage::setNoImageSource(bool noImageSource_)
{
    m_noImageSource = noImageSource_;

    emit noImageSourceChanged();
}

void ImagePointToNextImage::startLoad()
{
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton())) { return; }

    m_request = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get(
                QString("image-point-to-next-image/%1/of_image_point/").arg(m_imagePointId),
                m_request);
}

void ImagePointToNextImage::onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(m_request != request_)
    {
        return;
    }
    m_request = nullptr;
    if(errorCode_ != 0) { return; }

    Q_ASSERT(reply_.isArray());

    QJsonValue item = reply_[0];
    if(item.isUndefined())
    {
        return;
    }
    QJsonValue nextImageJV = item["next_image"];
    if(!nextImageJV.isUndefined() && nextImageJV.isString())
    {
        m_imageSource = nextImageJV.toString();
        m_noImageSource = false;
    }

    emit imagePointToImageLoaded();
}


AnswerData::AnswerData(QObject *parent_ /*= nullptr*/,
                       int id_ /*= -1*/,
                       int questionId_ /*= -1*/,
                       const QString &answer_ /*= QString()*/,
                       qreal pointsToPass_ /*= 1.0*/)
    :QObject(parent_),
    m_id(id_),
    m_questionId(questionId_),
    m_answer(answer_),
    m_pointsToPass(pointsToPass_)
{
}

AnswerData::~AnswerData()
{
}

int AnswerData::id() const
{
    return m_id;
}

void AnswerData::setId(int id_)
{
    m_id = id_;

    emit idChanged();
}

int AnswerData::questionId() const
{
    return m_questionId;
}

void AnswerData::setQuestionId(int questionId_)
{
    m_questionId = questionId_;

    emit questionIdChanged();
}

const QString &AnswerData::answer() const
{
    return m_answer;
}

void AnswerData::setAnswer(const QString &answer_)
{
    m_answer = answer_;

    emit answerChanged();
}

qreal AnswerData::pointsToPass() const
{
    return m_pointsToPass;
}

void AnswerData::setPointsToPass(qreal pointsToPass_)
{
    m_pointsToPass = pointsToPass_;

    emit pointsToPassChanged();
}

AnswerData *AnswerData::fromJson(QObject *parent_, int questionId_, const QJsonValue &jsonValue_, bool &error_)
{
    error_ = false;
    if(jsonValue_.isUndefined())
    {
        error_ = true;
        return nullptr;
    }
    Q_ASSERT(jsonValue_.isObject());

    int id = -1;
    int questionId = -1;
    QString answer;
    qreal pointsToPass = 1.0;

    QJsonValue idJV = jsonValue_["id"];
    if(!idJV.isUndefined()) { id = idJV.toInt(-1); }
    else { error_ = true; }

    QJsonValue questionIdJV = jsonValue_["question"];
    if(!questionIdJV.isUndefined()) {questionId = questionIdJV.toInt(-1); }
    else { error_ = true; }
    Q_ASSERT(questionId_ != -1 && questionId == questionId_);

    QJsonValue answerJV = jsonValue_["answer"];
    if(!answerJV.isUndefined() && answerJV.isString()) { answer = answerJV.toString(); }
    else { error_ = true; }

    QJsonValue pointsToPassJV = jsonValue_["points"];
    if(!pointsToPassJV.isUndefined()) { pointsToPass = pointsToPassJV.toDouble(1.0); }
    else { error_ = true; }

    return new AnswerData(parent_, id, questionId, answer, pointsToPass);
}

QuestionAnswersModel::QuestionAnswersModel(QObject *parent_ /*= nullptr*/, int questionId_ /*= -1*/)
    :QAbstractListModel(parent_)
    ,m_questionId(questionId_)
{
    m_roleNames[IDRole] = "questionAnswer_id";
    m_roleNames[QuestionIDRole] = "questionAnswer_questionId";
    m_roleNames[AnswerRole] = "questionAnswer_text";
    m_roleNames[PointsToPassRole] = "questionAnswer_pointsToPass";

    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)));

    startLoad();
}

QuestionAnswersModel::~QuestionAnswersModel()
{
    clearData();
}

int QuestionAnswersModel::rowCount(const QModelIndex &parent_) const
{
    Q_UNUSED(parent_);

    return m_data.size();
}

QVariant QuestionAnswersModel::data(const QModelIndex &index_, int role_) const
{
    const int row = index_.row();
    if(row < 0 || row >= m_data.size())
    {
        return QVariant();
    }
    const AnswerData *item = m_data[row];
    switch(role_)
    {
    case IDRole:
        return item->id();
    case QuestionIDRole:
        return item->questionId();
    case AnswerRole:
        return item->answer();
    case PointsToPassRole:
        return item->pointsToPass();
    }
    return QVariant();
}

AnswerData *QuestionAnswersModel::dataItemAt(int index_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return nullptr;
    }
    return m_data[index_];
}

const AnswerData *QuestionAnswersModel::dataItemAt(int index_) const
{
    return const_cast<QuestionAnswersModel *>(this)->dataItemAt(index_);
}

QVariant QuestionAnswersModel::itemAt(int index_)
{
    return QVariant::fromValue(dataItemAt(index_));
}

int QuestionAnswersModel::questionId() const
{
    return m_questionId;
}

void QuestionAnswersModel::setQuestionId(int questionId_)
{
    m_questionId = questionId_;

    startLoad();

    emit questionIdChanged();
}

void QuestionAnswersModel::startLoad()
{
    if(questionId() < 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_data.size());
        clearData();
        endRemoveRows();
        return;
    }
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton())) { return; }
    m_request = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get(
                QString("image-question-answers/%1/of_question/").arg(questionId()),
                m_request);
}

void QuestionAnswersModel::clearData()
{
    for(auto *&p : m_data)
    {
        delete p;
        p = nullptr;
    }
    m_data.clear();
}

QHash<int, QByteArray> QuestionAnswersModel::roleNames() const
{
    return m_roleNames;
}

void QuestionAnswersModel::onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(!(m_request == request_))
    {
        return;
    }
    m_request = nullptr;
    if(errorCode_ != 0) { return; }
    Q_ASSERT(reply_.isArray());
    if(!(reply_.isArray()))
    {
        return;
    }
    QVector<AnswerData *> answers;
    for(int i = 0; ; i++)
    {
        QJsonValue val = reply_[i];
        if(QJsonValue::Undefined == val.type())
        {
            break;
        }
        bool anyError = false;
        AnswerData *data = AnswerData::fromJson(this, questionId(), val, anyError);
        if(anyError)
        {
            delete data;
            data = nullptr;
            continue;
        }
        answers.push_back(data);
    }
    beginRemoveRows(QModelIndex(), 0, m_data.size());
    clearData();
    endRemoveRows();
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + answers.size() - 1);
    std::copy(std::begin(answers), std::end(answers),
              std::inserter(m_data, std::end(m_data)));
    endInsertRows();

    emit answersLoaded();
}

QuestionData::QuestionData(QObject *parent_ /*= nullptr*/,
             int id_ /*= -1*/,
             const QString &questionText_ /*= QString()*/,
             qreal pointsToPass_ /*= 1.0*/)
    :QObject(parent_),
      m_id (id_),
      m_questionText(questionText_),
      m_pointsToPass(pointsToPass_)
{
}

int QuestionData::id() const
{
    return m_id;
}

void QuestionData::setId(int id_)
{
    m_id = id_;

    emit idChanged();
}

const QString &QuestionData::questionText() const
{
    return m_questionText;
}

void QuestionData::setQuestionText(const QString &questionText_)
{
    m_questionText = questionText_;

    emit questionTextChanged();
}

qreal QuestionData::pointsToPass() const
{
    return m_pointsToPass;
}

void QuestionData::setPointsToPass(qreal pointsToPass_)
{
    m_pointsToPass = pointsToPass_;

    emit pointsToPassChanged();
}

template<typename JsonObject>
QuestionData *QuestionData_fromJsonT(QObject *parent_, const JsonObject &jsonValue_, bool &error_)
{
    Q_ASSERT(jsonValue_.isObject());

    int id = -1;
    QJsonValue idJS = jsonValue_["id"];
    if(!idJS.isUndefined()) { id = idJS.toInt(-1); }
    else { error_ = true; }

    QString questionText;
    QJsonValue questionTextJV = jsonValue_["question"];
    if(!questionTextJV.isUndefined() && questionTextJV.isString()) { questionText = questionTextJV.toString(); }
    else { error_ = true; }

    qreal pointsToPass = 1.0;
    QJsonValue pointsToPassJV = jsonValue_["points_to_pass"];
    if(!pointsToPassJV.isUndefined()) { pointsToPass = pointsToPassJV.toDouble(1.0); }
    else { error_ = true; }

    return new QuestionData(parent_, id, questionText, pointsToPass);
}

QuestionData *QuestionData::fromJson(QObject *parent_, const QJsonDocument &jsonValue_, bool &error_)
{
    error_ = false;
    if(jsonValue_.isNull())
    {
        error_ = true;
        return nullptr;
    }
    return QuestionData_fromJsonT(parent_, jsonValue_, error_);
}

QuestionData *QuestionData::fromJson(QObject *parent_, const QJsonValue &jsonValue_, bool &error_)
{
    error_ = false;
    if(jsonValue_.isUndefined())
    {
        error_ = true;
        return nullptr;
    }
    return QuestionData_fromJsonT(parent_, jsonValue_, error_);
}

ImagePointToQuestion::ImagePointToQuestion(QObject *parent_ /*= nullptr*/, int imagePointId_ /*= -1*/)
    :QObject(parent_),
    m_imagePointId(imagePointId_)
{
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)));

    startLoad();
}

ImagePointToQuestion::~ImagePointToQuestion()
{
    delete m_question;
    m_question = nullptr;
}

int ImagePointToQuestion::questionId() const
{
    return m_questionId;
}

void ImagePointToQuestion::setQuestionId(int questionId_)
{
    m_questionId = questionId_;

    emit questionIdChanged();
}

QString ImagePointToQuestion::question() const
{
    return nullptr != m_question? m_question->questionText() : QString();
}

void ImagePointToQuestion::setQuestion(const QString &question_)
{
    if(nullptr == m_question)
    {
        return;
    }
    m_question->setQuestionText(question_);

    emit questionChanged();
}

qreal ImagePointToQuestion::pointsToPass() const
{
    return nullptr != m_question ? m_question->pointsToPass() : 1.0;
}

void ImagePointToQuestion::setPointsToPass(qreal pointsToPass_)
{
    if(nullptr == m_question)
    {
        return;
    }
    m_question->setPointsToPass(pointsToPass_);

    emit pointsToPassChanged();
}

QVariant ImagePointToQuestion::questionObj() const
{
    return QVariant::fromValue(const_cast<ImagePointToQuestion *>(this)->questionData());
}

QuestionData *ImagePointToQuestion::questionData()
{
    return m_question;
}

void ImagePointToQuestion::setQuestionObj(const QVariant &questionObj_)
{
    QObject *obj = qvariant_cast<QObject *>(questionObj_);
    QuestionData *newQuestion = qobject_cast<QuestionData *>(static_cast<QObject *>(obj));
    if(m_question == newQuestion)
    {
        return;
    }
    m_question = newQuestion;
    emit questionObjChanged();
}

void ImagePointToQuestion::refresh()
{
    if(nullptr != m_request1 || nullptr != m_request2)
    {
        m_forceRefresh = true;
        return;
    }

    m_forceRefresh = false;
    startLoad();
}

void ImagePointToQuestion::startLoad()
{
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton())) { return; }

    m_request1 = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get(
                QString("image-point-to-question/%1/of_image_point/").arg(m_imagePointId),
                m_request1);
}

void ImagePointToQuestion::startLoad2()
{
    if(questionId() < 0) { return; }
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton())) { return; }

    m_request2 = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get(
                QString("image-questions/%1/").arg(questionId()),
                m_request2);
}

bool ImagePointToQuestion::questionIdLoaded() const
{
    return nullptr == m_request1;
}

void ImagePointToQuestion::onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(request_ == nullptr || (m_request1 != request_ && m_request2 != request_))
    {
        return;
    }
    if(m_request1 == request_)
    {
        m_request1 = nullptr;
        if(errorCode_ != 0) { return; }

        Q_ASSERT(reply_.isArray());

        QJsonValue item = reply_[0];
        if(item.isUndefined())
        {
            return;
        }
        QJsonValue questionJV = item["question"];
        if(!questionJV.isUndefined())
        {
            m_questionId = questionJV.toInt(-1);
        }

        startLoad2();

        emit imagePointToQuestionLoaded();
        emit questionIdChanged();
    }
    else if(m_request2 == request_)
    {
        m_request2 = nullptr;
        if(errorCode_ != 0) { return; }

        bool anyError = false;
        m_question = QuestionData::fromJson(this, reply_, anyError);
        if(anyError || nullptr == m_question)
        {
            delete m_question;
            m_question = nullptr;
            return;
        }

        emit imagePointToQuestionTextLoaded();
        emit questionChanged();
        emit pointsToPassChanged();
        emit questionObjChanged();

        if(m_forceRefresh)
        {
            refresh();
        }
    }
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
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onDataSavedRequestFinished(int, RequestData *, const QJsonDocument &)));
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onDataCreatedRequestFinished(int, RequestData *, const QJsonDocument &)));
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onQuestionCreatedRequestFinished(int, RequestData *, const QJsonDocument &)));
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onAnswerCreatedRequestFinished(int, RequestData *, const QJsonDocument &)));
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onPointToQuestionRequestFinished(int, RequestData *, const QJsonDocument &)));
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onPointToImageRequestFinished(int, RequestData *, const QJsonDocument &)));
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onPointRemovedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onQuestionRemovedSlot(int, RequestData *, const QJsonDocument &)));

}

ImagePointData::~ImagePointData()
{
    delete m_imagePointToNextImage;
    m_imagePointToNextImage = nullptr;
    delete m_imagePointToQuestion;
    m_imagePointToQuestion = nullptr;
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

QVariant ImagePointData::toNextImage() const
{
    if(nullptr == m_imagePointToNextImage)
    {
        const_cast<ImagePointData *>(this)->m_imagePointToNextImage = new ImagePointToNextImage(
                    const_cast<ImagePointData *>(this), pointId());
    }
    return QVariant::fromValue(m_imagePointToNextImage);
}

void ImagePointData::setToNextImage(QVariant toNextImage_)
{
    QObject *obj = qvariant_cast<QObject *>(toNextImage_);
    ImagePointToNextImage *newNextImage = qobject_cast<ImagePointToNextImage *>(static_cast<QObject *>(obj));
    if(m_imagePointToNextImage == newNextImage)
    {
        return;
    }
    m_imagePointToNextImage = newNextImage;
    emit toNextImageChanged();
}

QVariant ImagePointData::toQuestion() const
{
    if(nullptr == m_imagePointToQuestion)
    {
        const_cast<ImagePointData *>(this)->m_imagePointToQuestion = new ImagePointToQuestion(
                    const_cast<ImagePointData *>(this), pointId());
    }
    return QVariant::fromValue(m_imagePointToQuestion);
}

void ImagePointData::setToQuestion(QVariant toQuestion_)
{
    QObject *obj = qvariant_cast<QObject *>(toQuestion_);
    ImagePointToQuestion *newQuestion = qobject_cast<ImagePointToQuestion *>(static_cast<QObject *>(obj));
    if(m_imagePointToQuestion == newQuestion)
    {
        return;
    }
    m_imagePointToQuestion = newQuestion;
    emit toQuestionChanged();
}

ImagePointToQuestion *ImagePointData::getQuestion()
{
    return m_imagePointToQuestion;
}

ImagePointData *ImagePointData::fromJson(QObject *parent_, int sourceImageId_, const QJsonValue& jsonObject_, bool& error_)
{
    const bool chk001 = jsonObject_.isObject();
    Q_ASSERT(chk001);
    if(!chk001)
    {
        error_ = true;
        return nullptr;
    }

    error_ = false;

    int id = -1;
    qreal x = 0.5;
    qreal y = 0.5;
    qreal weight = 1.0;

    QJsonValue idVal = jsonObject_["id"];
    if(idVal.type() != QJsonValue::Undefined)
    {
        id = idVal.toInt(-1);
    }
    else
    {
        error_ = true;
    }

    QJsonValue xVal = jsonObject_["x"];
    if(xVal.type() != QJsonValue::Undefined && xVal.isDouble())
    {
        x = xVal.toDouble(0.5);
    }
    else
    {
        error_ = true;
    }

    QJsonValue yVal = jsonObject_["y"];
    if(yVal.type() != QJsonValue::Undefined && yVal.isDouble())
    {
        y = yVal.toDouble(0.5);
    }
    else
    {
        error_ = true;
    }

    QJsonValue weightVal = jsonObject_["weight"];
    if(weightVal.type() != QJsonValue::Undefined && weightVal.isDouble())
    {
        weight = weightVal.toDouble(0.5);
    }
    else
    {
        error_ = true;
    }
    return new ImagePointData(parent_, sourceImageId_, id, x, y, weight);
}

void ImagePointData::saveData()
{
    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_saveDataRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(pointId()));
    rec.insert("image", QJsonValue::fromVariant(getSourceImageId()));
    rec.insert("x", QJsonValue::fromVariant(xCoord()));
    rec.insert("y", QJsonValue::fromVariant(yCoord()));
    rec.insert("weight", QJsonValue::fromVariant(weight()));
    QJsonDocument doc(rec);

    m_saveDataRequest->setDocument(doc);
    NetAPI::getSingelton()->patch(QString("image-point/%1/").arg(pointId()), m_saveDataRequest);
}

void ImagePointData::onDataSavedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);

    if(nullptr == m_saveDataRequest || static_cast<RequestData *>(m_saveDataRequest) != request_) { return; }
    m_saveDataRequest = nullptr;
    if(0 != errorCode_) { return; }

    emit onDataSaved(pointId());
}

void ImagePointData::createData(bool pointToNextImage_)
{
    Q_ASSERT(pointId() < 0);
    m_pointToNextImage = pointToNextImage_;

    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_createDataRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("image", QJsonValue::fromVariant(getSourceImageId()));
    rec.insert("x", QJsonValue::fromVariant(xCoord()));
    rec.insert("y", QJsonValue::fromVariant(yCoord()));
    rec.insert("weight", QJsonValue::fromVariant(weight()));
    QJsonDocument doc(rec);

    m_createDataRequest->setDocument(doc);
    NetAPI::getSingelton()->post(QString("image-point/"), m_createDataRequest);
}

void ImagePointData::onDataCreatedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(nullptr == m_createDataRequest || static_cast<RequestData *>(m_createDataRequest) != request_) { return; }
    m_createDataRequest = nullptr;
    if(0 != errorCode_) { return; }

    Q_ASSERT(reply_.isObject());

    QJsonValue idJV = reply_["id"];
    if(!idJV.isUndefined()) { setPointId(idJV.toInt(-1)); }

    emit onDataCreated(pointId());
}

void ImagePointData::createTemplateData()
{
    if(m_pointToNextImage)
    {
        createTemplImageData();
    }
    else
    {
        cretateTemplQuestionData();
    }
}

void ImagePointData::createTemplImageData()
{
    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_createImagePointToImageRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("image_point", QJsonValue::fromVariant(pointId()));
    rec.insert("next_image", QJsonValue::fromVariant(getSourceImageId())); // set next image to itself
    QJsonDocument doc(rec);

    m_createImagePointToImageRequest->setDocument(doc);
    NetAPI::getSingelton()->post(QString("image-point-to-next-image/"), m_createImagePointToImageRequest);
}

void ImagePointData::onPointToImageRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);

    if(nullptr == m_createImagePointToImageRequest || static_cast<RequestData *>(m_createImagePointToImageRequest) != request_) { return; }
    m_createImagePointToImageRequest = nullptr;
    if(0 != errorCode_) { return; }

    if(nullptr != m_imagePointToNextImage)
    {
        m_imagePointToNextImage->startLoad();
    }

    emit onTemplateDataCreated(pointId());
}

void ImagePointData::cretateTemplQuestionData()
{
    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_questionID = -1;

    m_createQuestionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("question", QJsonValue::fromVariant("?"));           // template data
    rec.insert("points_to_pass", QJsonValue::fromVariant(1.0));     // template data
    QJsonDocument doc(rec);

    m_createQuestionRequest->setDocument(doc);
    NetAPI::getSingelton()->post(QString("image-questions/"), m_createQuestionRequest);
}

void ImagePointData::onQuestionCreatedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(nullptr == m_createQuestionRequest || static_cast<RequestData *>(m_createQuestionRequest) != request_) { return; }
    m_createQuestionRequest = nullptr;
    if(0 != errorCode_) { return; }

    Q_ASSERT(reply_.isObject());

    QJsonValue idJV = reply_["id"];
    if(!idJV.isUndefined()) { m_questionID = idJV.toInt(-1); }

    if(m_questionID <= 0) { return; }  // TODO: error processing

    Q_ASSERT(m_questionID > 0);

    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_createAnswerRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("question", QJsonValue::fromVariant(m_questionID));
    rec.insert("answer", QJsonValue::fromVariant("yes"));       // template data
    rec.insert("points", QJsonValue::fromVariant(1.0));         // template data
    QJsonDocument doc(rec);

    m_createAnswerRequest->setDocument(doc);
    NetAPI::getSingelton()->post(QString("image-question-answers/"), m_createAnswerRequest);
}

void ImagePointData::onAnswerCreatedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);

    if(nullptr == m_createAnswerRequest || static_cast<RequestData *>(m_createAnswerRequest) != request_) { return; }
    m_createAnswerRequest = nullptr;
    if(0 != errorCode_) { return; }

    Q_ASSERT(m_questionID > 0);

    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_createImagePointToQuestionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("image_point", QJsonValue::fromVariant(pointId()));
    rec.insert("question", QJsonValue::fromVariant(m_questionID));       // template data
    QJsonDocument doc(rec);

    m_createImagePointToQuestionRequest->setDocument(doc);
    NetAPI::getSingelton()->post(QString("image-point-to-question/"), m_createImagePointToQuestionRequest);
}

void ImagePointData::onPointToQuestionRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);

    if(nullptr == m_createImagePointToQuestionRequest || static_cast<RequestData *>(m_createImagePointToQuestionRequest) != request_) { return; }
    m_createImagePointToQuestionRequest = nullptr;
    if(0 != errorCode_) { return; }

    if(nullptr != m_imagePointToQuestion)
    {
        m_imagePointToQuestion->setQuestionId(m_questionID);
        m_imagePointToQuestion->refresh();
    }

    emit onTemplateDataCreated(pointId());
}

void ImagePointData::removePoint()
{
    (void)toQuestion();
    removePoint(true);
}

void ImagePointData::removePoint(bool tryToLoad_)
{
    if(nullptr != m_imagePointToQuestion)
    {
        if(m_imagePointToQuestion->questionIdLoaded())
        {
            m_questionID = m_imagePointToQuestion->questionId();
            if(m_questionID >= 0)
            {
                removeQuestion();
            }
            else
            {
                removeImagePoint();
            }
        }
        else if(tryToLoad_)
        {
            QObject::connect(m_imagePointToQuestion, SIGNAL(imagePointToQuestionLoaded()), this, SLOT(imagePointToQuestionLoadedSlot()));
        }
    }
    else
    {
        removeImagePoint();
    }
}

void ImagePointData::imagePointToQuestionLoadedSlot()
{
    QObject::disconnect(m_imagePointToQuestion, SIGNAL(imagePointToQuestionLoaded()), this, SLOT(imagePointToQuestionLoadedSlot()));
    Q_ASSERT(nullptr != m_imagePointToQuestion && m_imagePointToQuestion->questionIdLoaded());
    removePoint(false);
}

void ImagePointData::removeQuestion()
{
    Q_ASSERT(m_questionID >= 0);
    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_removeQuestionRequest = netAPI->startJsonRequest();
    NetAPI::getSingelton()->del(QString("image-questions/%1/").arg(m_questionID), m_removeQuestionRequest);
}

void ImagePointData::onQuestionRemovedSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);

    if(nullptr == m_removeQuestionRequest || static_cast<RequestData *>(m_removeQuestionRequest) != request_) { return; }
    m_removeQuestionRequest = nullptr;
    if(0 != errorCode_) { return; }

    removeImagePoint();
}

void ImagePointData::removeImagePoint()
{
    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_removeImagePointRequest = netAPI->startJsonRequest();
    NetAPI::getSingelton()->del(QString("image-point/%1/").arg(pointId()), m_removeImagePointRequest);
}

void ImagePointData::onPointRemovedSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);

    if(nullptr == m_removeImagePointRequest || static_cast<RequestData *>(m_removeImagePointRequest) != request_) { return; }
    m_removeImagePointRequest = nullptr;
    if(0 != errorCode_) { return; }

    emit onPointRemoved();
}

ImagePointsModel::ImagePointsModel(QObject *parent_)
    :QAbstractListModel(parent_)
{
    m_roleNames[IDRole] = "imagePoint_id";
    m_roleNames[XRole] = "imagePoint_x";
    m_roleNames[YRole] = "imagePoint_y";
    m_roleNames[WeightRole] = "imagePoint_weight";
    m_roleNames[ToNextImageRole] = "imagePoint_toNextImage";
    m_roleNames[ToQuestionRole] = "imagePoint_toQuestion";

    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)));
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
    case ToNextImageRole:
        return item->toNextImage();
    case ToQuestionRole:
        return item->toQuestion();
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
    case ToNextImageRole:
        item->setToNextImage(value);
        break;
    case ToQuestionRole:
        item->setToQuestion(value);
        break;
    default:
        return false;
    }
    emit dataChanged(index, index, { role } );
    return true;
}

qreal clamp(qreal v_, qreal min_, qreal max_)
{
    return std::min(max_, std::max(min_, v_));
}

static const qreal g_coordMin = 0.0;
static const qreal g_coordMax = 1.0;
static const qreal g_weightMin = 0.125;
static const qreal g_weightMax = 10.0;

void ImagePointsModel::setCurrentPointParams(int index_, qreal x_, qreal y_, qreal weight_)
{
    auto i = index(index_);
    setData(i, QVariant::fromValue(clamp(x_, g_coordMin, g_coordMax)), XRole);
    setData(i, QVariant::fromValue(clamp(y_, g_coordMin, g_coordMax)), YRole);
    setData(i, QVariant::fromValue(clamp(weight_, g_weightMin, g_weightMax)), WeightRole);

    refreshItems(index_);
}

int ImagePointsModel::addTempPoint(qreal x_, qreal y_, qreal weight_)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(new ImagePointData(this, m_sourceImageId, -1, x_, y_, weight_));
    endInsertRows();
    emit imagePointsCountChanged();
    refreshItems(-1);
    return m_data.size() - 1;
}

void ImagePointsModel::removeTempPoint(int index_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return;
    }
    auto fit = std::begin(m_data);
    std::advance(fit, index_);
    Q_ASSERT(nullptr != *fit && (*fit)->pointId() < 0);
    beginRemoveRows(QModelIndex(), index_, index_);
    delete *fit;
    *fit = nullptr;
    m_data.erase(fit);
    endRemoveRows();
    emit imagePointsCountChanged();
    refreshItems(-1);

    emit onDataSaved();
}

void ImagePointsModel::resetValuesAtIndex(int index_, qreal x_, qreal y_, qreal weight_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return;
    }
    auto fit = std::begin(m_data);
    std::advance(fit, index_);
    Q_ASSERT(nullptr != *fit && (*fit)->pointId() >= 0);
    (*fit)->setXCoord(x_);
    (*fit)->setYCoord(y_);
    (*fit)->setWeight(weight_);

    emit onDataSaved();
    refreshItems(index_);
}

void ImagePointsModel::savePointTemplate(int index_,bool pointToNextImage_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return;
    }
    auto fit = std::begin(m_data);
    std::advance(fit, index_);
    Q_ASSERT(nullptr != *fit && (*fit)->pointId() < 0);
    QObject::connect(*fit, SIGNAL(onDataCreated(int)), this, SLOT(onImagePointDataCreated1Slot(int)));
    (*fit)->createData(pointToNextImage_);
}

void ImagePointsModel::onImagePointDataCreated1Slot(int pointId_)
{
    auto *item = getById(pointId_);
    if(nullptr == item)
    {
        return;
    }
    QObject::disconnect(item, SIGNAL(onDataCreated(int)), this, SLOT(onImagePointDataCreated1Slot(int)));

    QObject::connect(item, SIGNAL(onTemplateDataCreated(int)), this, SLOT(onImagePointDataCreated2Slot(int)));
    item->createTemplateData();
}

void ImagePointsModel::onImagePointDataCreated2Slot(int pointId_)
{
    auto *item = getById(pointId_);
    if(nullptr == item)
    {
        return;
    }
    QObject::disconnect(item, SIGNAL(onTemplateDataCreated(int)), this, SLOT(onImagePointDataCreated2Slot(int)));

    emit onDataSaved();
    refreshItems(getIndexById(pointId_));
}

void ImagePointsModel::savePoint(int index_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return;
    }
    auto fit = std::begin(m_data);
    std::advance(fit, index_);
    Q_ASSERT(nullptr != *fit && (*fit)->pointId() >= 0);
    QObject::connect(*fit, SIGNAL(onDataSaved(int)), this, SLOT(onImagePointDataSavedSlot(int)));
    (*fit)->saveData();
}

void ImagePointsModel::onImagePointDataSavedSlot(int pointId_)
{
    auto *item = getById(pointId_);
    if(nullptr == item)
    {
        return;
    }
    QObject::disconnect(item, SIGNAL(onDataSaved(int)), this, SLOT(onImagePointDataSavedSlot(int)));

    emit onDataSaved();
    refreshItems(getIndexById(pointId_));
}

int ImagePointsModel::getSize() const
{
    return m_data.size();
}

void ImagePointsModel::refreshItems(int index_)
{
    if(index_ >= 0)
    {
        auto i = index(index_);
        emit dataChanged(i, i, { XRole, YRole, WeightRole } );
        emit onImagePointsRefreshed();
    }
    else
    {
        emit imagePointsLoaded();
    }
}

void ImagePointsModel::setSourceImageId(int sourceImageId_)
{
    m_sourceImageId = sourceImageId_;
}

void ImagePointsModel::startLoadImagePoints()
{
    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI)) { return; }

    m_request = netAPI->startRequest();
    NetAPI::getSingelton()->get(QString("image-point/%1/of_image/").arg(m_sourceImageId), m_request);
}

void ImagePointsModel::onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);
    if(nullptr == m_request || m_request != request_)
    {
        return;
    }
    m_request = nullptr;
    if(errorCode_ != 0) { return; }

    Q_ASSERT(reply_.isArray());
    if(!(reply_.isArray()))
    {
        return;
    }
    QVector<ImagePointData*> imagePoints;
    for(int i = 0; ; i++)
    {
        QJsonValue val = reply_[i];
        if(QJsonValue::Undefined == val.type())
        {
            break;
        }
        bool anyError = false;
        ImagePointData *data = ImagePointData::fromJson(this, m_sourceImageId, val, anyError);
        if(anyError)
        {
            continue;
        }
        imagePoints.push_back(data);
    }
    beginRemoveRows(QModelIndex(), 0, m_data.size());
    clearData();
    endRemoveRows();
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + imagePoints.size() - 1);
    std::copy(std::begin(imagePoints), std::end(imagePoints),
              std::inserter(m_data, std::end(m_data)));
    endInsertRows();

    emit imagePointsLoaded();
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

ImagePointData *ImagePointsModel::getAt(int index_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return nullptr;
    }
    return m_data.at(index_);
}

int ImagePointsModel::getIndexById(int pointId_) const
{
    auto fit = std::find_if(std::begin(m_data), std::end(m_data),
                            [pointId_](ImagePointData *item_)->bool
    {
        return nullptr != item_ && item_->pointId() == pointId_;
    });
    if(std::end(m_data) == fit)
    {
        return -1;
    }
    return std::distance(std::begin(m_data), fit);
}

ImagePointData *ImagePointsModel::getById(int pointId_)
{
    return getAt(getIndexById(pointId_));
}

bool ImagePointsModel::isEmpty() const
{
    return m_data.empty();
}

QVariant ImagePointsModel::itemAt(int index_)
{
    return QVariant::fromValue(getAt(index_));
}

struct VoronoyMaterial
{
    QVector<QVector2D> points;
    QVector<GLfloat> weights;
};

class VoronoyShader : public QSGSimpleMaterialShader<VoronoyMaterial>
{
private:
    using base = QSGSimpleMaterialShader<VoronoyMaterial>;

protected:
    template<int pointsCount>
    static QSGMaterialShader *createShader()
    {
        return new VoronoyShader(pointsCount);
    }

    template<int pointsCount, int index>
    class FuncInitializer
    {
    public:
        static void initCreateShaderFuncs(QSGMaterialShader* (**createShaderFuncs)(void))
        {
            createShaderFuncs[index - 1] = &VoronoyShader::createShader<index - 1>;
            FuncInitializer<pointsCount, index - 1>::initCreateShaderFuncs(createShaderFuncs);
        }
    };

    template<int pointsCount>
    class FuncInitializer<pointsCount, 0>
    {
    public:
        static void initCreateShaderFuncs(QSGMaterialShader* (**createShaderFuncs)(void))
        {
            Q_UNUSED(createShaderFuncs);
        }
    };


public:
    static QSGSimpleMaterial<VoronoyMaterial> *createMaterial(int pointsCount)
    {
        static QSGMaterialShader* (*createShaderFuncs[128])(void);
        FuncInitializer<128, 128>::initCreateShaderFuncs(createShaderFuncs);
        return new QSGSimpleMaterial<VoronoyMaterial>(createShaderFuncs[pointsCount]);
    }

public:
    VoronoyShader(int pointsCount)
    {
        m_vertexShader =
            "uniform highp mat4 qt_Matrix;\n"
            "attribute highp vec2 vertices;\n"
            "attribute highp vec3 color;\n"
            "varying highp vec3 fragColor;\n"
            "varying highp vec2 fragCoord;\n"
            "void main() {\n"
            "    fragCoord = vertices;\n"
            "    gl_Position = qt_Matrix * vec4(vertices, 0.0, 1.0);\n"
            "    fragColor = color;\n"
            "}\n";
        m_fragmentShader = (pointsCount >= 2) ? QString(
            "uniform lowp float qt_Opacity;\n"
            "uniform highp vec2 seeds[%1];\n"
            "uniform highp float weights[%1];\n"
            "varying highp vec3 fragColor;\n"
            "varying highp vec2 fragCoord;\n"
            "void main() {\n"
            "    highp float dist0 = distance(seeds[0], fragCoord) * weights[0];\n"
            "    highp float dist = distance(seeds[1], fragCoord) * weights[1];\n"
            "    highp float dist1 = dist;\n"
            "    if(dist < dist0) {"
            "        dist1 = dist0;\n"
            "        dist0 = dist;\n"
            "    }\n"
            "    for(int i = 2; i < %1; i++) {\n"
            "        float highp dist = distance(seeds[i], fragCoord) * weights[i];\n"
            "        if(dist < dist0) {\n"
            "            dist1 = dist0;\n"
            "            dist0 = dist;\n"
            "        } else if(dist < dist1) {\n"
            "            dist1 = dist;\n"
            "        }\n"
            "    }\n"
            "    highp float c = 2.0/(1.0 + exp(0.5*abs(dist0 - dist1)));\n"
            "    gl_FragColor = qt_Opacity * vec4(c * fragColor.rgb, clamp(c*1.5, 0.0, 1.0));\n"
            "}\n").arg(pointsCount)
            : QString(
            "uniform lowp float qt_Opacity;\n"
            "varying highp vec3 fragColor;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = qt_Opacity * vec4(fragColor.rgb, 1.0);\n"
            "}\n");

        m_vertexShaderBA = m_vertexShader.toLocal8Bit();
        m_fragmentShaderBA = m_fragmentShader.toLocal8Bit();
    }

    QList<QByteArray> attributes() const override {  return QList<QByteArray>() << "vertices" << "color"; }

    virtual const char *vertexShader() const override
    {
        return m_vertexShaderBA.data();
    }

    virtual const char *fragmentShader() const override
    {
        return m_fragmentShaderBA.data();
    }

    /*virtual void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override
    {
        base::updateState(state, newMaterial, oldMaterial);
    }*/

    void updateState(const VoronoyMaterial *m, const VoronoyMaterial *) override
    {
        program()->setUniformValueArray(m_id_seeds, &(m->points[0]), m->points.size());
        program()->setUniformValueArray(m_id_weights, &(m->weights[0]), m->weights.size(), 1);
    }

    void resolveUniforms() override {
        m_id_seeds = program()->uniformLocation("seeds");
        m_id_weights = program()->uniformLocation("weights");
    }

private:
    int m_id_seeds = -1;
    int m_id_weights = -1;
    QString m_vertexShader;
    QByteArray m_vertexShaderBA;
    QString m_fragmentShader;
    QByteArray m_fragmentShaderBA;
};

struct VoronoyVertex {
    float x;
    float y;
    float r;
    float g;
    float b;
    inline void set(float x_, float y_, float r_, float g_, float b_) { x = x_; y = y_; r = r_; g = g_; b = b_; }
};

static const QSGGeometry::AttributeSet &voronoy_attributes()
{
    static QSGGeometry::Attribute attr[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 3, GL_FLOAT)
    };
    static QSGGeometry::AttributeSet set = { 2, 5 * sizeof(float), attr };
    return set;
}

VoronoyNode::VoronoyNode(const QVector<QVector2D> &points_, const QVector<qreal> &weights_, const QRectF &rect_)
    : m_geometry(voronoy_attributes(), 0)
{
    setGeometry(&m_geometry);
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);

    Q_ASSERT(points_.size() == weights_.size());
    QSGSimpleMaterial<VoronoyMaterial> *m = VoronoyShader::createMaterial(points_.size());
    m->state()->points.reserve(points_.size());
    m->state()->points.clear();
    for(const auto &pt : points_)
    {
        m->state()->points.push_back(QVector2D(rect_.x() + pt.x() * rect_.width(), rect_.y() + pt.y() * rect_.height()));
    }
    m->state()->weights.reserve(weights_.size());
    m->state()->weights.clear();
    for(const auto &w : weights_)
    {
        m->state()->weights.push_back(w);
    }
    m->setFlag(QSGMaterial::Blending);
    setMaterial(m);
    //setFlag(OwnsMaterial);
}

void VoronoyNode::updateGeometry(const QRectF &bounds_, const QVector<QVector2D> &points_, const QVector<qreal> &weights_, const QVector3D &color_)
{
    m_geometry.allocate(4);

    float x = bounds_.x();
    float y = bounds_.y();
    float w = bounds_.width();
    float h = bounds_.height();

    QSGSimpleMaterial<VoronoyMaterial> *m = static_cast<QSGSimpleMaterial<VoronoyMaterial> *>(material());
    Q_ASSERT(m->state()->points.size() == points_.size() && m->state()->weights.size() == weights_.size() && points_.size() == weights_.size());
    for(int i = 0; i < points_.size(); i++)
    {
        m->state()->points[i].setX(x + points_[i].x() * w);
        m->state()->points[i].setY(y + points_[i].y() * h);
        m->state()->weights[i] = weights_[i];
    }
    VoronoyVertex *v = reinterpret_cast<VoronoyVertex *>(m_geometry.vertexData());
    v[0].set(x, y, color_.x(), color_.y(), color_.z());
    v[1].set(x+w, y, color_.x(), color_.y(), color_.z());
    v[2].set(x, y+h, color_.x(), color_.y(), color_.z());
    v[3].set(x+w, y+h, color_.x(), color_.y(), color_.z());

    markDirty(QSGNode::DirtyForceUpdate | QSGNode::DirtyMaterial | QSGNode::DirtyGeometry);
}


VoronoyGraphItem::VoronoyGraphItem(QQuickItem *parent_ /*= nullptr*/)
    :QQuickItem(parent_)
{
    setFlag(ItemHasContents, true);
}

VoronoyGraphItem::~VoronoyGraphItem()
{
}

QVariant VoronoyGraphItem::model()
{
    return QVariant::fromValue(m_model);
}

void VoronoyGraphItem::setModel(QVariant model_)
{
    if(nullptr != m_model)
    {
        QObject::disconnect(m_model, SIGNAL(imagePointsLoaded()), this, SLOT(imagePointsLoadedSlot()));
        QObject::disconnect(m_model, SIGNAL(onImagePointsRefreshed()), this, SLOT(onImagePointsRefreshedSlot()));
    }

    QObject *obj = qvariant_cast<QObject *>(model_);
    ImagePointsModel *newModel = qobject_cast<ImagePointsModel *>(static_cast<QObject *>(obj));
    if(newModel == m_model)
    {
        return;
    }
    m_model = newModel;
    if(nullptr != m_model)
    {
        QObject::connect(m_model, SIGNAL(imagePointsLoaded()), this, SLOT(imagePointsLoadedSlot()));
        QObject::connect(m_model, SIGNAL(onImagePointsRefreshed()), this, SLOT(onImagePointsRefreshedSlot()));
        setPointsFromModel();
    }
    emit modelChanged();
}

QVariant VoronoyGraphItem::color()
{
    return QVariant::fromValue(m_color);
}

void VoronoyGraphItem::setColor(QVariant color_)
{
    m_color = QColor(color_.toString());
    emit colorChanged();
}

int VoronoyGraphItem::pointsCount() const
{
    return m_points.size();
}

void VoronoyGraphItem::addPoint(qreal x_, qreal y_, qreal weight_)
{
    m_points.push_back(QVector2D(x_,y_));
    m_weights.push_back(weight_);
    m_pointsChanged = true;
    update();
}

void VoronoyGraphItem::setPoint(int index_, qreal x_, qreal y_, qreal weight_)
{
    if(index_ < 0 || index_ >= m_points.size() || index_ >= m_weights.size())
    {
        return;
    }
    Q_ASSERT(m_points.size() == m_weights.size());
    m_points[index_].setX(x_);
    m_points[index_].setY(y_);
    m_weights[index_] = weight_;
    m_pointsChanged = true;
    update();
}

void VoronoyGraphItem::removePointAt(int index_)
{
    if(index_ < 0 || index_ >= m_points.size() || index_ >= m_weights.size())
    {
        return;
    }
    Q_ASSERT(m_points.size() == m_weights.size());
    auto fit = std::begin(m_points);
    auto fit1 = std::begin(m_weights);
    std::advance(fit, index_);
    std::advance(fit1, index_);
    m_points.erase(fit);
    m_weights.erase(fit1);
    m_pointsChanged = true;
    update();
}

void VoronoyGraphItem::resetPoints()
{
    setPointsFromModel();
}

void VoronoyGraphItem::modelUpdated()
{
    setPointsFromModel();
    m_pointsChanged = true;
    update();
}

void VoronoyGraphItem::imagePointsLoadedSlot()
{
    modelUpdated();
}

void VoronoyGraphItem::onImagePointsRefreshedSlot()
{
    modelUpdated();
}

class VoronoyGraphNode : public QSGNode
{
public:
    virtual ~VoronoyGraphNode() override
    {
        delete item;
        item = nullptr;
    }

    VoronoyNode *item;
};


QSGNode *VoronoyGraphItem::updatePaintNode(QSGNode *oldNode_, UpdatePaintNodeData *upnd_)
{
    Q_UNUSED(upnd_);

    VoronoyGraphNode *n= static_cast<VoronoyGraphNode *>(oldNode_);
    QRectF rect = boundingRect();

    if (rect.isEmpty()) {
        delete n;
        return nullptr;
    }

    if (!n || m_oldPointsCount < 0 || m_oldPointsCount != m_points.size()) {
        delete n;
        n = nullptr;
        n = new VoronoyGraphNode();

        n->item = new VoronoyNode(m_points,  m_weights,  rect);
        n->appendChildNode(n->item);

        m_oldPointsCount = m_points.size();
    }

    if (m_geometryChanged)
    {
    }

    if (m_geometryChanged || m_pointsChanged) {
        n->item->updateGeometry(rect, m_points, m_weights, QVector3D(m_color.redF(), m_color.greenF(), m_color.blueF()));
    }

    m_geometryChanged = false;
    m_pointsChanged = false;

    return n;
}

void VoronoyGraphItem::geometryChanged(const QRectF &newGeometry_, const QRectF &oldGeometry_)
{
    m_geometryChanged = true;
    update();
    QQuickItem::geometryChanged(newGeometry_, oldGeometry_);
}

void VoronoyGraphItem::setPointsFromModel()
{
    if(nullptr == m_model)
    {
        m_points.clear();
        m_weights.clear();
        update();
        return;
    }
    Q_ASSERT(nullptr != m_model);
    const int imagePointsCnt = nullptr != m_model ? m_model->rowCount(QModelIndex()) : 0;
    m_points.clear();
    m_weights.clear();
    for(int i = 0; i < imagePointsCnt; i++)
    {
        const bool chk001 = nullptr != m_model && nullptr != m_model->getAt(i);
        Q_ASSERT(chk001);
        if(!chk001)
        {
            continue;
        }
        qreal x = m_model->getAt(i)->xCoord();
        qreal y = m_model->getAt(i)->yCoord();
        m_points.push_back(QVector2D(x,y));
        m_weights.push_back(m_model->getAt(i)->weight());
    }
    m_pointsChanged = true;
    update();
}

DescriptionData::DescriptionData(QObject * parent_ /*= nullptr*/,
                                 int id_ /*= -1*/,
                                 int imageId_ /*= -1*/,
                                 const QDateTime &fromDateTime_ /*= QDateTime()*/,
                                 const QString &descriptionStr_ /*= QString()*/
        )
    :QObject(parent_),
      m_id(id_),
      m_imageId(imageId_),
      m_fromDateTime(fromDateTime_),
      m_descriptionStr(descriptionStr_)
{
}

DescriptionData* DescriptionData::fromJson(QObject *parent_, int imageId_, const QJsonValue &value_, bool &error_)
{
    int id = -1;
    int imageId = imageId_;
    QDateTime fromDateTime;
    QString descriptionStr;

    Q_ASSERT(value_.isObject());

    // id
    QJsonValue idJV = value_["id"];
    if(!idJV.isUndefined()) { id = idJV.toInt(-1); }
    else { error_ |= true; }

    // imageId
    QJsonValue imageIdJV = value_["image"];
    if(!imageIdJV.isUndefined()) { imageId = imageIdJV.toInt(-1); }
    else { error_ |= true; }
    Q_ASSERT(imageId_ == -1 || imageId_ == imageId);

    // fromDateTime
    QJsonValue fromDateTimeJV = value_["from_field"];
    if(!fromDateTimeJV.isUndefined()) { fromDateTime = dateTimeFromJsonString(fromDateTimeJV.toString()); }
    else { error_ |= true; }

    // descriptionStr
    QJsonValue descriptionStrJV = value_["descr"];
    if(!descriptionStrJV.isUndefined() && descriptionStrJV.isString()) {
        descriptionStr = descriptionStrJV.toString();
    } else { error_ |= true; }

    return new DescriptionData(parent_, id, imageId, fromDateTime, descriptionStr);
}

int DescriptionData::id() const
{
    return m_id;
}

void DescriptionData::setId(int id_)
{
    m_id = id_;

    emit idChanged();
}

int DescriptionData::imageId() const
{
    return m_imageId;
}

void DescriptionData::setImageId(int imageId_)
{
    m_imageId = imageId_;

    emit imageIdChanged();
}

const QDateTime &DescriptionData::fromDateTime() const
{
    return m_fromDateTime;
}

void DescriptionData::setFromDateTime(const QDateTime &fromDateTime_)
{
    m_fromDateTime = fromDateTime_;

    emit fromDateTimeChanged();
}

const QString &DescriptionData::descriptionStr() const
{
    return m_descriptionStr;
}

void DescriptionData::setDescriptionStr(const QString &descriptionStr_)
{
    m_descriptionStr = descriptionStr_;

    emit descriptionStrChanged();
}

DescriptionModel::DescriptionModel(QObject *parent_ /*= nullptr*/)
    :QAbstractListModel(parent_)
{
    m_roleNames[IDRole] = "description_id";
    m_roleNames[ImageIDRole] = "description_imageId";
    m_roleNames[FromDateTimeRole] = "description_fromDateTime";
    m_roleNames[DescriptionRole] = "description_description";

    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(loadDescriptionsJsonRequestFinished(int, RequestData *, const QJsonDocument &)));
}

DescriptionModel::~DescriptionModel()
{
    clearData();
}

int DescriptionModel::rowCount(const QModelIndex &parent_) const
{
    Q_UNUSED(parent_);
    return m_data.size();
}

QVariant DescriptionModel::data(const QModelIndex &index_, int role_) const
{
    const int row = index_.row();
    if(row < 0 || row >= m_data.size())
    {
        return QVariant();
    }
    const DescriptionData *item = m_data[row];
    switch(role_)
    {
    case IDRole:
        return item->id();
    case ImageIDRole:
        return item->imageId();
    case FromDateTimeRole:
        return item->fromDateTime();
    case DescriptionRole:
        return item->descriptionStr();
    }
    return QVariant();
}

bool DescriptionModel::setData(const QModelIndex &index_, const QVariant &value_, int role_ /*= Qt::EditRole*/)
{
    const int row = index_.row();
    if(row < 0 || row >= m_data.size())
    {
        return false;
    }
    DescriptionData *item = m_data[row];
    switch(role_)
    {
    case IDRole:
        item->setId(value_.toInt());
        break;
    case ImageIDRole:
        item->setImageId(value_.toInt());
        break;
    case FromDateTimeRole:
        item->setFromDateTime(value_.toDateTime());
        break;
    case DescriptionRole:
        item->setDescriptionStr(value_.toString());
        break;
    default:
        return false;
    }
    return true;
}

QVariant DescriptionModel::itemAt(int index_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return QVariant();
    }
    return QVariant::fromValue(m_data[index_]);
}

void DescriptionModel::setItemAt(int index_, QVariant itemVal_)
{
    if(index_ < 0 || index_ >= m_data.size())
    {
        return;
    }
    QObject *obj = qvariant_cast<QObject *>(itemVal_);
    DescriptionData *item = qobject_cast<DescriptionData *>(static_cast<QObject *>(obj));
    if(nullptr == item
            || item->id() != m_data[index_]->id()
            || item->imageId() != m_data[index_]->imageId()
            )
    {
        delete item;
        return;
    }
    delete m_data[index_];
    m_data[index_] = item;
}

int DescriptionModel::getIdOfIndex(int index_) const
{
    if(index_ < 0 || index_ >= m_data.size() || nullptr == m_data[index_])
    {
        return -1;
    }
    return m_data[index_]->id();
}

int DescriptionModel::getIndexOfId(int id_) const
{
    auto fit = std::find_if(std::begin(m_data), std::end(m_data),
                            [id_](DescriptionData *item_) -> bool
    {
        return nullptr != item_ && item_->id() == id_;
    });
    if(std::end(m_data) == fit)
    {
        return -1;
    }
    return std::distance(std::begin(m_data), fit);
}

void DescriptionModel::removeById(int id_)
{
    const int index = getIndexOfId(id_);
    if(index < 0 || index >= m_data.size())
    {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index + 1);
    auto fit = std::begin(m_data);
    std::advance(fit, index);
    m_data.erase(fit);
    endRemoveRows();
}

int DescriptionModel::imageID() const
{
    return m_imageId;
}

void DescriptionModel::setImageID(int imageId_)
{
    m_imageId = imageId_;

    startLoadDescriptions();

    emit imageIDChanged();
}

void DescriptionModel::startLoadDescriptions()
{
    Q_ASSERT(nullptr != NetAPI::getSingelton());
    if(!(nullptr != NetAPI::getSingelton())) { return; }
    if(imageID() < 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_data.size());
        clearData();
        endRemoveRows();
        return;
    }

    m_request = NetAPI::getSingelton()->startRequest();
    NetAPI::getSingelton()->get(
                QString("image-descriptions/%1/of_image/").arg(imageID()),
                m_request);
}

void DescriptionModel::clearData()
{
    for(auto *&p : m_data)
    {
        delete p;
        p = nullptr;
    }
    m_data.clear();
}

void DescriptionModel::loadDescriptionsJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
{
    if(!(m_request == request_))
    {
        return;
    }
    m_request = nullptr;
    if(errorCode_ != 0) { return; }
    Q_ASSERT(reply_.isArray());
    if(!(reply_.isArray()))
    {
        return;
    }
    QList<DescriptionData *> data;
    for(int i = 0; ; i++)
    {
        QJsonValue val = reply_[i];
        if(QJsonValue::Undefined == val.type())
        {
            break;
        }
        bool error = false;
        DescriptionData *item = DescriptionData::fromJson(this, imageID(), val, error);
        if(nullptr != item && !error) { data.push_back(item); }
        else { delete  item; item = nullptr; }
    }
    beginRemoveRows(QModelIndex(), 0, m_data.size());
    clearData();
    endRemoveRows();
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + data.size() - 1);
    std::copy(std::begin(data), std::end(data),
              std::inserter(m_data, std::end(m_data)));
    endInsertRows();

    emit descriptionLoaded();
}

QHash<int, QByteArray> DescriptionModel::roleNames() const
{
    return m_roleNames;
}
