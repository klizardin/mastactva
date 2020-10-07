#include "mastactvaapi.h"
#include <qqml.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QFileInfo>
#include "netapi.h"
#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"


MastactvaAPI::MastactvaAPI(QObject *parent) : QObject(parent)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onNewGalleryCreatedSlot(RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onEditGallerySlot(RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onAddImageSlot(RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(delImageOfGallerySlot(RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onNewDescriptionAddedSlot(RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onDescriptionEditedSlot(RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(RequestData *, const QJsonDocument &)),
                     this, SLOT(onDescriptionDeletedSlot(RequestData *, const QJsonDocument &)));

}

void MastactvaAPI::reloadGalleriesModel()
{
    QMLMainObjects* qmlMainObjects = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != qmlMainObjects);
    if(nullptr != qmlMainObjects)
    {
        GalleryEditViewModel *galleryModel = qmlMainObjects->getGalleryViewModel();
        Q_ASSERT(nullptr != galleryModel);
        if(nullptr != galleryModel)
        {
            galleryModel->startLoadGalleries();
            QObject::connect(galleryModel, SIGNAL(galleryRealoded()), this, SLOT(galleryReloadedSlot()));
        }
    }
}

void MastactvaAPI::galleryReloadedSlot()
{
    emit galleryReloaded();
}

int MastactvaAPI::imageOfGalleryId() const
{
    return m_imageOfGalleryId;
}

void MastactvaAPI::setimageOfGalleryId(int id_)
{
    m_imageOfGalleryId = id_;

    emit imageOfGalleryIdChanged();
}

void MastactvaAPI::reloadAllImagesOfGalleryViewModel()
{
    QMLMainObjects* qmlMainObjects = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != qmlMainObjects);
    if(nullptr != qmlMainObjects)
    {
        auto *allImagesModel = qmlMainObjects->getGalleryAllImagesModel();
        Q_ASSERT(nullptr != allImagesModel);
        if(nullptr != allImagesModel)
        {
            allImagesModel->startLoadImages();
        }
    }
}

void MastactvaAPI::loadGalleryViewImagesOfGallery()
{
    // TODO: implement method
}

void MastactvaAPI::loadAllImagesOfGallery()
{
    // TODO: implement method
}

void MastactvaAPI::createNewGallery(const QString &description_, const QString &keywords_, double pointsToPass_)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_createNewGalleryRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("description", QJsonValue::fromVariant(description_));
    rec.insert("keywords", QJsonValue::fromVariant(keywords_));
    rec.insert("created", QJsonValue::fromVariant(QDateTime::currentDateTime()));
    rec.insert("points_to_pass", QJsonValue::fromVariant(pointsToPass_));
    QJsonDocument doc(rec);

    m_createNewGalleryRequest->setDocument(doc);
    netAPI->post("galleries/", m_createNewGalleryRequest);
}

void MastactvaAPI::onNewGalleryCreatedSlot(RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_createNewGalleryRequest))
    {
        return;
    }
    m_createNewGalleryRequest = nullptr;

    // TODO: analyse error

    emit onNewGalleryCreated();
}

void MastactvaAPI::editGallery(
        int id_,
        const QString &description_,
        const QString &keywords_,
        const QDateTime &created_,
        const QVariant &pointsToPass_)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_editGalleryRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("description", QJsonValue::fromVariant(description_));
    rec.insert("keywords", QJsonValue::fromVariant(keywords_));
    rec.insert("created", QJsonValue::fromVariant(created_));
    rec.insert("points_to_pass", QJsonValue::fromVariant(pointsToPass_));
    QJsonDocument doc(rec);

    m_editGalleryRequest->setDocument(doc);
    netAPI->patch(QString("galleries/%1/").arg(id_), m_editGalleryRequest);
}

void MastactvaAPI::onEditGallerySlot(RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_editGalleryRequest))
    {
        return;
    }
    m_editGalleryRequest = nullptr;

    // TODO: analyse error

    emit onGalleryEdited();
}

void MastactvaAPI::addImage(int galleryId, const QString &fileURL, bool topImage)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_addImageRequest = netAPI->startMultiPartFormData();
    m_addImageRequest->addPart("form-data; name=\"gallery\"", QString("%1").arg(galleryId).toUtf8());
    QUrl url(fileURL);
    QString filename = url.toLocalFile();
    QFile *file = new QFile(filename);
    QFileInfo fileInfo(file->fileName());
    m_addImageRequest->addPart(QString("form-data; name=\"filename\"; filename=\"%1\"").arg(fileInfo.fileName().replace("\"", "")), file);
    m_addImageRequest->addPart("form-data; name=\"hash\"", "123");
    m_addImageRequest->addPart("form-data; name=\"use_in_gallery_view\"", topImage?"True":"False");

    netAPI->post("images/", m_addImageRequest);
}

void MastactvaAPI::removeCurrentImage()
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    const int currentImageId = imageOfGalleryId();
    if(currentImageId < 0)
    {
        return;
    }
    m_delImageOfGalleryRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(currentImageId));
    QJsonDocument doc(rec);

    m_delImageOfGalleryRequest->setDocument(doc);
    netAPI->del(QString("images/%1/").arg(currentImageId), m_delImageOfGalleryRequest);
}

void MastactvaAPI::delImageOfGallerySlot(RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_delImageOfGalleryRequest))
    {
        return;
    }
    m_delImageOfGalleryRequest = nullptr;

    // TODO: analyse error

    emit imageOfGalleryRemoved();
}

void MastactvaAPI::onAddImageSlot(RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_addImageRequest))
    {
        return;
    }
    m_addImageRequest = nullptr;

    // TODO: analyse error

    emit onImageAdded();
}

int MastactvaAPI::getGalleryId() const
{
    return m_galleryId;
}

void MastactvaAPI::setGalleryId(int galleryId_)
{
    m_galleryId = galleryId_;

    emit galleryIdChanged();
}

const QJsonDocument &MastactvaAPI::getJsonDoc() const
{
    return m_jsonDoc;
}

void MastactvaAPI::setJsonDoc(const QJsonDocument &jsonDoc_)
{
    m_jsonDoc = jsonDoc_;
}

int MastactvaAPI::galleryIndex() const
{
    return m_galleryIndex;
}

void MastactvaAPI::setgalleryIndex(int index_)
{
    m_galleryIndex = index_;

    emit galleryIndexChanged();

    setCurrentGalleryIdByIndex(m_galleryIndex);
}

int MastactvaAPI::imageOfGalleryIndex() const
{
    return m_imageOfGalleryIndex;
}

void MastactvaAPI::setimageOfGalleryIndex(int index_)
{
    m_imageOfGalleryIndex = index_;

    emit imageOfGalleryIndexChanged();

    setCurrentImageOfGalleryIdByIndex(m_imageOfGalleryIndex);
}

void MastactvaAPI::setCurrentGalleryIdByIndex(int index_)
{
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p))
    {
        return;
    }
    auto *m = p->getGalleryViewModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m))
    {
        return;
    }
    const int id = m->getIdOfIndex(index_);
    setGalleryId(id);
}

void MastactvaAPI::setCurrentImageOfGalleryIdByIndex(int index_)
{
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p))
    {
        return;
    }
    auto *m = p->getGalleryAllImagesModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m))
    {
        return;
    }
    const int id = m->getIdOfIndex(index_);
    setimageOfGalleryId(id);
}

int MastactvaAPI::imageOfGalleryDescriptionIndex() const
{
    return m_imageOfGalleryDescriptionIndex;
}

void MastactvaAPI::setImageOfGalleryDescriptionIndex(int index_)
{
    m_imageOfGalleryDescriptionIndex = index_;

    setCurrentOImageOfGalleryDescriptionIdByIndex(index_);

    emit imageOfGalleryDescriptionIndexChanged();
}

int MastactvaAPI::imageOfGalleryDescriptionId() const
{
    return m_imageOfGalleryDescriptionId;
}

void MastactvaAPI::setImageOfGalleryDescriptionId(int id_)
{
    m_imageOfGalleryDescriptionId = id_;

    emit imageOfGalleryDescriptionIdChanged();
}

void MastactvaAPI::setCurrentOImageOfGalleryDescriptionIdByIndex(int index_)
{
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p))
    {
        return;
    }
    auto *m = p->getImageOfGalleryDescriptionModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m))
    {
        return;
    }
    const int id = m->getIdOfIndex(index_);
    setImageOfGalleryDescriptionId(id);
}

void MastactvaAPI::removeCurrentDescription()
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }

    m_delDescriptionRequest = netAPI->startJsonRequest();
    netAPI->del(QString("image-descriptions/%1/").arg(m_imageOfGalleryDescriptionId), m_delDescriptionRequest);

    setImageOfGalleryDescriptionIndex(-1);
}

void MastactvaAPI::onDescriptionDeletedSlot(RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_delDescriptionRequest) != request_)
    {
        return;
    }

    m_delDescriptionRequest = nullptr;

    emit onDescriptionDeleted();
}

QString MastactvaAPI::nowJsonStr()
{
    QDateTime dt = QDateTime::currentDateTime();
    return dt.toString(Qt::ISODate);
}

void MastactvaAPI::refreshDescriptions()
{
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p))
    {
        return;
    }
    auto *m = p->getImageOfGalleryDescriptionModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m))
    {
        return;
    }
    m->startLoadDescriptions();
    QObject::connect(m, SIGNAL(descriptionLoaded()), this, SLOT(onRefreshDescriptionsSlot()));
}

void MastactvaAPI::onRefreshDescriptionsSlot()
{
    emit onRefreshDescriptions();
}

void MastactvaAPI::newDescription(const QString &description, const QString &fromDateTime)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_newDescriptionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("image", QJsonValue::fromVariant(m_imageOfGalleryId));
    rec.insert("from_field", QJsonValue::fromVariant(QDateTime::fromString(fromDateTime,Qt::ISODate)));
    rec.insert("descr", QJsonValue::fromVariant(description));
    QJsonDocument doc(rec);

    m_newDescriptionRequest->setDocument(doc);
    netAPI->post(QString("image-descriptions/"), m_newDescriptionRequest);
}

void MastactvaAPI::onNewDescriptionAddedSlot(RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_newDescriptionRequest) != request_)
    {
        return;
    }
    m_newDescriptionRequest = nullptr;

    emit onNewDescriptionAdded();
}

void MastactvaAPI::editDescription(int id_, int imageId_, const QString &description, const QString &fromDateTime)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    Q_ASSERT(imageId_ == m_imageOfGalleryId);

    m_editDescriptionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("image", QJsonValue::fromVariant(m_imageOfGalleryId));
    rec.insert("from_field", QJsonValue::fromVariant(QDateTime::fromString(fromDateTime,Qt::ISODate)));
    rec.insert("descr", QJsonValue::fromVariant(description));
    QJsonDocument doc(rec);

    m_editDescriptionRequest->setDocument(doc);
    netAPI->patch(QString("image-descriptions/%1/").arg(id_), m_editDescriptionRequest);
}

void MastactvaAPI::onDescriptionEditedSlot(RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_editDescriptionRequest) != request_)
    {
        return;
    }
    m_editDescriptionRequest = nullptr;

    emit onDescriptionEdited();
}

QVariant MastactvaAPI::getCurrentDescription()
{
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p))
    {
        return QVariant();
    }
    auto *m = p->getImageOfGalleryDescriptionModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m))
    {
        return QVariant();
    }
    return QVariant::fromValue(m->itemAt(m_imageOfGalleryDescriptionIndex));
}

int MastactvaAPI::imageOfGalleryPointIndex() const
{
    return m_imageOfGalleryPointIndex;
}

void MastactvaAPI::setImageOfGalleryPointIndex(int index_)
{
    m_imageOfGalleryPointIndex = index_;

    emit imageOfGalleryPointIndexChanged();
}
