#ifndef GALLERYMODELVIEW_H
#define GALLERYMODELVIEW_H


#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/userstep.h"


class GalleryModelView : public ILocalDataAPI
{
public:
    GalleryModelView();
    virtual ~GalleryModelView() = default;

    virtual bool canProcess(const DBRequestInfo *r_) const override;
    virtual bool getListImpl(DBRequestInfo *r_) override;
    virtual bool addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_) override;
    virtual bool setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_) override;
    virtual bool delItemImpl(const QVariant &id_, DBRequestInfo *r_) override;

private:
    UserStepModel m_userStepModel;
    GalleryModel m_galleryModel;
};

#endif // GALLERYMODELVIEW_H
