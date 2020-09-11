#ifndef GALLERYEDITVIEWMODEL_H
#define GALLERYEDITVIEWMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDateTime>
#include <QString>
#include <QVector>
#include <QHash>


class GalleryEditViewImagesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames {
        ImageSourceRole = Qt::UserRole,
    };

public:
    GalleryEditViewImagesModel(QObject *parent = nullptr);
    virtual ~GalleryEditViewImagesModel() override;
    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

public:
    void setGalleryID(long long galleryId);

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    long long m_galleryId = -1;
    QVector<QString> m_images;
    QHash<int, QByteArray> m_roleNames;
};


class GalleryItemData
{
protected:
    QString description;
    QDateTime created;
    double pointsToPass = 1.0;
    GalleryEditViewImagesModel *images = nullptr;

public:
    GalleryItemData(const QString &description_ = QString(),
                const QDateTime &created_ = QDateTime(),
                double pointsToPass_ = 1.0);
    virtual ~GalleryItemData();

    const QString &getDescription() const;
    const QDateTime &getCreated() const;
    double getPointsToPass() const;
    GalleryEditViewImagesModel *getImagesModel(QObject *parent);
};


class GalleryEditViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    // Define the role names to be used
    enum RoleNames {
        DescriptionRole = Qt::UserRole,
        CreatedRole = Qt::UserRole+1,
        PointsToPassRole = Qt::UserRole+2,
        ImagesRole = Qt::UserRole+3,
    };

public:
    explicit GalleryEditViewModel(QObject *parent = nullptr);
    virtual ~GalleryEditViewModel() override;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    QVector<GalleryItemData> m_data;
};

#endif // GALLERYEDITVIEWMODEL_H
