#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QtQuick/QQuickItem>
#include "imagesource.h"
#include "Layout.h"
#include "Model.h"
#include "imagepoint.h"
#include "imagedescription.h"


class ImageModel;


class Image : public QObject, protected IListModelInfoObjectImpl
{
    Q_OBJECT
public:
    explicit Image(ImageModel *parent_ = nullptr);

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString imageSource READ getFilename WRITE setFilenameStr NOTIFY filenameChanged)
    Q_PROPERTY(QString hash READ hash WRITE setHash NOTIFY hashChanged)
    Q_PROPERTY(int gallery READ gallery WRITE setGallery NOTIFY galleryChanged)
    Q_PROPERTY(bool top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant imagePoints READ imagePoints WRITE setImagePoints NOTIFY imagePointsChanged)
    Q_PROPERTY(QVariant imageDescription READ imageDescription WRITE setImageDescription NOTIFY imageDescriptionChanged)

    Q_INVOKABLE bool isImageLoaded() const;

public:
    class DefaultLayout : public LayoutBase<Image>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("images");
            addSpecial<int>(layout::SpecialFieldEn::appId, &Image::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Image::m_thisListModelInfo);
            addField<int>("id", "id", &Image::id, &Image::setId);
            addField<ImageSource>("filename", "imageSource", &Image::filename, &Image::setFilename);
            addField<QString>("hash", "hash", &Image::hash, &Image::setHash);
            addField<int>("gallery", "galleryId", &Image::gallery, &Image::setGallery);
            addField<bool>("use_in_gallery_view", "top", &Image::top, &Image::setTop);
            addModel<ImagePointModel>("imagePoints", &Image::m_imagePoints, &Image::createImagePoints);
            addModel<ImageDescriptionModel>("imageDescription", &Image::m_imageDescriptionModel, &Image::createImageDescriptionModel);
        }
    };

    int id() const;
    void setId(const int &id_);
    ImageSource filename() const;
    void setFilename(const ImageSource &filename_);
    QString getFilename() const;
    void setFilenameStr(const QString &filename_);
    QString hash() const;
    void setHash(const QString &hash_);
    int gallery() const;
    void setGallery(const int &gallery_);
    bool top() const;
    void setTop(const bool &top_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    QVariant imagePoints() const;
    void setImagePoints(const QVariant &obj_);
    QVariant imageDescription() const;
    void setImageDescription(const QVariant &obj_);

protected:
    ImagePointModel *createImagePoints();
    ImageDescriptionModel *createImageDescriptionModel();

signals:
    void idChanged();
    void filenameChanged();
    void hashChanged();
    void galleryChanged();
    void topChanged();
    void createdChanged();
    void imagePointsChanged();
    void imageDescriptionChanged();

private:
    ImageModel *m_imageModel = nullptr;
    int m_appId = 0;
    int m_id = -1;
    ImageSource m_filename;
    QString m_hash;
    int m_gallery = -1;
    bool m_top = false;
    QDateTime m_created;
    ImagePointModel *m_imagePoints = nullptr;
    ImageDescriptionModel *m_imageDescriptionModel = nullptr;
    IListModelInfo *m_thisListModelInfo = 0;
};

class ImageModel : public ListModelBaseOfData<Image, ImageModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Image, ImageModel>;

public:
    explicit ImageModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void listReloaded();
    void outputModelChanged();
};


#endif // IMAGE_H
