#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QtQuick/QQuickItem>
#include "imagesource.h"
#include "Layout.h"
#include "Model.h"


class Image : public QObject
{
    Q_OBJECT
public:
    explicit Image(QObject *parent_ = nullptr);

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(ImageSource filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString hash READ hash WRITE setHash NOTIFY hashChanged)
    Q_PROPERTY(int gallery READ gallery WRITE setGallery NOTIFY galleryChanged)
    Q_PROPERTY(bool top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)

public:
    class DefaultLayout : public LayoutBase<Image>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("images");
            addSpecial<QObject *>(layout::SpecialFieldEn::appId, &Image::m_appId);
            addField<int>("id", "id", &Image::id, &Image::setId);
            addField<ImageSource>("filename", "imageSource", &Image::filename, &Image::setFilename);
            addField<QString>("hash", "hash", &Image::hash, &Image::setHash);
            addField<int>("gallery", "galleryId", &Image::gallery, &Image::setGallery);
            addField<bool>("use_in_gallery_view", "top", &Image::top, &Image::setTop);
        }
    };

    int id() const;
    void setId(const int &id_);
    ImageSource filename() const;
    void setFilename(const ImageSource &filename_);
    QString hash() const;
    void setHash(const QString &hash_);
    int gallery() const;
    void setGallery(const int &gallery_);
    bool top() const;
    void setTop(const bool &top_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);

signals:
    void idChanged();
    void filenameChanged();
    void hashChanged();
    void galleryChanged();
    void topChanged();
    void createdChanged();

private:
    QObject *m_appId = nullptr;
    int m_id = -1;
    ImageSource m_filename;
    QString m_hash;
    int m_gallery = -1;
    bool m_top = false;
    QDateTime m_created;
};

class ImageModel : public ListModelBaseOfData<Image>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Image>;

public:
    explicit ImageModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

signals:
    void currentIndexChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
};


#endif // IMAGE_H
