#ifndef GALLERYSTATISTICS_H
#define GALLERYSTATISTICS_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class GalleryStatistics : public QObject
{
    Q_OBJECT
public:
    explicit GalleryStatistics(QObject *parent_ = nullptr);

    Q_PROPERTY(int ugsId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int ugsUserId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(int ugsGalleryId READ galleryId WRITE setGalleryId NOTIFY galleryIdChanged)
    Q_PROPERTY(qreal ugsPoints READ points WRITE setPoints NOTIFY pointsChanged)


    class DefaultLayout : public LayoutBase<GalleryStatistics>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("quiz-user-galleries-stat");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &GalleryStatistics::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &GalleryStatistics::m_appId);
            addField<int>("id", "ugsId", &GalleryStatistics::id, &GalleryStatistics::setId);
            addField<int>("user", "ugsUserId", &GalleryStatistics::userId, &GalleryStatistics::setUserId);
            addField<int>("gallery", "ugsGalleryId", &GalleryStatistics::galleryId, &GalleryStatistics::setGalleryId);
            addField<qreal>("points", "ugsPoints", &GalleryStatistics::points, &GalleryStatistics::setPoints);
        }
    };


    int id() const;
    void setId(const int &id_);
    int userId() const;
    void setUserId(const int &userId_);
    int galleryId() const;
    void setGalleryId(const int &galleryId_);
    qreal points() const;
    void setPoints(const qreal &points_);

signals:
    void idChanged();
    void userIdChanged();
    void galleryIdChanged();
    void pointsChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_userId = -1;
    int m_galleryId = -1;
    qreal m_points = 0.0;
};


class GalleryStatisticsModel : public ListModelBaseOfData<GalleryStatistics, GalleryStatisticsModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<GalleryStatistics, GalleryStatisticsModel>;

public:
    explicit GalleryStatisticsModel(QObject *parent_ = nullptr);

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
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void error(int errorCode_, const QJsonDocument &reply_);
};


#endif // GALLERYSTATISTICS_H
