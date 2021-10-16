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

#ifndef GALLERYMODELVIEW_H
#define GALLERYMODELVIEW_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/userstep.h"


class GalleryModelView : public QObject, public ILocalDataGetUpdateAPI
{
    Q_OBJECT
public:
    GalleryModelView(QObject * parent_ = nullptr);
    virtual ~GalleryModelView() override;

    virtual bool canProcess(const DBRequestBase *r_) const override;
    virtual bool getListImpl(DBRequestBase *r_) override;
    virtual bool addItemImpl(const QVariant &appId_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool setItemImpl(const QVariant &id_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool delItemImpl(const QVariant &id_, DBRequestBase *r_) override;

protected:
    void loadSteps();
    void setData(DBRequestBase *request_);

protected slots:
    void userStepModelListReloaded();
    void userStepPlayedGallriesModelListReloaded();
    void galleryModelListReloaded();

private:
    QList<DBRequestBase *> m_requests;
    const UserStep *m_lastUserStep = nullptr;
    UserStepModel *m_userStepModel;
    UserStepModel *m_userStepPlayedGalleriesModel;
    GalleryModel *m_galleryModel;
};

#endif // GALLERYMODELVIEW_H
