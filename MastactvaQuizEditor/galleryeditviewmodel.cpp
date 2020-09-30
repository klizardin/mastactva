#include "galleryeditviewmodel.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QSGRendererInterface>
#include <QQuickWindow>
#include <QAbstractItemModel>
#include <QRunnable>
#include <QDebug>
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

ImagePointsModel *ImageData::getImagePoints(QObject *parent_)
{
    if(nullptr == imagePointsModel)
    {
        imagePointsModel = new ImagePointsModel(parent_);
        imagePointsModel->setSourceImageId(m_id);
        imagePointsModel->startLoadImagePoints();
    }
    return imagePointsModel;
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
    m_roleNames[ImagePointsRole] = "image_points";

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
    case ImagePointsRole:
        if(m_showGalleryViewImages)
            return QVariant();
        else
            return QVariant::fromValue<QObject *>(
                        const_cast<ImageData &>(image).getImagePoints(
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


ImagePointsModel::ImagePointsModel(QObject *parent_)
    :QAbstractListModel(parent_)
{
    m_roleNames[IDRole] = "imagePoint_id";
    m_roleNames[XRole] = "imagePoint_x";
    m_roleNames[YRole] = "imagePoint_y";
    m_roleNames[WeightRole] = "imagePoint_weight";

    QObject::connect(NetAPI::getSingelton(), SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onJsonRequestFinished(RequestData *, const QJsonDocument &)));
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

void ImagePointsModel::setSourceImageId(int sourceImageId_)
{
    m_sourceImageId = sourceImageId_;
}

void ImagePointsModel::startLoadImagePoints()
{
    auto *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(!(nullptr != netAPI))
    {
        return;
    }

    m_request = netAPI->startRequest();
    NetAPI::getSingelton()->get(QString("image-point/%1/of_image/").arg(m_sourceImageId), m_request);
}

void ImagePointsModel::onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_)
{
    Q_UNUSED(reply_);
    if(nullptr == m_request || m_request != request_)
    {
        return;
    }
    m_request = nullptr;

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

VoronoyDiagramRender::VoronoyDiagramRender(QObject *parent_)
    :QObject(parent_)
{
}

VoronoyDiagramRender::~VoronoyDiagramRender()
{
    delete m_program;
    m_program = nullptr;
    m_model = nullptr;
    m_window = nullptr;
}

void VoronoyDiagramRender::setModel(ImagePointsModel *model_)
{
    m_model = model_;
    m_imagePointsCnt = nullptr != m_model ? m_model->rowCount(QModelIndex()) : -1;
    m_points.clear();
    for(int i = 0; i < m_imagePointsCnt; i++)
    {
        const bool chk001 = nullptr != m_model && nullptr != m_model->getAt(i);
        Q_ASSERT(chk001);
        if(!chk001)
        {
            continue;
        }
        qreal x = m_model->getAt(i)->xCoord() * m_viewportSize.width();
        qreal y = m_model->getAt(i)->yCoord() * m_viewportSize.height();
        m_points.push_back(QVector2D(x,y));
    }
}

void VoronoyDiagramRender::setViewportSize(const QSize &size_)
{
    m_viewportSize = size_;
}

void VoronoyDiagramRender::setWindow(QQuickWindow *window_)
{
    m_window = window_;
}

void VoronoyDiagramRender::init()
{
    if (nullptr == m_program && nullptr != m_model && m_imagePointsCnt > 0)
    {
        QSGRendererInterface *rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    "attribute highp vec2 vertices;\n"
                                                    "void main() {\n"
                                                    "    gl_Position = vec4(vertices, 0.0, 1.0);\n"
                                                    "}\n");
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                            QString("uniform vec2 seeds[%1];\n"
                                                    "void main() {\n"
                                                    "    highp float dist0 = distance(seeds[0], gl_FragCoord.xy);\n"
                                                    "    highp float dist1 = dist0;\n"
                                                    "    for(int i = 1; i < %1; i++) {\n"
                                                    "        lowp float dist = distance(seeds[i], gl_FragCoord.xy);\n"
                                                    "        if(dist < dist0) {\n"
                                                    "            dist1 = dist0;\n"
                                                    "            dist0 = dist;\n"
                                                    "        } else if(dist < dist1) {\n"
                                                    "            dist1 = dist;\n"
                                                    "        }\n"
                                                    "    }\n"
                                                    "    highp float c = 2.0/(1.0 + exp(0.5*abs(dist0 - dist1)));\n"
                                                    "    gl_FragColor = vec4(c, c, c, 1.0);\n"
                                                    "}\n").arg(m_imagePointsCnt));

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();
    }
}

void VoronoyDiagramRender::paint()
{
    if(nullptr == m_program || nullptr == m_model)
    {
        return;
    }

    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();

    m_program->bind();

    m_program->enableAttributeArray(0);

    float values[] = {
        0, 0,
        (float)m_viewportSize.width(), 0,
        0, (float)m_viewportSize.height(),
        (float)m_viewportSize.width(), (float)m_viewportSize.height(),
    };

    // This example relies on (deprecated) client-side pointers for the vertex
    // input. Therefore, we have to make sure no vertex buffer is bound.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_program->setAttributeArray(0, GL_FLOAT, values, 8);
    m_program->setUniformValueArray("seeds", &m_points[0], m_imagePointsCnt);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(0);
    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();

    m_window->endExternalCommands();
}


class CleanupJob : public QRunnable
{
public:
    CleanupJob(VoronoyDiagramRender *renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    VoronoyDiagramRender *m_renderer = nullptr;
};


VoronoyDiagramItem::VoronoyDiagramItem()
{
    connect(this, &QQuickItem::windowChanged, this, &VoronoyDiagramItem::handleWindowChanged);
}

VoronoyDiagramItem::~VoronoyDiagramItem()
{
}

QVariant VoronoyDiagramItem::model()
{
    return QVariant::fromValue(m_model);
}

void VoronoyDiagramItem::setModel(QVariant model_)
{
    if(nullptr != m_model)
    {
        QObject::disconnect(m_model, SIGNAL(imagePointsLoaded()), this, SLOT(imagePointsLoadedSlot()));
    }

    QObject *obj = qvariant_cast<QObject *>(model_);
    ImagePointsModel *newModel = qobject_cast<ImagePointsModel *>(static_cast<QObject *>(obj));
    if(newModel == m_model)
    {
        return;
    }
    m_modelLoaded = false;
    m_model = newModel;
    if(nullptr != m_model)
    {
        QObject::connect(m_model, SIGNAL(imagePointsLoaded()), this, SLOT(imagePointsLoadedSlot()));
    }
    emit modelChanged();
}

void VoronoyDiagramItem::sync()
{
    if (nullptr == m_renderer && nullptr != window()) {
        m_renderer = new VoronoyDiagramRender(this);
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &VoronoyDiagramRender::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &VoronoyDiagramRender::paint, Qt::DirectConnection);
    }
    if(m_modelLoaded && nullptr != m_renderer)
    {
        m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
        m_renderer->setWindow(window());
        m_renderer->setModel(m_model);
        if (window())
            window()->update();
    }
}

void VoronoyDiagramItem::cleanup()
{
    delete m_renderer;
    m_renderer = nullptr;
}

void VoronoyDiagramItem::handleWindowChanged(QQuickWindow *win_)
{
    if (nullptr != win_)
    {
        connect(win_, &QQuickWindow::beforeSynchronizing, this, &VoronoyDiagramItem::sync, Qt::DirectConnection);
        connect(win_, &QQuickWindow::sceneGraphInvalidated, this, &VoronoyDiagramItem::cleanup, Qt::DirectConnection);
    }
}

void VoronoyDiagramItem::imagePointsLoadedSlot()
{
    m_modelLoaded = true;
}

void VoronoyDiagramItem::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}
