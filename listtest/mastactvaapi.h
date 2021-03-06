#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H

#include <QObject>
#include <QJsonDocument>
#include <QQmlExtensionPlugin>


class MastactvaAPI : public QObject
{
    Q_OBJECT
public:
    explicit MastactvaAPI(QObject *parent = nullptr);

    Q_PROPERTY(QJsonDocument jsonDoc READ getJsonDoc WRITE setJsonDoc NOTIFY jsonDocChanged)
    Q_PROPERTY(int galleryId READ getGalleryId WRITE setGalleryId NOTIFY galleryIdChanged)

public:
    Q_INVOKABLE void reloadGalleriesModel();
    Q_INVOKABLE void loadGalleryViewImagesOfGallery();
    Q_INVOKABLE void loadAllImagesOfGallery();

signals:
    void galleryIdChanged();
    void jsonDocChanged();

private:
    int m_galleryId = -1;
    QJsonDocument m_jsonDoc;

private:
    int getGalleryId() const;
    void setGalleryId(int galleryId_);
    const QJsonDocument &getJsonDoc() const;
    void setJsonDoc(const QJsonDocument &jsonDoc_);
};


class MastactvaAPIPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri);
};

#endif // MASTACTVAAPI_H
