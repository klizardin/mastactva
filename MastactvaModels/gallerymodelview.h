#ifndef GALLERYMODELVIEW_H
#define GALLERYMODELVIEW_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
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
