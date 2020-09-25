#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H


#include <QObject>
#include <QJsonDocument>


class RequestData;
class JsonRequestData;


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

signals:
    void galleryIdChanged();
    void jsonDocChanged();
    void onNewGalleryCreated();
    void onGalleryEdited();

protected slots:
    void onNewGalleryCreatedSlot(RequestData *request_, const QJsonDocument &document_);
    void onEditGallerySlot(RequestData *request_, const QJsonDocument &document_);

private:
    int m_galleryId = -1;
    QJsonDocument m_jsonDoc;
    JsonRequestData *m_createNewGalleryRequest = nullptr;
    JsonRequestData *m_editGalleryRequest = nullptr;

private:
    int getGalleryId() const;
    void setGalleryId(int galleryId_);
    const QJsonDocument &getJsonDoc() const;
    void setJsonDoc(const QJsonDocument &jsonDoc_);
};


#endif // MASTACTVAAPI_H
