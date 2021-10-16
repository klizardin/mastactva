/*
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

#ifndef IMAGEDESCRIPTION_H
#define IMAGEDESCRIPTION_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class ImageDescription : public QObject, public IListModelItem
{
    Q_OBJECT
public:
    explicit ImageDescription(QObject *parent_ = nullptr);

    Q_PROPERTY(int idId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int idImageId READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QDateTime idFromDate READ fromDate WRITE setFromDate NOTIFY fromDateChanged)
    Q_PROPERTY(QString idDescriptionText READ description WRITE setDescription NOTIFY descriptionChanged)


    class DefaultLayout : public LayoutBase<ImageDescription>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-description");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ImageDescription::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImageDescription::m_appId);
            addField<int>("id", "idId", &ImageDescription::id, &ImageDescription::setId);
            addField<int>("image", "idImageId", &ImageDescription::image, &ImageDescription::setImage);
            addField<QDateTime>("from_field", "idFromDate", &ImageDescription::fromDate, &ImageDescription::setFromDate);
            addField<QString>("description", "idDescriptionText", &ImageDescription::description, &ImageDescription::setDescription);
            setIdField("id");
        }
    };


    int id() const;
    void setId(const int &id_);
    int image() const;
    void setImage(const int &imageId_);
    QDateTime fromDate() const;
    void setFromDate(const QDateTime &fromDate_);
    QString description() const;
    void setDescription(const QString &descriptionText_);

signals:
    void idChanged();
    void imageChanged();
    void fromDateChanged();
    void descriptionChanged();

private:
    IListModelInfo *m_parentModelInfo = 0;
    int m_appId = -1;
    int m_id = -1;
    int m_imageId = -1;
    QDateTime m_fromDate;
    QString m_descriptionText;
};


class ImageDescriptionModel : public ListModelBaseOfData<ImageDescription, ImageDescriptionModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ImageDescription, ImageDescriptionModel>;

public:
    explicit ImageDescriptionModel(QObject *parent_ = nullptr);

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


#endif // IMAGEDESCRIPTION_H
