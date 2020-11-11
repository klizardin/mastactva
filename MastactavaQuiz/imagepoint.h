#ifndef IMAGEPOINT_H
#define IMAGEPOINT_H


#include <QObject>
#include <QDateTime>
#include <QVariant>
#include "Layout.h"
#include "Model.h"


class ImagePoint : public QObject
{
    Q_OBJECT
public:
    explicit ImagePoint(QObject *parent_ = nullptr);

    Q_PROPERTY(int ipId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int ipImageId READ imageId WRITE setImageId NOTIFY imageIdChanged)
    Q_PROPERTY(qreal ipXCoord READ xCoord WRITE setXCoord NOTIFY xCoordChanged)
    Q_PROPERTY(qreal ipYCoord READ yCoord WRITE setYCoord NOTIFY yCoordChanged)
    Q_PROPERTY(qreal ipWeight READ weight WRITE setWeight NOTIFY weightChanged)
    Q_PROPERTY(QDateTime ipCreated READ created WRITE setCreated NOTIFY createdChanged)
    //Q_PROPERTY(QVariant nextImage READ nextImage WRITE setNextImage NOTIFY nextImageChanged)

    class DefaultLayout : public LayoutBase<ImagePoint>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-point");
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImagePoint::m_appId);
            addField<int>("id", "ipId", &ImagePoint::id, &ImagePoint::setId);
            addField<int>("image", "ipImageId", &ImagePoint::imageId, &ImagePoint::setImageId);
            addField<qreal>("x", "ipXCoord", &ImagePoint::xCoord, &ImagePoint::setXCoord);
            addField<qreal>("y", "ipYCoord", &ImagePoint::yCoord, &ImagePoint::setYCoord);
            addField<qreal>("weight", "ipWeight", &ImagePoint::weight, &ImagePoint::setWeight);
            addField<QDateTime>("created", "ipCreated", &ImagePoint::created, &ImagePoint::setCreated);
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

signals:
    void idChanged();
    void imageIdChanged();
    void xCoordChanged();
    void yCoordChanged();
    void weightChanged();
    void createdChanged();

private:
    int m_appId = 0;
    int m_id = 0;
    int m_imageId = 0;
    qreal m_x = 0.0;
    qreal m_y = 0.0;
    qreal m_weight = 1.0;
    QDateTime m_created;
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

signals:
    void currentIndexChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
};


#endif // IMAGEPOINT_H
