﻿/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef IMAGE_H
#define IMAGE_H


#include <QObject>
#include <QDateTime>
#include <QString>
#include <QtQuick/QQuickItem>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/imagepoint.h"
#include "../MastactvaModels/imagedescription.h"


class ImageModel;


class Image : public QObject, protected IListModelInfoObjectImpl, public IListModelItem
{
    Q_OBJECT
public:
    explicit Image(ImageModel *parent_ = nullptr);
    virtual ~Image() override;

    Q_PROPERTY(int imageId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString imageSource READ getFilename WRITE setFilenameStr NOTIFY filenameChanged)
    Q_PROPERTY(QString localImageSource READ localImageSource WRITE setLocalImageSource NOTIFY localImageSourceChanged)
    Q_PROPERTY(QString imageHash READ hash WRITE setHash NOTIFY hashChanged)
    Q_PROPERTY(int imageGallery READ gallery WRITE setGallery NOTIFY galleryChanged)
    Q_PROPERTY(bool imageTop READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QDateTime imageCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant imagePoints READ imagePoints WRITE setImagePoints NOTIFY imagePointsChanged)
    Q_PROPERTY(QVariant imageDescription READ imageDescription WRITE setImageDescription NOTIFY imageDescriptionChanged)

    Q_INVOKABLE bool isImageLoaded() const;
    Q_INVOKABLE void loadChildren();
    void downloadImage();

public:
    class DefaultLayout : public LayoutBase<Image>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image");
            addSpecial<int>(layout::SpecialFieldEn::appId, &Image::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Image::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &Image::m_objectModelInfo);
            addField<int>("id", "imageId", &Image::id, &Image::setId);
            addField<FileSource>("filename", "imageSource", &Image::filename, &Image::setFilename);
            addField<QString>("", "localImageSource", &Image::localImageSource, &Image::setLocalImageSource);
            addField<QString>("hash", "imageHash", &Image::hash, &Image::setHash);
            addField<int>("gallery", "imageGallery", &Image::gallery, &Image::setGallery);
            addField<bool>("use_in_gallery_view", "imageTop", &Image::top, &Image::setTop);
            addField<QDateTime>("created", "imageCreated", &Image::created, &Image::setCreated);
            addModel<ImagePointModel>("imagePoints", &Image::m_imagePointsModel, &Image::createImagePoints);
            /* 1:N */
            addModel<ImageDescriptionModel>("imageDescription", &Image::m_imageDescriptionModel, &Image::createImageDescriptionModel);
            /* 1:N */
            setIdField("id");
        }
    };

    int id() const;
    void setId(const int &id_);
    FileSource filename() const;
    void setFilename(const FileSource &filename_);
    QString getFilename() const;
    void setFilenameStr(const QString &filename_);
    QString localImageSource() const;
    void setLocalImageSource(const QString &url_);
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
    virtual void startLoadChildModel() override;
    virtual void endLoadChildModel() override;
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;
    virtual void listLoadedVF() override;
    QString getObjectName() const;

signals:
    void idChanged();
    void filenameChanged();
    void localImageSourceChanged();
    void hashChanged();
    void galleryChanged();
    void topChanged();
    void createdChanged();
    void imagePointsChanged();
    void imageDescriptionChanged();
    void imageLoaded();

protected slots:
    void imageDownloaded(const QString &url_);

private:
    ImageModel *m_imageModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = 0;
    int m_id = -1;
    FileSource m_filename;
    QString m_localImageUrl;
    QString m_hash;
    int m_gallery = -1;
    bool m_top = false;
    QDateTime m_created;
    ImagePointModel *m_imagePointsModel = nullptr;
    ImageDescriptionModel *m_imageDescriptionModel = nullptr;
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


#endif // IMAGE_H
