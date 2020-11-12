#ifndef IMAGEPOINT_H
#define IMAGEPOINT_H


#include <QObject>
#include <QDateTime>
#include <QVariant>
#include "imagesource.h"
#include "Layout.h"
#include "Model.h"


class ImagePointToNextImage : public QObject
{
    Q_OBJECT

public:
    explicit ImagePointToNextImage(QObject *parent_ = nullptr);

    Q_PROPERTY(int iptniId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int iptniImagePointId READ imagePointId WRITE setImagePointId NOTIFY imagePointIdChanged)
    Q_PROPERTY(int iptniNextImage READ nextImage WRITE setNextImage NOTIFY nextImageChanged)


    class DefaultLayout : public LayoutBase<ImagePointToNextImage>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-point-to-next-image");
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImagePointToNextImage::m_appId);
            addField<int>("id", "iptniId", &ImagePointToNextImage::id, &ImagePointToNextImage::setId);
            addField<int>("image_point", "iptniImagePointId", &ImagePointToNextImage::imagePointId, &ImagePointToNextImage::setImagePointId);
            addField<int>("next_image", "iptniNextImage", &ImagePointToNextImage::nextImage, &ImagePointToNextImage::setNextImage);
        }
    };

    int id() const;
    void setId(const int &id_);
    int imagePointId() const;
    void setImagePointId(const int &imagePointId_);
    int nextImage() const;
    void setNextImage(const int &nextImage_);

signals:
    void idChanged();
    void imagePointIdChanged();
    void nextImageChanged();

private:
    int m_appId = -1;
    int m_id = -1;
    int m_imagePointId = -1;
    int m_nextImage = -1;
};

class ImagePointToNextImageModel : public ListModelBaseOfData<ImagePointToNextImage, ImagePointToNextImageModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ImagePointToNextImage, ImagePointToNextImageModel>;

public:
    explicit ImagePointToNextImageModel(QObject *parent_ = nullptr);

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
};


class ImagePointModel;


class ImagePoint : public QObject
{
    Q_OBJECT
public:
    explicit ImagePoint(ImagePointModel *parent_ = nullptr);

    Q_PROPERTY(int ipId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int ipImageId READ imageId WRITE setImageId NOTIFY imageIdChanged)
    Q_PROPERTY(qreal ipXCoord READ xCoord WRITE setXCoord NOTIFY xCoordChanged)
    Q_PROPERTY(qreal ipYCoord READ yCoord WRITE setYCoord NOTIFY yCoordChanged)
    Q_PROPERTY(qreal ipWeight READ weight WRITE setWeight NOTIFY weightChanged)
    Q_PROPERTY(QDateTime ipCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant nextImage READ nextImage WRITE setNextImage NOTIFY nextImageChanged)

    class DefaultLayout : public LayoutBase<ImagePoint>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-point");
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImagePoint::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ImagePoint::m_parentModelInfo);
            addField<int>("id", "ipId", &ImagePoint::id, &ImagePoint::setId);
            addField<int>("image", "ipImageId", &ImagePoint::imageId, &ImagePoint::setImageId);
            addField<qreal>("x", "ipXCoord", &ImagePoint::xCoord, &ImagePoint::setXCoord);
            addField<qreal>("y", "ipYCoord", &ImagePoint::yCoord, &ImagePoint::setYCoord);
            addField<qreal>("weight", "ipWeight", &ImagePoint::weight, &ImagePoint::setWeight);
            addField<QDateTime>("created", "ipCreated", &ImagePoint::created, &ImagePoint::setCreated);
            addModel<ImagePointToNextImageModel>("nextImage", &ImagePoint::m_imagePointToNextImage, &ImagePoint::createImagePointToNextImage);
        }
    };

    int id() const;
    void setId(const int &id_);
    int imageId() const;
    void setImageId(const int &imageId_);
    qreal xCoord() const;
    void setXCoord(const qreal &xCoord_);
    qreal yCoord() const;
    void setYCoord(const qreal &yCoord_);
    qreal weight() const;
    void setWeight(const qreal &weight_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    QVariant nextImage() const;
    void setNextImage(const QVariant &obj_);
    ImagePointToNextImageModel *getNextImage() const;

    qreal distanceCoef(qreal x_, qreal y_) const;

signals:
    void idChanged();
    void imageIdChanged();
    void xCoordChanged();
    void yCoordChanged();
    void weightChanged();
    void createdChanged();
    void nextImageChanged();

protected:
    ImagePointToNextImageModel *createImagePointToNextImage();

private:
    int m_appId = -1;
    int m_id = -1;
    int m_imageId = -1;
    qreal m_x = 0.0;
    qreal m_y = 0.0;
    qreal m_weight = 1.0;
    QDateTime m_created;
    ImagePointToNextImageModel *m_imagePointToNextImage = nullptr;
    ImagePointModel *m_imagePointModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
};

class ImagePointModel : public ListModelBaseOfData<ImagePoint, ImagePointModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ImagePoint, ImagePointModel>;

public:
    explicit ImagePointModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

    Q_INVOKABLE QVariant nextImageByCoords(qreal x_, qreal y_);

public slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

    virtual void listLoaded(const QJsonDocument &reply_) override;

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
};


#endif // IMAGEPOINT_H
