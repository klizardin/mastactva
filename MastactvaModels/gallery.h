#ifndef GALLERY_H
#define GALLERY_H


#include <QObject>
#include <QDateTime>
#include <QString>
#include <QtQuick/QQuickItem>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/image.h"
#include "../MastactvaModels/gallerystatistics.h"


class GalleryModel;


class Gallery : public QObject, public IListModelItem
{
    Q_OBJECT

public:
    explicit Gallery(GalleryModel *parent = nullptr);
    virtual ~Gallery() override;

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString keywords READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(qreal pointsToPass READ pointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_PROPERTY(bool ownGallery READ ownGallery WRITE setOwnGallery NOTIFY ownGalleryChanged)
    Q_PROPERTY(QVariant images READ images NOTIFY imagesChanged)
    Q_PROPERTY(QVariant galleryStatistics READ statistics WRITE setStatistics NOTIFY statisticsChanged)

public:
    class DefaultLayout : public LayoutBase<Gallery>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("gallery");
            addSpecial<int>(layout::SpecialFieldEn::appId, &Gallery::m_appId);
            addField<int>("id", "id", &Gallery::id, &Gallery::setId);
            addField<QString>("description", "description", &Gallery::description, &Gallery::setDescription);
            addField<QString>("keywords", "keywords", &Gallery::keyword, &Gallery::setKeyword);
            addField<QDateTime>("created", "created", &Gallery::created, &Gallery::setCreated);
            addField<qreal>("points_to_pass", "pointsToPass", &Gallery::pointsToPass, &Gallery::setPointsToPass);
            addField<int>("owner", "owner", &Gallery::ownGallery, &Gallery::setOwnGallery);
            addModel<ImageModel>("images", &Gallery::m_imagesModel, &Gallery::createImages);
            /* 1:N */
            addModel<GalleryStatisticsModel>("galleryStatistics", &Gallery::m_galleryStatisticsModel, &Gallery::createGalleryStatistics);
            /* 1:N(1) */
            setIdField("id");
        }
    };

    int getAppId() const;
    int id() const;
    void setId(const int &id_);
    QString description() const;
    void setDescription(const QString &description_);
    QString keyword() const;
    void setKeyword(const QString &keyword_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    qreal pointsToPass() const;
    void setPointsToPass(const qreal &pointToPass_);
    int ownGallery() const;
    void setOwnGallery(const int &ownGallery_);
    QVariant images();
    QVariant statistics() const;
    void setStatistics(const QVariant &obj_);

signals:
    void idChanged();
    void descriptionChanged();
    void keywordChanged();
    void createdChanged();
    void pointsToPassChanged();
    void ownGalleryChanged();
    void imagesChanged();
    void statisticsChanged();

protected:
    ImageModel *createImages();
    GalleryStatisticsModel *createGalleryStatistics();

private:
    GalleryModel *m_galleryModel = nullptr;
    int m_appId = 0;
    int m_id = -1;
    QString m_description;
    QString m_keywords;
    QDateTime m_created;
    qreal m_pointsToPass = 1.0;
    int m_ownGallery = false;
    ImageModel *m_imagesModel = nullptr;
    GalleryStatisticsModel *m_galleryStatisticsModel = nullptr;
};


class GalleryModel : public ListModelBaseOfData<Gallery, GalleryModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Gallery, GalleryModel>;

public:
    explicit GalleryModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString &errorCodeStr_,
                          RequestData *request_,
                          const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, errorCodeStr_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentItemChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void refValueChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void autoCreateChildrenModelsOnSelectChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void sortFieldsChanged();
    void readonlyChanged();
    void objectNameDerivedChanged();
    void error(const QString &errorCode_, const QString &description_);
};


#endif // GALLERY_H
