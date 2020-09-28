#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H


#include <QObject>
#include <QJsonDocument>


class RequestData;
class JsonRequestData;
class MultiPartRequestData;


class MastactvaAPI : public QObject
{
    Q_OBJECT
public:
    explicit MastactvaAPI(QObject *parent = nullptr);

    Q_PROPERTY(QJsonDocument jsonDoc READ getJsonDoc WRITE setJsonDoc NOTIFY jsonDocChanged)
    Q_PROPERTY(int galleryId READ getGalleryId WRITE setGalleryId NOTIFY galleryIdChanged)

public:
    Q_INVOKABLE void reloadGalleriesModel();
    Q_INVOKABLE void reloadAllImagesOfGalleryViewModel();
    Q_INVOKABLE void loadGalleryViewImagesOfGallery();
    Q_INVOKABLE void loadAllImagesOfGallery();
    Q_INVOKABLE void createNewGallery(const QString &description_, const QString &keywords_, double pointsToPass_);
    Q_INVOKABLE void editGallery(int id, const QString &description_, const QString &keywords_, const QDateTime &created, const QVariant &pointsToPass_);
    Q_INVOKABLE void addImage(int galleryId, const QString &fileURL, bool topImage);

signals:
    void galleryReloaded();
    void imagesOfGalleryReloaded();
    void galleryIdChanged();
    void jsonDocChanged();
    void onNewGalleryCreated();
    void onGalleryEdited();
    void onImageAdded();

protected slots:
    void onNewGalleryCreatedSlot(RequestData *request_, const QJsonDocument &document_);
    void onEditGallerySlot(RequestData *request_, const QJsonDocument &document_);
    void onAddImageSlot(RequestData *request_, const QJsonDocument &document_);
    void galleryReloadedSlot();

private:
    int m_galleryId = -1;
    QJsonDocument m_jsonDoc;
    JsonRequestData *m_createNewGalleryRequest = nullptr;
    JsonRequestData *m_editGalleryRequest = nullptr;
    MultiPartRequestData *m_addImageRequest = nullptr;

private:
    int getGalleryId() const;
    void setGalleryId(int galleryId_);
    const QJsonDocument &getJsonDoc() const;
    void setJsonDoc(const QJsonDocument &jsonDoc_);
};


#endif // MASTACTVAAPI_H
