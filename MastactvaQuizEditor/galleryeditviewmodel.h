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
class ImagePointsModel;


class ImageData
{
public:
    ImageData(int id_ = -1, const QString& source_ = QString());
    ~ImageData() = default;

    int getId() const;
    void setId(int id_);
    const QString& getSource() const;
    void setSource(const QString& source_);
    ImagePointsModel *getImagePoints(QObject *parent_);

protected:
    int m_id = -1;
    QString m_source;
    ImagePointsModel *imagePointsModel = nullptr;
};

class GalleryImagesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames {
        IdRole = Qt::UserRole,
        ImageSourceRole = Qt::UserRole + 1,
        ImagePointsRole = Qt::UserRole + 2,
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
    // constructions
    explicit GalleryEditViewModel(QObject *parent = nullptr);
    virtual ~GalleryEditViewModel() override;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

    // properies
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentIdChanged)
    Q_INVOKABLE QObject* getCurrentItem();

    //slots
private slots:
    void onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_);

    // methods
public:
    void startLoadGalleries();
    int getIdOfIndex(int index_) const;
    int currentIndex() const;
    void setCurrentIndex(int index_);
    int currentId() const;
    void setCurrentId(int id_);

    // protected methods
protected:
    void clearData();

    // signals
signals:
    void currentIndexChanged();
    void currentIdChanged();
    void galleryRealoded();

    // data members
protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    RequestData* m_request = nullptr;
    QHash<int, QByteArray> m_roleNames;
    QVector<GalleryItemData*> m_data;
    int m_currentIndex = -1;
};

class ImagePointData : public QObject
{
    Q_OBJECT

public:
    ImagePointData(QObject *parent_,
                   int sourceImageId_ = -1,
                   int pointId_ = -1,
                   qreal x_ = 0.5,
                   qreal y_ = 0.5,
                   qreal weight_ = 1.0);
    virtual ~ImagePointData() = default;

public:
    Q_PROPERTY(int pointId READ pointId WRITE setPointId NOTIFY pointIdChanged)
    Q_PROPERTY(qreal xCoord READ xCoord WRITE setXCoord NOTIFY xCoordChanged)
    Q_PROPERTY(qreal yCoord READ yCoord WRITE setYCoord NOTIFY yCoordChanged)
    Q_PROPERTY(qreal weight READ weight WRITE setWeight NOTIFY weightChanged)

public:
    int getSourceImageId() const;
    void setSourceImageId(int sourceImageId_);
    int pointId() const;
    void setPointId(int id_);
    qreal xCoord() const;
    void setXCoord(qreal x_);
    qreal yCoord() const;
    void setYCoord(qreal y_);
    qreal weight() const;
    void setWeight(qreal weight_);

    static ImagePointData *fromJson(QObject *parent_, int sourceImageId_, const QJsonValue& jsonObject_, bool& error_);

signals:
    void pointIdChanged();
    void xCoordChanged();
    void yCoordChanged();
    void weightChanged();

private:
    int m_sourceImageId = -1;
    int m_pointId = -1;
    qreal m_x = 0.5;
    qreal m_y = 0.5;
    qreal m_weight = 1.0;
};

class ImagePointsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames {
        IDRole = Qt::UserRole,
        XRole = Qt::UserRole + 1,
        YRole = Qt::UserRole + 2,
        WeightRole = Qt::UserRole + 3,
    };

public:
    ImagePointsModel(QObject *parent_);
    virtual ~ImagePointsModel() override;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

public:
    void setSourceImageId(int sourceImageId_);
    void startLoadImagePoints();

signals:
    void imagePointsLoaded();

    //slots
private slots:
    void onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_);

protected:
    void clearData();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    int m_sourceImageId = -1;
    QVector<ImagePointData *> m_data;
    RequestData* m_request = nullptr;
};


#endif // GALLERYEDITVIEWMODEL_H
