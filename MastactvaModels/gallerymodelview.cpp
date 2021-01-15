#include "gallerymodelview.h"


GalleryModelView::GalleryModelView()
{
}

bool GalleryModelView::canProcess(const DBRequestInfo *r_) const
{
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::getListImpl(DBRequestInfo *r_)
{
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    Q_UNUSED(appId_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::delItemImpl(const QVariant &id_, DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(r_);
    return false;
}
