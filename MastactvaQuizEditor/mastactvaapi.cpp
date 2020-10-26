#include "mastactvaapi.h"
#include <qqml.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QFileInfo>
#include "netapi.h"
#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"
#include "appconsts.h"
#include <type_traits>


MastactvaAPI::MastactvaAPI(QObject *parent) : QObject(parent)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onNewGalleryCreatedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onEditGallerySlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onAddImageSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(delImageOfGallerySlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onNewDescriptionAddedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onDescriptionEditedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onDescriptionDeletedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onQuestionEditedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onAnswerAddedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onAnswerEditedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(onCurrentAnswerRemovedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(currentGalleryOwnshipTakenSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(currentGalleryOwnshipFreedSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(imageOfGalleryAsTopSetSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(imageOfGalleryAsTopResetSlot(int, RequestData *, const QJsonDocument &)));
    QObject::connect(netAPI, SIGNAL(onJsonRequestFinished(int, RequestData *, const QJsonDocument &)),
                     this, SLOT(testLogingInSlot(int, RequestData *, const QJsonDocument &)));
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
    rec.insert("owner", QJsonValue::fromVariant(0));
    QJsonDocument doc(rec);

    m_createNewGalleryRequest->setDocument(doc);
    netAPI->post("galleries/", m_createNewGalleryRequest);
}

void MastactvaAPI::onNewGalleryCreatedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_createNewGalleryRequest))
    {
        return;
    }
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

void MastactvaAPI::onEditGallerySlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_editGalleryRequest))
    {
        return;
    }
    m_editGalleryRequest = nullptr;
    if(errorCode_ != 0) { return; }

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

void MastactvaAPI::delImageOfGallerySlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_delImageOfGalleryRequest))
    {
        return;
    }
    m_delImageOfGalleryRequest = nullptr;
    if(errorCode_ != 0) { return; }

    // TODO: analyse error

    emit imageOfGalleryRemoved();
}

void MastactvaAPI::onAddImageSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(request_ != static_cast<RequestData *>(m_addImageRequest))
    {
        return;
    }
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

void MastactvaAPI::onDescriptionDeletedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_delDescriptionRequest) != request_)
    {
        return;
    }

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

void MastactvaAPI::newDescription(int imageId_, const QString &description, const QString &fromDateTime)
{
    if(imageId_ < 0) { return; }
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_newDescriptionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("image", QJsonValue::fromVariant(imageId_));
    rec.insert("from_field", QJsonValue::fromVariant(QDateTime::fromString(fromDateTime,Qt::ISODate)));
    rec.insert("descr", QJsonValue::fromVariant(description));
    QJsonDocument doc(rec);

    m_newDescriptionRequest->setDocument(doc);
    netAPI->post(QString("image-descriptions/"), m_newDescriptionRequest);
}

void MastactvaAPI::onNewDescriptionAddedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_newDescriptionRequest) != request_)
    {
        return;
    }
    m_newDescriptionRequest = nullptr;
    if(errorCode_ != 0) { return; }

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

void MastactvaAPI::onDescriptionEditedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_editDescriptionRequest) != request_)
    {
        return;
    }
    m_editDescriptionRequest = nullptr;
    if(errorCode_ != 0) { return; }

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
    if(m_imageOfGalleryIndex < 0 || m_imageOfGalleryPointIndex < 0)
    {
        return QVariant();
    }

    auto *p = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != p);
    if(!(nullptr != p))
    {
        return QVariant();
    }
    auto *m = p->getGalleryAllImagesModel();
    Q_ASSERT(nullptr != m);
    if(!(nullptr != m))
    {
        return QVariant();
    }
    auto *pImageData = m->dataItemAt(m_imageOfGalleryIndex);
    if(nullptr == pImageData)
    {
        return QVariant();
    }
    using PModelType = std::remove_const<decltype(m)>::type;
    auto *pImagePointsModel = pImageData->getImagePoints(static_cast<QObject *>(const_cast<PModelType>(m)));
    if(nullptr == pImagePointsModel || pImagePointsModel->isEmpty())
    {
        return QVariant();
    }
    auto *pImagePoint = pImagePointsModel->getAt(m_imageOfGalleryPointIndex);
    if(nullptr == pImagePoint)
    {
        return QVariant();
    }
    auto *pQuestion = pImagePoint->getQuestion();
    if(nullptr == pQuestion)
    {
        return QVariant();
    }
    return pQuestion->questionObj();
}

void MastactvaAPI::editQuestion(int id_, const QString &questionText_, qreal pointsToPass_)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    if(id_ < 0)
    {
        return;
    }

    m_editQuestionRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("question", QJsonValue::fromVariant(questionText_));
    rec.insert("points_to_pass", QJsonValue::fromVariant(pointsToPass_));
    QJsonDocument doc(rec);

    m_editQuestionRequest->setDocument(doc);
    netAPI->patch(QString("image-questions/%1/").arg(id_), m_editQuestionRequest);
}

void MastactvaAPI::onQuestionEditedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_editQuestionRequest) != request_)
    {
        return;
    }
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
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_addAnswerRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("question", QJsonValue::fromVariant(questionID_));
    rec.insert("answer", QJsonValue::fromVariant(answerText_));
    rec.insert("points", QJsonValue::fromVariant(pointFoAnswer_));
    QJsonDocument doc(rec);

    m_addAnswerRequest->setDocument(doc);
    netAPI->post(QString("image-question-answers/"), m_addAnswerRequest);
}

void MastactvaAPI::onAnswerAddedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_addAnswerRequest || static_cast<RequestData *>(m_addAnswerRequest) != request_)
    {
        return;
    }
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
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_editAnswerRequest = netAPI->startJsonRequest();
    QJsonObject rec;
    rec.insert("id", QJsonValue::fromVariant(id_));
    rec.insert("question", QJsonValue::fromVariant(questionId_));
    rec.insert("answer", QJsonValue::fromVariant(answerText_));
    rec.insert("points", QJsonValue::fromVariant(pointsForAnswer_));
    QJsonDocument doc(rec);

    m_editAnswerRequest->setDocument(doc);
    netAPI->patch(QString("image-question-answers/%1/").arg(id_), m_editAnswerRequest);
}

void MastactvaAPI::onAnswerEditedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_editAnswerRequest || static_cast<RequestData *>(m_editAnswerRequest) != request_)
    {
        return;
    }
    m_editAnswerRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onAnswerEdited();
}

void MastactvaAPI::removeCurrentAnswer()
{
    if(m_imageOfGalleryIndex < 0 || m_imageOfGalleryPointIndex < 0 || m_imageOfGalleryAnswerIndex < 0) { return; }

    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }

    m_removeAnswerRequest = netAPI->startJsonRequest();
    netAPI->del(QString("image-question-answers/%1/").arg(m_imageOfGalleryAnswerIndex), m_removeAnswerRequest);

    setImageOfGalleryAnswerIndex(-1);
}

void MastactvaAPI::onCurrentAnswerRemovedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(nullptr == m_removeAnswerRequest || static_cast<RequestData *>(m_removeAnswerRequest) != request_)
    {
        return;
    }
    m_removeAnswerRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit onCurrentAnswerRemoved();
}

void MastactvaAPI::takeCurrentGalleryOwnship()
{
    if(m_galleryId < 0) { return; }

    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }

    m_takeGalleryOwnshipRequest = netAPI->startJsonRequest();
    netAPI->get(QString("galleries/%1/take_ownship/").arg(m_galleryId), m_takeGalleryOwnshipRequest);
}

void MastactvaAPI::freeCurrentGalleryOwnship()
{
    if(m_galleryId < 0) { return; }

    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }

    m_freeGalleryOwnshipRequest = netAPI->startJsonRequest();
    netAPI->get(QString("galleries/%1/free_ownship/").arg(m_galleryId), m_freeGalleryOwnshipRequest);
}

void MastactvaAPI::currentGalleryOwnshipTakenSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_takeGalleryOwnshipRequest) != request_) { return; }
    m_takeGalleryOwnshipRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit currentGalleryOwnshipTaken();
}

void MastactvaAPI::currentGalleryOwnshipFreedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_freeGalleryOwnshipRequest) != request_) { return; }
    m_freeGalleryOwnshipRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit currentGalleryOwnshipFreed();
}

void MastactvaAPI::refreshCurrentGallery()
{
    if(m_galleryIndex < 0) { return; }
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
    m->refreshItemAtIndex(m_galleryIndex);
}

void MastactvaAPI::setImageOfGalleryAsTop()
{
    if(m_imageOfGalleryId < 0 || m_imageOfGalleryIndex < 0) { return; }

    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_setImageOfGalleryAsTopRequest = netAPI->startJsonRequest();
    // TODO: make none get method
    netAPI->get(QString("images/%1/set_top/").arg(m_imageOfGalleryId), m_setImageOfGalleryAsTopRequest);
}

void MastactvaAPI::resetImageOfGalleryAsTop()
{
    if(m_imageOfGalleryId < 0 || m_imageOfGalleryIndex < 0) { return; }

    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_resetImageOfGalleryAsTopRequest = netAPI->startJsonRequest();
    // TODO: make none get method
    netAPI->get(QString("images/%1/reset_top/").arg(m_imageOfGalleryId), m_resetImageOfGalleryAsTopRequest);
}

void MastactvaAPI::imageOfGalleryAsTopSetSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_setImageOfGalleryAsTopRequest) != request_) { return; }
    m_setImageOfGalleryAsTopRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit imageOfGalleryAsTopSet();
}

void MastactvaAPI::imageOfGalleryAsTopResetSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    Q_UNUSED(document_);

    if(static_cast<RequestData *>(m_resetImageOfGalleryAsTopRequest) != request_) { return; }
    m_resetImageOfGalleryAsTopRequest = nullptr;
    if(errorCode_ != 0) { return; }

    emit imageOfGalleryAsTopReset();
}

void MastactvaAPI::testLogin(const QString &hostURL_, const QString &login_, const QString &password_)
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return;
    }
    m_lastHostURL = hostURL_;
    NetAPI::getSingelton()->getDefaultRequestData().setUrlBase(hostURL_);
    NetAPI::getSingelton()->getDefaultRequestData().setAuth(login_, password_);
    m_testLoginRequest = netAPI->startJsonRequest();
    netAPI->get(QString("galleries/"), m_testLoginRequest);
}

void MastactvaAPI::testLogingInSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_)
{
    if(static_cast<RequestData *>(m_testLoginRequest) != request_) { return; }
    m_testLoginRequest = nullptr;
    if(errorCode_ != 0)
    {
        emit onFailedLogingIn(document_.toJson());
    }
    else
    {
        AppConsts::getInstance()->addServerURL(m_lastHostURL);
        AppConsts::getInstance()->save();
        emit onLogingIn();
    }
}

QString MastactvaAPI::getHostURL()
{
    NetAPI *netAPI = NetAPI::getSingelton();
    Q_ASSERT(nullptr != netAPI);
    if(nullptr == netAPI)
    {
        return QString();
    }
    return netAPI->getDefaultRequestData().getUrlBase();
}

QVariant MastactvaAPI::getHostURLs()
{
    return QVariant::fromValue(AppConsts::getInstance()->getServerURLsModel());
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

void MastactvaAPI::removeCurrentImagePoint()
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

    QObject::connect(pImagePoint, SIGNAL(onPointRemoved()), this, SLOT(onImagePointRemovedSlot()));
    pImagePoint->removePoint();
}

void MastactvaAPI::onImagePointRemovedSlot()
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
    pImagePointsModel->startLoadImagePoints();
    setImageOfGalleryPointIndex(-1);

    emit currentImagePointRemoved();
}

QVariant MastactvaAPI::getCurrentImagePoint()
{
    if(m_imageOfGalleryIndex < 0 || m_imageOfGalleryPointIndex < 0 ) { return QVariant(); }

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
    return QVariant::fromValue(pImagePoint);
}
