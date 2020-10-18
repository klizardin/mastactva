#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H


#include <QObject>
#include <QJsonDocument>
#include <QtQuick/QQuickItem>


class RequestData;
class JsonRequestData;
class MultiPartRequestData;
class ImagePointToQuestion;


class MastactvaAPI : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MastactvaAPI(QObject *parent = nullptr);

    Q_PROPERTY(QJsonDocument jsonDoc READ getJsonDoc WRITE setJsonDoc NOTIFY jsonDocChanged)
    Q_PROPERTY(int galleryId READ getGalleryId WRITE setGalleryId NOTIFY galleryIdChanged)
    Q_PROPERTY(int imageOfGalleryId READ imageOfGalleryId WRITE setimageOfGalleryId NOTIFY imageOfGalleryIdChanged)
    Q_PROPERTY(int galleryIndex READ galleryIndex WRITE setgalleryIndex NOTIFY galleryIndexChanged)
    Q_PROPERTY(int imageOfGalleryIndex READ imageOfGalleryIndex WRITE setimageOfGalleryIndex NOTIFY imageOfGalleryIndexChanged)
    Q_PROPERTY(int imageOfGalleryDescriptionIndex READ imageOfGalleryDescriptionIndex WRITE setImageOfGalleryDescriptionIndex NOTIFY imageOfGalleryDescriptionIndexChanged)
    Q_PROPERTY(int imageOfGalleryDescriptionId READ imageOfGalleryDescriptionId WRITE setImageOfGalleryDescriptionId NOTIFY imageOfGalleryDescriptionIdChanged)
    Q_PROPERTY(int imageOfGalleryPointIndex READ imageOfGalleryPointIndex WRITE setImageOfGalleryPointIndex NOTIFY imageOfGalleryPointIndexChanged)
    Q_PROPERTY(int imageOfGalleryAnswerIndex READ imageOfGalleryAnswerIndex WRITE setImageOfGalleryAnswerIndex NOTIFY imageOfGalleryAnswerIndexChanged)

public:
    Q_INVOKABLE void reloadGalleriesModel();
    Q_INVOKABLE void reloadAllImagesOfGalleryViewModel();
    Q_INVOKABLE void loadGalleryViewImagesOfGallery();
    Q_INVOKABLE void loadAllImagesOfGallery();
    Q_INVOKABLE void createNewGallery(const QString &description_, const QString &keywords_, double pointsToPass_);
    Q_INVOKABLE void editGallery(int id, const QString &description_, const QString &keywords_, const QDateTime &created, const QVariant &pointsToPass_);
    Q_INVOKABLE void addImage(int galleryId, const QString &fileURL, bool topImage);
    Q_INVOKABLE void removeCurrentImage();
    Q_INVOKABLE void setCurrentGalleryIdByIndex(int index_);
    Q_INVOKABLE void setCurrentImageOfGalleryIdByIndex(int index_);
    Q_INVOKABLE void setCurrentOImageOfGalleryDescriptionIdByIndex(int index_);
    Q_INVOKABLE void removeCurrentDescription();
    Q_INVOKABLE QString nowJsonStr();
    Q_INVOKABLE void refreshDescriptions();
    Q_INVOKABLE void newDescription(const QString &description, const QString &fromDateTime);
    Q_INVOKABLE void editDescription(int id_, int imageId_, const QString &description, const QString &fromDateTime);
    Q_INVOKABLE QVariant getCurrentDescription();
    Q_INVOKABLE QVariant getCurrentQuestion();
    Q_INVOKABLE void editQuestion(int id_, const QString &questionText_, qreal pointsToPass_);
    Q_INVOKABLE void refreshCurrentImagePointToQuestion();
    Q_INVOKABLE void addNewAnswer(int questionID_, const QString &answerText_, qreal pointFoAnswer_);
    Q_INVOKABLE QVariant getCurrentAnswer();
    Q_INVOKABLE void editAnswer(int id_, int questionId_, const QString &answerText_, qreal pointsForAnswer_);
    Q_INVOKABLE void removeCurrentAnswer();

signals:
    void galleryReloaded();
    void imagesOfGalleryReloaded();
    void galleryIdChanged();
    void jsonDocChanged();
    void onNewGalleryCreated();
    void onGalleryEdited();
    void onImageAdded();
    void imageOfGalleryIdChanged();
    void imageOfGalleryRemoved();
    void galleryIndexChanged();
    void imageOfGalleryIndexChanged();
    void imageOfGalleryDescriptionIndexChanged();
    void imageOfGalleryDescriptionIdChanged();
    void onNewDescriptionAdded();
    void onDescriptionEdited();
    void onRefreshDescriptions();
    void onDescriptionDeleted();
    void imageOfGalleryPointIndexChanged();
    void onQuestionEdited();
    void imagePointToQuestionRefreshed();
    void imageOfGalleryAnswerIndexChanged();
    void onNewAnswerAdded();
    void onAnswerEdited();
    void onCurrentAnswerRemoved();

protected:
    int imageOfGalleryId() const;
    void setimageOfGalleryId(int id_);
    int galleryIndex() const;
    void setgalleryIndex(int index_);
    int imageOfGalleryIndex() const;
    void setimageOfGalleryIndex(int index_);
    int imageOfGalleryDescriptionIndex() const;
    void setImageOfGalleryDescriptionIndex(int index_);
    int imageOfGalleryDescriptionId() const;
    void setImageOfGalleryDescriptionId(int id_);
    int imageOfGalleryPointIndex() const;
    void setImageOfGalleryPointIndex(int index_);
    int imageOfGalleryAnswerIndex() const;
    void setImageOfGalleryAnswerIndex(int index_);


protected slots:
    void onNewGalleryCreatedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onEditGallerySlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onAddImageSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void galleryReloadedSlot();
    void delImageOfGallerySlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onNewDescriptionAddedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onDescriptionEditedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onRefreshDescriptionsSlot();
    void onDescriptionDeletedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onQuestionEditedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void imagePointToQuestionTextLoadedSlot();
    void onAnswerAddedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onAnswerEditedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);
    void onCurrentAnswerRemovedSlot(int errorCode_, RequestData *request_, const QJsonDocument &document_);

private:
    int m_galleryId = -1;
    int m_imageOfGalleryId = -1;
    int m_galleryIndex = -1;
    int m_imageOfGalleryIndex = -1;
    int m_imageOfGalleryDescriptionIndex = -1;
    int m_imageOfGalleryDescriptionId = -1;
    int m_imageOfGalleryPointIndex = -1;
    int m_imageOfGalleryAnswerIndex = -1;
    QJsonDocument m_jsonDoc;
    JsonRequestData *m_createNewGalleryRequest = nullptr;
    JsonRequestData *m_editGalleryRequest = nullptr;
    JsonRequestData *m_delImageOfGalleryRequest = nullptr;
    MultiPartRequestData *m_addImageRequest = nullptr;
    JsonRequestData *m_newDescriptionRequest = nullptr;
    JsonRequestData *m_editDescriptionRequest = nullptr;
    JsonRequestData *m_delDescriptionRequest = nullptr;
    JsonRequestData *m_editQuestionRequest = nullptr;
    JsonRequestData *m_addAnswerRequest = nullptr;
    JsonRequestData *m_editAnswerRequest = nullptr;
    JsonRequestData *m_removeAnswerRequest = nullptr;
    ImagePointToQuestion *m_imagePointToQuestion = nullptr;

private:
    int getGalleryId() const;
    void setGalleryId(int galleryId_);
    const QJsonDocument &getJsonDoc() const;
    void setJsonDoc(const QJsonDocument &jsonDoc_);
};


#endif // MASTACTVAAPI_H
