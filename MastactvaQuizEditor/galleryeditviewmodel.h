#ifndef GALLERYEDITVIEWMODEL_H
#define GALLERYEDITVIEWMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDateTime>
#include <QString>
#include <QVector>
#include <QHash>
#include <QJsonValue>


class RequestData;

class GalleryImagesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames {
        ImageSourceRole = Qt::UserRole,
    };

public:
    GalleryImagesModel(QObject *parent = nullptr);
    virtual ~GalleryImagesModel() override;
    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

public:
    Q_PROPERTY(bool galleryViewImages READ galleryViewImages WRITE setGalleryViewImages NOTIFY galleryViewImagesChanged)
    Q_PROPERTY(int galleryId READ galleryId WRITE setGalleryId NOTIFY galleryIdChanged)

    bool galleryViewImages() const;
    void setGalleryViewImages(bool modeShowGalleryViewImages);
    int galleryId() const;
    void setGalleryId(int galleryId);

signals:
    void galleryIdChanged();
    void galleryViewImagesChanged();

private slots:
    void onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_);
    void ongalleryIdChanged();

protected:
    void startLoadImages();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    bool m_showGalleryViewImages = true;
    int m_galleryId = -1;
    RequestData* m_request = nullptr;
    QVector<QString> m_images;
    QHash<int, QByteArray> m_roleNames;
};




class GalleryItemData
{
protected:
    int id = -1;
    QString description;
    QDateTime created;
    double pointsToPass = 1.0;
    GalleryImagesModel *images = nullptr;

public:
    GalleryItemData(int id_ = -1,
                const QString &description_ = QString(),
                const QDateTime &created_ = QDateTime(),
                double pointsToPass_ = 1.0);
    virtual ~GalleryItemData();

    int getId() const;
    const QString &getDescription() const;
    const QDateTime &getCreated() const;
    double getPointsToPass() const;
    GalleryImagesModel *getImagesModel(QObject *parent_);

    static GalleryItemData fromJson(const QJsonValue& jsonValue_, bool &anyError);
};


class GalleryEditViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    // Define the role names to be used
    enum RoleNames {
        GalleryIDRole = Qt::UserRole,
        DescriptionRole = Qt::UserRole+1,
        CreatedRole = Qt::UserRole+2,
        PointsToPassRole = Qt::UserRole+3,
        ImagesRole = Qt::UserRole+4,
    };

public:
    explicit GalleryEditViewModel(QObject *parent = nullptr);
    virtual ~GalleryEditViewModel() override;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

private slots:
    void onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_);

public:
    void startLoadGalleries();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    RequestData* m_request = nullptr;
    QHash<int, QByteArray> m_roleNames;
    QVector<GalleryItemData> m_data;
};

#endif // GALLERYEDITVIEWMODEL_H
