#include "mastactvaapi.h"
#include <qqml.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QGuiApplication>
#include <type_traits>
#include "netapiv0.h"
#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"
#include "appconsts.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/netappconsts.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/utils.h"


MastactvaAPI::MastactvaAPI(QObject *parent) : QObject(parent)
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onNewGalleryCreatedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onEditGallerySlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onAddImageSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(delImageOfGallerySlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onNewDescriptionAddedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onDescriptionEditedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onDescriptionDeletedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onQuestionEditedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onAnswerAddedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onAnswerEditedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(onCurrentAnswerRemovedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(currentGalleryOwnshipTakenSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(currentGalleryOwnshipFreedSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(imageOfGalleryAsTopSetSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(imageOfGalleryAsTopResetSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(testLogingInSlot(int, RequestDataV0 *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestDataV0 *, const QJsonDocument &)),
                     this, SLOT(currentGalleryRemovedSlot(int, RequestDataV0 *, const QJsonDocument &)));

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
    QMLMainObjects* qmlMainObjects = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != qmlMainObjects);
    if(nullptr != qmlMainObjects)
    {
        GalleryEditViewModel *galleryModel = qmlMainObjects->getGalleryViewModel();
        Q_ASSERT(nullptr != galleryModel);
        if(nullptr != galleryModel)
        {
            QObject::disconnect(galleryModel, SIGNAL(galleryRealoded()), this, SLOT(galleryReloadedSlot()));
        }
    }

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
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_createNewGalleryRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("description", QJsonValue::fromVariant(description_));
    rec.insert("keywords", QJsonValue::fromVariant(keywords_));
    rec.insert("created", QJsonValue::fromVariant(QDateTime::currentDateTime()));
    rec.insert("points_to_pass", QJsonValue::fromVariant(pointsToPass_));
    rec.insert("owner", QJsonValue::fromVariant(0));
    rec.insert("creater", QJsonValue::fromVariant(0));
    QJsonDocument doc(rec);

    m_createNewGalleryRequest->setDocument(doc);
    netAPI->post("gallery/", m_createNewGalleryRequest);
}

void MastactvaAPI::onNewGalleryCreatedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_createNewGalleryRequest || request_ != static_cast<RequestDataV0 *>(m_createNewGalleryRequest)) { return; }
    m_createNewGalleryRequest = nullptr;
    if(errorCode_ != 0) { return; }

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
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_editGalleryRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("description", QJsonValue::fromVariant(description_));
    rec.insert("keywords", QJsonValue::fromVariant(keywords_));
    rec.insert("created", QJsonValue::fromVariant(created_));
    rec.insert("points_to_pass", QJsonValue::fromVariant(pointsToPass_));
    rec.insert("owner", QJsonValue::fromVariant(0));
    rec.insert("creater", QJsonValue::fromVariant(0));
    QJsonDocument doc(rec);

    m_editGalleryRequest->setDocument(doc);
    netAPI->patch(QString("gallery/%1/").arg(id_), m_editGalleryRequest);
}

void MastactvaAPI::onEditGallerySlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestDataV0 *>(m_editGalleryRequest)) { return; }
    m_editGalleryRequest = nullptr;
    if(errorCode_ != 0) { return; }

    // TODO: analyse error

    emit onGalleryEdited();
}

void MastactvaAPI::addImage(int galleryId_, const QString &fileURL_, bool topImage_)
{
    if(galleryId_ < 0)
    {
        if(m_galleryId < 0 || m_galleryIndex < 0) { setgalleryIndex(0); }
        if(m_galleryId < 0 || m_galleryIndex < 0) { return; }
        galleryId_ = m_galleryId;
    }

    const bool galleryEmpty = isCurrentGalleryEmpty();
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_addImageRequest = netAPI->startMultiPartFormData();
    m_addImageRequest->addPart("form-data; name=\"gallery\"", QString("%1").arg(galleryId_).toUtf8());
    QUrl url(fileURL_);
    QString filename = url.toLocalFile();
    QFile *file = new QFile(filename);
    QFileInfo fileInfo(file->fileName());
    QString hash = calculateHash(fileURL_);
    m_addImageRequest->addPart(QString("form-data; name=\"filename\"; filename=\"%1\"").arg(fileInfo.fileName().replace("\"", "")), file);
    m_addImageRequest->addPart("form-data; name=\"hash\"", hash.toUtf8());
    m_addImageRequest->addPart("form-data; name=\"use_in_gallery_view\"", (topImage_ || galleryEmpty)?"True":"False");
    m_addImageRequest->addPart("form-data; name=\"created\"", dateTimeToJsonString(QDateTime::currentDateTime()).toUtf8());

    netAPI->post("image/", m_addImageRequest);
}

void MastactvaAPI::removeCurrentImage()
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    const int currentImageId = imageOfGalleryId();
    if(currentImageId < 0) { return; }
    m_delImageOfGalleryRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(currentImageId));
    QJsonDocument doc(rec);

    m_delImageOfGalleryRequest->setDocument(doc);
    netAPI->del(QString("image/%1/").arg(currentImageId), m_delImageOfGalleryRequest);
}

void MastactvaAPI::delImageOfGallerySlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_delImageOfGalleryRequest || request_ != static_cast<RequestDataV0 *>(m_delImageOfGalleryRequest)) { return; }
    m_delImageOfGalleryRequest = nullptr;
    if(errorCode_ != 0) { return; }

    // TODO: analyse error

    emit imageOfGalleryRemoved();
}

void MastactvaAPI::onAddImageSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_addImageRequest || request_ != static_cast<RequestDataV0 *>(m_addImageRequest)) { return; }
    m_addImageRequest = nullptr;
    if(errorCode_ != 0) { return; }

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
    if(!(nullptr != p)) { return; }
    auto *m = p->getGalleryViewModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return; }
    const int id = m->getIdOfIndex(index_);
    setGalleryId(id);
}

void MastactvaAPI::setCurrentImageOfGalleryIdByIndex(int index_)
{
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p)) { return; }
    auto *m = p->getGalleryAllImagesModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return; }
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
    if(!(nullptr != p)) { return; }
    auto *m = p->getImageOfGalleryDescriptionModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return; }
    const int id = m->getIdOfIndex(index_);
    setImageOfGalleryDescriptionId(id);
}

void MastactvaAPI::removeCurrentDescription()
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }

    m_delDescriptionRequest = netAPI->startJsonRequest();
    netAPI->del(QString("image-description/%1/").arg(m_imageOfGalleryDescriptionId), m_delDescriptionRequest);

    setImageOfGalleryDescriptionIndex(-1);
}

void MastactvaAPI::onDescriptionDeletedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_delDescriptionRequest || static_cast<RequestDataV0 *>(m_delDescriptionRequest) != request_) { return; }

    m_delDescriptionRequest = nullptr;
    if(errorCode_ != 0) { return; }

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
    if(!(nullptr != p)) { return; }
    auto *m = p->getImageOfGalleryDescriptionModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return; }
    m->startLoadDescriptions();
    QObject::connect(m, SIGNAL(descriptionLoaded()), this, SLOT(onRefreshDescriptionsSlot()));
}

void MastactvaAPI::onRefreshDescriptionsSlot()
{
    emit onRefreshDescriptions();
}

void MastactvaAPI::newDescription(int imageId_, const QString &description, const QString &fromDateTime)
{
    if(imageId_ < 0) { return; }
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_newDescriptionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("image", QJsonValue::fromVariant(imageId_));
    rec.insert("from_field", QJsonValue::fromVariant(QDateTime::fromString(fromDateTime,Qt::ISODate)));
    rec.insert("descr", QJsonValue::fromVariant(description));
    QJsonDocument doc(rec);

    m_newDescriptionRequest->setDocument(doc);
    netAPI->post(QString("image-description/"), m_newDescriptionRequest);
}

void MastactvaAPI::onNewDescriptionAddedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_newDescriptionRequest || static_cast<RequestDataV0 *>(m_newDescriptionRequest) != request_) { return; }
    m_newDescriptionRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onNewDescriptionAdded();
}

void MastactvaAPI::editDescription(int id_, int imageId_, const QString &description, const QString &fromDateTime)
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    Q_ASSERT(imageId_ == m_imageOfGalleryId);

    m_editDescriptionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("image", QJsonValue::fromVariant(m_imageOfGalleryId));
    rec.insert("from_field", QJsonValue::fromVariant(QDateTime::fromString(fromDateTime,Qt::ISODate)));
    rec.insert("descr", QJsonValue::fromVariant(description));
    QJsonDocument doc(rec);

    m_editDescriptionRequest->setDocument(doc);
    netAPI->patch(QString("image-description/%1/").arg(id_), m_editDescriptionRequest);
}

void MastactvaAPI::onDescriptionEditedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr != m_editDescriptionRequest || static_cast<RequestDataV0 *>(m_editDescriptionRequest) != request_) { return; }
    m_editDescriptionRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onDescriptionEdited();
}

QVariant MastactvaAPI::getCurrentDescription()
{
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p)) { return QVariant(); }
    auto *m = p->getImageOfGalleryDescriptionModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return QVariant(); }
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

int MastactvaAPI::imageOfGalleryAnswerIndex() const
{
    return m_imageOfGalleryAnswerIndex;
}

void MastactvaAPI::setImageOfGalleryAnswerIndex(int index_)
{
    m_imageOfGalleryAnswerIndex = index_;

    emit imageOfGalleryAnswerIndexChanged();
}

QVariant MastactvaAPI::getCurrentQuestion()
{
    if(m_imageOfGalleryIndex < 0 || m_imageOfGalleryPointIndex < 0) { return QVariant(); }

    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p)) { return QVariant(); }
    auto *m = p->getGalleryAllImagesModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return QVariant(); }
    auto *pImageData = m->dataItemAt(m_imageOfGalleryIndex);
    if(nullptr == pImageData) { return QVariant(); }
    using PModelType = std::remove_const<decltype(m)>::type;
    auto *pImagePointsModel = pImageData->getImagePoints(static_cast<QObject *>(const_cast<PModelType>(m)));
    if(nullptr == pImagePointsModel || pImagePointsModel->isEmpty()) { return QVariant(); }
    auto *pImagePoint = pImagePointsModel->getAt(m_imageOfGalleryPointIndex);
    if(nullptr == pImagePoint) { return QVariant(); }
    auto *pQuestion = pImagePoint->getQuestion();
    if(nullptr == pQuestion) { return QVariant(); }
    return pQuestion->questionObj();
}

void MastactvaAPI::editQuestion(int id_, const QString &questionText_, qreal pointsToPass_, const QDateTime &created_)
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    if(id_ < 0) { return; }

    m_editQuestionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("question", QJsonValue::fromVariant(questionText_));
    rec.insert("points_to_pass", QJsonValue::fromVariant(pointsToPass_));
    rec.insert("created", QJsonValue::fromVariant(created_));
    QJsonDocument doc(rec);

    m_editQuestionRequest->setDocument(doc);
    netAPI->patch(QString("image-question/%1/").arg(id_), m_editQuestionRequest);
}

void MastactvaAPI::onQuestionEditedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_editQuestionRequest || static_cast<RequestDataV0 *>(m_editQuestionRequest) != request_) { return; }
    m_editQuestionRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onQuestionEdited();
}

void MastactvaAPI::refreshCurrentImagePointToQuestion()
{
    if(m_imageOfGalleryIndex < 0 || m_imageOfGalleryPointIndex < 0) { return; }

    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p)) { return; }
    auto *m = p->getGalleryAllImagesModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return; }
    auto *pImageData = m->dataItemAt(m_imageOfGalleryIndex);
    if(nullptr == pImageData) { return; }
    using PModelType = std::remove_const<decltype(m)>::type;
    auto *pImagePointsModel = pImageData->getImagePoints(static_cast<QObject *>(const_cast<PModelType>(m)));
    if(nullptr == pImagePointsModel || pImagePointsModel->isEmpty()) { return; }
    auto *pImagePoint = pImagePointsModel->getAt(m_imageOfGalleryPointIndex);
    if(nullptr == pImagePoint) { return; }
    m_imagePointToQuestion = pImagePoint->getQuestion();
    if(nullptr == m_imagePointToQuestion) { return; }
    QObject::connect(m_imagePointToQuestion, SIGNAL(imagePointToQuestionTextLoaded()), this , SLOT(imagePointToQuestionTextLoadedSlot()));
    m_imagePointToQuestion->refresh();
}

void MastactvaAPI::imagePointToQuestionTextLoadedSlot()
{
    if(nullptr != m_imagePointToQuestion)
    {
        QObject::disconnect(m_imagePointToQuestion, SIGNAL(imagePointToQuestionTextLoaded()), this , SLOT(imagePointToQuestionTextLoadedSlot()));
    }
    m_imagePointToQuestion = nullptr;
    emit imagePointToQuestionRefreshed();
}

void MastactvaAPI::addNewAnswer(int questionID_, const QString &answerText_, qreal pointFoAnswer_)
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_addAnswerRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("question", QJsonValue::fromVariant(questionID_));
    rec.insert("answer", QJsonValue::fromVariant(answerText_));
    rec.insert("points", QJsonValue::fromVariant(pointFoAnswer_));
    QJsonDocument doc(rec);

    m_addAnswerRequest->setDocument(doc);
    netAPI->post(QString("image-question-answer/"), m_addAnswerRequest);
}

void MastactvaAPI::onAnswerAddedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_addAnswerRequest || static_cast<RequestDataV0 *>(m_addAnswerRequest) != request_) { return; }
    m_addAnswerRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onNewAnswerAdded();
}

QVariant MastactvaAPI::getCurrentAnswer()
{
    if(m_imageOfGalleryIndex < 0 || m_imageOfGalleryPointIndex < 0 || m_imageOfGalleryAnswerIndex < 0) { return QVariant(); }

    QMLMainObjects* qmlMainObjects = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != qmlMainObjects);
    if(nullptr == qmlMainObjects) { return QVariant(); }
    auto *answersModel = qmlMainObjects->getQuestionAnswersModel();
    Q_ASSERT(nullptr != answersModel);
    if(nullptr == answersModel) { return QVariant(); }

    return answersModel->itemAt(m_imageOfGalleryAnswerIndex);
}

void MastactvaAPI::editAnswer(int id_, int questionId_, const QString &answerText_, qreal pointsForAnswer_)
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_editAnswerRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("question", QJsonValue::fromVariant(questionId_));
    rec.insert("answer", QJsonValue::fromVariant(answerText_));
    rec.insert("points", QJsonValue::fromVariant(pointsForAnswer_));
    QJsonDocument doc(rec);

    m_editAnswerRequest->setDocument(doc);
    netAPI->patch(QString("image-question-answer/%1/").arg(id_), m_editAnswerRequest);
}

void MastactvaAPI::onAnswerEditedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_editAnswerRequest || static_cast<RequestDataV0 *>(m_editAnswerRequest) != request_) { return; }
    m_editAnswerRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onAnswerEdited();
}

void MastactvaAPI::removeCurrentAnswer()
{
    if(m_imageOfGalleryIndex < 0 || m_imageOfGalleryPointIndex < 0 || m_imageOfGalleryAnswerIndex < 0) { return; }

    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }

    m_removeAnswerRequest = netAPI->startJsonRequest();
    netAPI->del(QString("image-question-answer/%1/").arg(m_imageOfGalleryAnswerIndex), m_removeAnswerRequest);

    setImageOfGalleryAnswerIndex(-1);
}

void MastactvaAPI::onCurrentAnswerRemovedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_removeAnswerRequest || static_cast<RequestDataV0 *>(m_removeAnswerRequest) != request_) { return; }
    m_removeAnswerRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onCurrentAnswerRemoved();
}

void MastactvaAPI::takeCurrentGalleryOwnship()
{
    if(m_galleryId < 0) { return; }

    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }

    m_takeGalleryOwnshipRequest = netAPI->startJsonRequest();
    netAPI->get(QString("gallery/%1/take_ownship/").arg(m_galleryId), m_takeGalleryOwnshipRequest);
}

void MastactvaAPI::freeCurrentGalleryOwnship()
{
    if(m_galleryId < 0) { return; }

    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }

    m_freeGalleryOwnshipRequest = netAPI->startJsonRequest();
    netAPI->get(QString("gallery/%1/free_ownship/").arg(m_galleryId), m_freeGalleryOwnshipRequest);
}

void MastactvaAPI::currentGalleryOwnshipTakenSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestDataV0 *>(m_takeGalleryOwnshipRequest) != request_) { return; }
    m_takeGalleryOwnshipRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit currentGalleryOwnshipTaken();
}

void MastactvaAPI::currentGalleryOwnshipFreedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestDataV0 *>(m_freeGalleryOwnshipRequest) != request_) { return; }
    m_freeGalleryOwnshipRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit currentGalleryOwnshipFreed();
}

void MastactvaAPI::refreshCurrentGallery()
{
    if(m_galleryIndex < 0) { return; }
    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p)) { return; }
    auto *m = p->getGalleryViewModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return; }
    m->refreshItemAtIndex(m_galleryIndex);
}

void MastactvaAPI::setImageOfGalleryAsTop()
{
    if(m_imageOfGalleryId < 0 || m_imageOfGalleryIndex < 0) { return; }

    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_setImageOfGalleryAsTopRequest = netAPI->startJsonRequest();
    netAPI->post(QString("image/%1/set_top/").arg(m_imageOfGalleryId), m_setImageOfGalleryAsTopRequest);
}

void MastactvaAPI::resetImageOfGalleryAsTop()
{
    if(m_imageOfGalleryId < 0 || m_imageOfGalleryIndex < 0) { return; }

    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_resetImageOfGalleryAsTopRequest = netAPI->startJsonRequest();
    netAPI->post(QString("image/%1/reset_top/").arg(m_imageOfGalleryId), m_resetImageOfGalleryAsTopRequest);
}

void MastactvaAPI::imageOfGalleryAsTopSetSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_setImageOfGalleryAsTopRequest || static_cast<RequestDataV0 *>(m_setImageOfGalleryAsTopRequest) != request_) { return; }
    m_setImageOfGalleryAsTopRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit imageOfGalleryAsTopSet();
}

void MastactvaAPI::imageOfGalleryAsTopResetSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_resetImageOfGalleryAsTopRequest || static_cast<RequestDataV0 *>(m_resetImageOfGalleryAsTopRequest) != request_) { return; }
    m_resetImageOfGalleryAsTopRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit imageOfGalleryAsTopReset();
}

void MastactvaAPI::testLogin(const QString &hostURL_, const QString &login_, const QString &password_)
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_lastHostURL = hostURL_;
    NetAPIV0::getSingelton()->getDefaultRequestData().setUrlBase(hostURL_);
    NetAPIV0::getSingelton()->getDefaultRequestData().setAuth(login_, password_);
    m_testLoginRequest = netAPI->startJsonRequest();
    netAPI->get(QString("gallery/"), m_testLoginRequest);
}

void MastactvaAPI::testLogingInSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    if(static_cast<RequestDataV0 *>(m_testLoginRequest) != request_) { return; }
    m_testLoginRequest = nullptr;
    if(errorCode_ != 0)
    {
        emit onFailedLogingIn(document_.toJson());
    }
    else
    {
        AppConsts::getInstance()->addServerURL(m_lastHostURL);
        AppConsts::getInstance()->save();
        NetAppConsts::set(
                    m_lastHostURL,
                    NetAPIV0::getSingelton()->getDefaultRequestData().getLogin(),
                    NetAPIV0::getSingelton()->getDefaultRequestData().getPass()
                    );
        QMLObjects::getInstance().getNetAPI()->updateAuthConsts();
        emit onLogingIn();
    }
}

QString MastactvaAPI::getHostURL()
{
    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return QString(); }
    return netAPI->getDefaultRequestData().getUrlBase();
}

QVariant MastactvaAPI::getHostURLs()
{
    return QVariant::fromValue(AppConsts::getInstance()->getServerURLsModel());
}

bool MastactvaAPI::useColorsVoronoyDiagram()
{
    return AppConsts::getInstance()->useColorsVoronoyDiagram();
}

void MastactvaAPI::regenerateVoronoyDiagramColors()
{
    AppConsts::getInstance()->initColors();
}

void MastactvaAPI::setUseColorsVoronoyDiagram(bool useColorsVoronoyDiagram_)
{
    AppConsts::getInstance()->setUseColorsVoronoyDiagram(useColorsVoronoyDiagram_);
}

void MastactvaAPI::saveAppConsts()
{
    AppConsts::getInstance()->save();
}

QString MastactvaAPI::dateTimeToUserStr(const QString &dateTimeStr_)
{
    QString res = dateTimeStr_;
    res.replace("T", ",");
    return res;
}

QString MastactvaAPI::dateTimeToUserStr(const QDateTime &dateTime_)
{
    return dateTimeToUserStr(dateTime_.toString(Qt::ISODate));
}

QString MastactvaAPI::dateTimeFromUserStr(const QString &dateTimeUserStr_)
{
    QString res = dateTimeUserStr_;
    res.replace(",", "T");
    return res;
}

ImagePointsModel* MastactvaAPI::getCurrentImagePointsModel()
{
    if(m_imageOfGalleryIndex < 0) { return nullptr; }

    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p)) { return nullptr; }
    auto *m = p->getGalleryAllImagesModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m)) { return nullptr; }
    auto *pImageData = m->dataItemAt(m_imageOfGalleryIndex);
    if(nullptr == pImageData) { return nullptr; }
    using PModelType = std::remove_const<decltype(m)>::type;
    return pImageData->getImagePoints(static_cast<QObject *>(const_cast<PModelType>(m)));
}

ImagePointData* MastactvaAPI::getCurrentImagePointData()
{
    if(m_imageOfGalleryPointIndex < 0) { return nullptr; }
    auto *pImagePointsModel = getCurrentImagePointsModel();
    if(nullptr == pImagePointsModel || pImagePointsModel->isEmpty()) { return nullptr; }
    return pImagePointsModel->getAt(m_imageOfGalleryPointIndex);
}


void MastactvaAPI::removeCurrentImagePoint()
{
    auto *pImagePoint = getCurrentImagePointData();
    if(nullptr == pImagePoint) { return; }

    QObject::connect(pImagePoint, SIGNAL(onPointRemoved()), this, SLOT(onImagePointRemovedSlot()));
    pImagePoint->removePoint();
}

void MastactvaAPI::onImagePointRemovedSlot()
{
    auto *pImagePointsModel = getCurrentImagePointsModel();
    if(nullptr == pImagePointsModel || pImagePointsModel->isEmpty()) { return; }
    pImagePointsModel->startLoadImagePoints();
    setImageOfGalleryPointIndex(-1);

    emit currentImagePointRemoved();
}

QVariant MastactvaAPI::getCurrentImagePoint()
{
    auto *pImagePoint = getCurrentImagePointData();
    if(nullptr == pImagePoint) { return QVariant(); }
    return QVariant::fromValue(pImagePoint);
}

void MastactvaAPI::editNextImageOfCurrentImagePoint(int nextImageID_)
{
    auto *pImagePoint = getCurrentImagePointData();
    if(nullptr == pImagePoint) { return; }
    (void)pImagePoint->toNextImage();
    m_imagePointToNextImage = pImagePoint->getNextImageData();
    if(nullptr == m_imagePointToNextImage) { return; }
    m_lastNextImageID = nextImageID_;
    if(m_imagePointToNextImage->loaded())
    {
        QObject::connect(m_imagePointToNextImage, SIGNAL(nextImageSet()), this, SLOT(nextImageSetSlot()));
        m_imagePointToNextImage->setNextImage(m_lastNextImageID);
    }
    else
    {
        QObject::connect(m_imagePointToNextImage, SIGNAL(imagePointToImageLoaded()), this, SLOT(imagePointToImageLoadedSlot()));
    }
}

void MastactvaAPI::imagePointToImageLoadedSlot()
{
    QObject::disconnect(m_imagePointToNextImage, SIGNAL(imagePointToImageLoaded()), this, SLOT(imagePointToImageLoadedSlot()));
    Q_ASSERT(m_imagePointToNextImage->loaded());
    QObject::connect(m_imagePointToNextImage, SIGNAL(nextImageSet()), this, SLOT(nextImageSetSlot()));
    m_imagePointToNextImage->setNextImage(m_lastNextImageID);
}

void MastactvaAPI::nextImageSetSlot()
{
    QObject::disconnect(m_imagePointToNextImage, SIGNAL(nextImageSet()), this, SLOT(nextImageSetSlot()));

    emit nextImageEdited();
}

bool MastactvaAPI::isCurrentGalleryEmpty()
{
    QMLMainObjects* qmlMainObjects = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != qmlMainObjects);
    if(nullptr != qmlMainObjects)
    {
        GalleryEditViewModel *galleryModel = qmlMainObjects->getGalleryViewModel();
        Q_ASSERT(nullptr != galleryModel);
        if(nullptr != galleryModel)
        {
            return galleryModel->isCurrentGalleryEmpty();
        }
    }
    return true;
}

void MastactvaAPI::removeCurrentGallery()
{
    if(m_galleryId < 0 || m_galleryIndex < 0) { return; }

    NetAPIV0 *netAPI = NetAPIV0::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI) { return; }
    m_removeCurrentGalleryRequest = netAPI->startJsonRequest();
    netAPI->del(QString("gallery/%1/").arg(m_galleryId), m_removeCurrentGalleryRequest);
}

void MastactvaAPI::currentGalleryRemovedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_removeCurrentGalleryRequest || static_cast<RequestDataV0 *>(m_removeCurrentGalleryRequest)!=request_) { return; }
    m_removeCurrentGalleryRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit currentGalleryRemoved();
}

void MastactvaAPI::showErrorMessage(const QString &header_, const QString &description_)
{
    emit errorMessage(header_, description_);
}

inline
bool isSymbol(const QChar &ch_)
{
    return ch_.category() >= QChar::Letter_Uppercase;
}

QString MastactvaAPI::readMore(const QString &str_, int maxSymbols_, const QString &rms_)
{
    QString str = ::leftDoubleCR(str_);
    if(str.length() <= maxSymbols_) { return str; }
    int i = maxSymbols_;
    for(;i < str.length() && isSymbol(str.at(i)); i++) {}
    return str.mid(0, i) + rms_;
}

void MastactvaAPI::refreshNextImage()
{
    ImagePointData *pImagePoint = getCurrentImagePointData();
    if(nullptr == pImagePoint) { return; }
    pImagePoint->setToNextImage(QVariant::fromValue(static_cast<QObject *>(nullptr)));
    (void)pImagePoint->toNextImage();
    emit imageOfGalleryPointIndexChanged();
}

void MastactvaAPI::switchLanguage(const QString & lang_)
{
    AppConsts::getInstance()->switchLanguage(lang_);
}

int MastactvaAPI::getLanguageIndex()
{
    return AppConsts::getInstance()->getLanguageIndex();
}

void MastactvaAPI::questionRefreshed()
{
    emit imageOfGalleryPointIndexChanged();
}

void MastactvaAPI::informInitialized()
{
    QMLObjects::getInstance().setInitialized();
}

void MastactvaAPI::setInitialized()
{
    emit initialized();
}

QString MastactvaAPI::leftDoubleCR(const QString &str_)
{
    return ::leftDoubleCR(str_);
}

QString MastactvaAPI::calculateHash(const QString &fileUrl_)
{
    QUrl url(fileUrl_);
    QString filename = url.toLocalFile();
    QFile *f1 = new QFile(filename);
    QFileInfo fileInfo(f1->fileName());
    QByteArray fd = f1->open(QIODevice::ReadOnly) ? f1->readAll() : fileInfo.fileName().toUtf8();
    return QString("%1").arg(QString(QCryptographicHash::hash(fd, QCryptographicHash::RealSha3_256).toHex()));
}
