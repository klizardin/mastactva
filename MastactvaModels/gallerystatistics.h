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

#ifndef GALLERYSTATISTICS_H
#define GALLERYSTATISTICS_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/localdataapiviews.h"


class GalleryStatistics : public QObject, public IListModelItem
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
            setIdField("id");
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


class GalleryStatisticsModel : public ListModelBaseOfData<
        GalleryStatistics,
        GalleryStatisticsModel
        >
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<GalleryStatistics, GalleryStatisticsModel>;

public:
    explicit GalleryStatisticsModel(QObject *parent_ = nullptr);

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


class GalleryStatisticsModelView : public QObject, public LocalDataAPIGelListByRefImpl<GalleryStatisticsModel>
{
    Q_OBJECT
public:
    GalleryStatisticsModelView(QObject *parent_ = nullptr);
};


#endif // GALLERYSTATISTICS_H
