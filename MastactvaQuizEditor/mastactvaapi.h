#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H


#include <QObject>
#include <QJsonDocument>
#include <QtQuick/QQuickItem>


class RequestData;
class JsonRequestData;
class MultiPartRequestData;


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

protected:
    int imageOfGalleryId() const;
    void setimageOfGalleryId(int id_);
    int galleryIndex() const;
    void setgalleryIndex(int index_);
    int imageOfGalleryIndex() const;
    void setimageOfGalleryIndex(int index_);


protected slots:
    void onNewGalleryCreatedSlot(RequestData *request_, const QJsonDocument &document_);
    void onEditGallerySlot(RequestData *request_, const QJsonDocument &document_);
    void onAddImageSlot(RequestData *request_, const QJsonDocument &document_);
    void galleryReloadedSlot();
    void delImageOfGallerySlot(RequestData *request_, const QJsonDocument &document_);

private:
    int m_galleryId = -1;
    int m_imageOfGalleryId = -1;
    int m_galleryIndex = -1;
    int m_imageOfGalleryIndex = -1;
    QJsonDocument m_jsonDoc;
    JsonRequestData *m_createNewGalleryRequest = nullptr;
    JsonRequestData *m_editGalleryRequest = nullptr;
    JsonRequestData *m_delImageOfGalleryRequest = nullptr;
    MultiPartRequestData *m_addImageRequest = nullptr;

private:
    int getGalleryId() const;
    void setGalleryId(int galleryId_);
    const QJsonDocument &getJsonDoc() const;
    void setJsonDoc(const QJsonDocument &jsonDoc_);
};


#endif // MASTACTVAAPI_H
