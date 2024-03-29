/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H


#include <QObject>
#include <QJsonDocument>
#include <QtQuick/QQuickItem>
#include <QTranslator>


class RequestDataV0;
class JsonRequestDataV0;
class MultiPartRequestDataV0;
class ImagePointToQuestionV0;
class ImagePointData;
class ImagePointsModel;
class ImagePointToNextImageV0;


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
    Q_INVOKABLE void addImage(int galleryId_, const QString &fileURL_, bool topImage_);
    Q_INVOKABLE void removeCurrentImage();
    Q_INVOKABLE void setCurrentGalleryIdByIndex(int index_);
    Q_INVOKABLE void setCurrentImageOfGalleryIdByIndex(int index_);
    Q_INVOKABLE void setCurrentOImageOfGalleryDescriptionIdByIndex(int index_);
    Q_INVOKABLE void removeCurrentDescription();
    Q_INVOKABLE QString nowJsonStr();
    Q_INVOKABLE void refreshDescriptions();
    Q_INVOKABLE void newDescription(int imageId_, const QString &description, const QString &fromDateTime);
    Q_INVOKABLE void editDescription(int id_, int imageId_, const QString &description, const QString &fromDateTime);
    Q_INVOKABLE QVariant getCurrentDescription();
    Q_INVOKABLE QVariant getCurrentQuestion();
    Q_INVOKABLE void editQuestion(int id_, const QString &questionText_, qreal pointsToPass_, const QDateTime &created_);
    Q_INVOKABLE void refreshCurrentImagePointToQuestion();
    Q_INVOKABLE void addNewAnswer(int questionID_, const QString &answerText_, qreal pointFoAnswer_);
    Q_INVOKABLE QVariant getCurrentAnswer();
    Q_INVOKABLE void editAnswer(int id_, int questionId_, const QString &answerText_, qreal pointsForAnswer_);
    Q_INVOKABLE void removeCurrentAnswer();
    Q_INVOKABLE void takeCurrentGalleryOwnship();
    Q_INVOKABLE void freeCurrentGalleryOwnship();
    Q_INVOKABLE void refreshCurrentGallery();
    Q_INVOKABLE void setImageOfGalleryAsTop();
    Q_INVOKABLE void resetImageOfGalleryAsTop();
    Q_INVOKABLE void testLogin(const QString &hostURL_, const QString &login_, const QString &password_);
    Q_INVOKABLE QString getHostURL();
    Q_INVOKABLE QString dateTimeToUserStr(const QString &dateTimeStr_);
    Q_INVOKABLE QString dateTimeToUserStr(const QDateTime &dateTime_);
    Q_INVOKABLE QString dateTimeFromUserStr(const QString &dateTimeUserStr_);
    Q_INVOKABLE QVariant getCurrentImagePoint();
    Q_INVOKABLE void removeCurrentImagePoint();
    Q_INVOKABLE QVariant getHostURLs();
    Q_INVOKABLE QString getLastLogin();
    Q_INVOKABLE bool useColorsVoronoyDiagram();
    Q_INVOKABLE void regenerateVoronoyDiagramColors();
    Q_INVOKABLE void setUseColorsVoronoyDiagram(bool useColorsVoronoyDiagram_);
    Q_INVOKABLE void saveAppConsts();
    Q_INVOKABLE void editNextImageOfCurrentImagePoint(int nextImageID_);
    Q_INVOKABLE bool isCurrentGalleryEmpty();
    Q_INVOKABLE void removeCurrentGallery();
    Q_INVOKABLE void showErrorMessage(const QString &header_, const QString &description_);
    Q_INVOKABLE QString readMore(const QString &str_, int maxSymbols_, const QString &rms_);
    Q_INVOKABLE void refreshNextImage();
    Q_INVOKABLE void switchLanguage(const QString & lang_);
    Q_INVOKABLE int getLanguageIndex();
    Q_INVOKABLE void questionRefreshed();
    Q_INVOKABLE void informInitialized();
    Q_INVOKABLE QString leftDoubleCR(const QString &str_);
    Q_INVOKABLE QString calculateHash(const QString &fileUrl_);
    Q_INVOKABLE QString getShaderDescription(const QString &fileUrl_);
    Q_INVOKABLE QString getLuaDescription(const QString &fileUrl_);
    Q_INVOKABLE QDateTime now() const;
    Q_INVOKABLE bool isShaderUrl(const QString &url_);
    Q_INVOKABLE bool isLuaUrl(const QString &url_);
    Q_INVOKABLE bool isTextureUrl(const QString &url_);
    Q_INVOKABLE QString getFileDialogFilter(const QStringList &filters_, int artefactTypeId_);
    Q_INVOKABLE QString urlToFilename(const QString &fileNameURL_);
    Q_INVOKABLE QString getFileText(const QString &fileNameURL_);
    Q_INVOKABLE QString getLocalFileText(const QString &fileNameURL_);
    Q_INVOKABLE QString createTempFile(const QString &fileURL_, const QString &text_);
    Q_INVOKABLE void copyTmpFileTo(const QString &tmpFileName_, const QString &fileUrl_);
    Q_INVOKABLE void removeTmpFile(const QString &tmpFileName_);

public:
    void setInitialized();

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
    void currentGalleryOwnshipTaken();
    void currentGalleryOwnshipFreed();
    void imageOfGalleryAsTopSet();
    void imageOfGalleryAsTopReset();
    void onLogingIn();
    void onFailedLogingIn(const QString &msg_);
    void currentImagePointRemoved();
    void nextImageEdited();
    void currentGalleryRemoved();
    void errorMessage(const QString &header_, const QString &description_);
    void initialized();

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
    ImagePointsModel* getCurrentImagePointsModel();
    ImagePointData* getCurrentImagePointData();


protected slots:
    void onNewGalleryCreatedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onEditGallerySlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onAddImageSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void galleryReloadedSlot();
    void delImageOfGallerySlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onNewDescriptionAddedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onDescriptionEditedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onRefreshDescriptionsSlot();
    void onDescriptionDeletedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onQuestionEditedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void imagePointToQuestionTextLoadedSlot();
    void onAnswerAddedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onAnswerEditedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onCurrentAnswerRemovedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void currentGalleryOwnshipTakenSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void currentGalleryOwnshipFreedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void imageOfGalleryAsTopSetSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void imageOfGalleryAsTopResetSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void testLogingInSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);
    void onImagePointRemovedSlot();
    void nextImageSetSlot();
    void imagePointToImageLoadedSlot();
    void currentGalleryRemovedSlot(int errorCode_, RequestDataV0 *request_, const QJsonDocument &document_);

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
    JsonRequestDataV0 *m_createNewGalleryRequest = nullptr;
    JsonRequestDataV0 *m_editGalleryRequest = nullptr;
    JsonRequestDataV0 *m_delImageOfGalleryRequest = nullptr;
    MultiPartRequestDataV0 *m_addImageRequest = nullptr;
    JsonRequestDataV0 *m_newDescriptionRequest = nullptr;
    JsonRequestDataV0 *m_editDescriptionRequest = nullptr;
    JsonRequestDataV0 *m_delDescriptionRequest = nullptr;
    JsonRequestDataV0 *m_editQuestionRequest = nullptr;
    JsonRequestDataV0 *m_addAnswerRequest = nullptr;
    JsonRequestDataV0 *m_editAnswerRequest = nullptr;
    JsonRequestDataV0 *m_removeAnswerRequest = nullptr;
    JsonRequestDataV0 *m_takeGalleryOwnshipRequest = nullptr;
    JsonRequestDataV0 *m_freeGalleryOwnshipRequest = nullptr;
    JsonRequestDataV0 *m_setImageOfGalleryAsTopRequest = nullptr;
    JsonRequestDataV0 *m_resetImageOfGalleryAsTopRequest = nullptr;
    JsonRequestDataV0 *m_testLoginRequest = nullptr;
    JsonRequestDataV0 *m_removeCurrentGalleryRequest = nullptr;
    ImagePointToQuestionV0 *m_imagePointToQuestion = nullptr;
    QString m_lastHostURL;
    ImagePointToNextImageV0 *m_ImagePointToNextImageV0 = nullptr;
    int m_lastNextImageID = -1;

private:
    int getGalleryId() const;
    void setGalleryId(int galleryId_);
    const QJsonDocument &getJsonDoc() const;
    void setJsonDoc(const QJsonDocument &jsonDoc_);
};


#endif // MASTACTVAAPI_H
