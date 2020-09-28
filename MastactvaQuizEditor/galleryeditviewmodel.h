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

class ImageData
{
public:
    ImageData(int id_ = -1, const QString& source_ = QString());
    ~ImageData() = default;

    int getId() const;
    void setId(int id_);
    const QString& getSource() const;
    void setSource(const QString& source_);

protected:
    int m_id = -1;
    QString m_source;
};

class GalleryImagesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames {
        IdRole = Qt::UserRole,
        ImageSourceRole = Qt::UserRole + 1,
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
    Q_PROPERTY(int galleryIndex READ galleryIndex WRITE setGalleryIndex NOTIFY galleryIndexChanged)

    bool galleryViewImages() const;
    void setGalleryViewImages(bool modeShowGalleryViewImages_);
    int galleryId() const;
    void setGalleryId(int galleryId_);
    int galleryIndex() const;
    void setGalleryIndex(int galleryIndex_);
    void startLoadImages();
    int getIdOfIndex(int index_);

signals:
    void galleryIdChanged();
    void galleryViewImagesChanged();
    void galleryIndexChanged();

private slots:
    void onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_);
    void ongalleryIdChanged();

protected:
protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    bool m_showGalleryViewImages = true;
    int m_galleryId = -1;
    int m_galleryIndex = -1;
    RequestData* m_request = nullptr;
    QVector<ImageData> m_images;
    QHash<int, QByteArray> m_roleNames;
};


class GalleryItemData : public QObject
{
    Q_OBJECT

protected:
    int id = -1;
    QString description;
    QString keywords;
    QDateTime created;
    qreal pointsToPass = 1.0;
    GalleryImagesModel *images = nullptr;

public:
    GalleryItemData(QObject* parent_,
                int id_ = -1,
                const QString &description_ = QString(),
                const QString &keywords_ = QString(),
                const QDateTime &created_ = QDateTime(),
                qreal pointsToPass_ = 1.0);
    virtual ~GalleryItemData();

    Q_PROPERTY(int id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString keywords READ getKeywords WRITE setKeywords NOTIFY keywordsChanged)
    Q_PROPERTY(QDateTime created READ getCreated WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QString pointsToPass READ getPointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_INVOKABLE QObject* getImages();

    int getId() const;
    const QString &getDescription() const;
    const QString &getKeywords() const;
    const QDateTime &getCreated() const;
    QString getPointsToPass() const;
    GalleryImagesModel *getImagesModel();

    void setId(int id_);
    void setDescription(const QString& description_);
    void setKeywords(const QString& keywords_);
    void setCreated(const QDateTime& created_);
    void setPointsToPass(const QString &pointsToPass_);

signals:
    void idChanged();
    void descriptionChanged();
    void keywordsChanged();
    void createdChanged();
    void pointsToPassChanged();

public:
    static GalleryItemData* fromJson(QObject* parent_, const QJsonValue& jsonValue_, bool &anyError);
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

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentIdChanged)
    Q_INVOKABLE QObject* getCurrentItem();

private slots:
    void onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_);

public:
    void startLoadGalleries();
    int getIdOfIndex(int index_) const;
    int currentIndex() const;
    void setCurrentIndex(int index_);
    int currentId() const;
    void setCurrentId(int id_);

protected:
    void clearData();

signals:
    void currentIndexChanged();
    void currentIdChanged();
    void galleryRealoded();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    RequestData* m_request = nullptr;
    QHash<int, QByteArray> m_roleNames;
    QVector<GalleryItemData*> m_data;
    int m_currentIndex = -1;
};

#endif // GALLERYEDITVIEWMODEL_H
