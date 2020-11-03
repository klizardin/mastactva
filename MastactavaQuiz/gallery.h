#ifndef GALLERY_H
#define GALLERY_H


#include <QObject>
#include <QDateTime>
#include <QString>
#include <QtQuick/QQuickItem>
#include "image.h"
#include "Layout.h"
#include "Model.h"


class Gallery : public QObject
{
    Q_OBJECT

public:
    explicit Gallery(QObject *parent = nullptr);

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(qreal pointsToPass READ pointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_PROPERTY(bool ownGallery READ ownGallery WRITE setOwnGallery NOTIFY ownGalleryChanged)
    Q_PROPERTY(QVariant images READ images NOTIFY imagesChanged)

public:
    class DefaultLayout : public LayoutBase<Gallery>
    {
    public:
        DefaultLayout()
        {
            addSpecial<int>(layout::SpecialFieldEn::appId, &Gallery::m_appId);
            addField<int>("id", "id", &Gallery::id, &Gallery::setId);
            addField<QString>("description", "description", &Gallery::description, &Gallery::setDescription);
            addField<QString>("keywords", "keywords", &Gallery::keyword, &Gallery::setKeyword);
            addField<QDateTime>("created", "created", &Gallery::created, &Gallery::setCreated);
            addField<qreal>("points_to_pass", "pointsToPass", &Gallery::pointsToPass, &Gallery::setPointsToPass);
            addField<int>("owner", "owner", &Gallery::ownGallery, &Gallery::setOwnGallery);
            addModel<ImageModel>("images", &Gallery::m_images, &Gallery::createImages);
        }
    };

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

signals:
    void idChanged();
    void descriptionChanged();
    void keywordChanged();
    void createdChanged();
    void pointsToPassChanged();
    void ownGalleryChanged();
    void imagesChanged();

protected:
    ImageModel *createImages();

private:
    int m_appId = -1;
    int m_id = -1;
    QString m_description;
    QString m_keywords;
    QDateTime m_created;
    qreal m_pointsToPass = 1.0;
    int m_ownGallery = false;
    ImageModel *m_images = nullptr;
};


class GalleryModel : public ListModelBaseOfData<Gallery>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Gallery>;

public:
    explicit GalleryModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

protected slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

signals:
    void currentIndexChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
};



#endif // GALLERY_H
