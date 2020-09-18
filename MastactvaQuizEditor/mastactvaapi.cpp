#include "mastactvaapi.h"
#include <qqml.h>
#include "netapi.h"
#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"


MastactvaAPI::MastactvaAPI(QObject *parent) : QObject(parent)
{

}

void MastactvaAPI::reloadGalleriesModel()
{
    QMLMainObjects* qmlMainObjects = QMLMainObjects::getSingelton();
    Q_ASSERT(nullptr != qmlMainObjects);
    if(nullptr != qmlMainObjects)
    {
        GalleryEditViewModel *galleryModel = qmlMainObjects->getGalleryViewModel();
        Q_ASSERT(nullptr != galleryModel);
        if(nullptr != galleryModel)
        {
            galleryModel->startLoadGalleries();
        }
    }
}

void MastactvaAPI::loadGalleryViewImagesOfGallery()
{
    // TODO: implement method
}

void MastactvaAPI::loadAllImagesOfGallery()
{
    // TODO: implement method
}

int MastactvaAPI::getGalleryId() const
{
    return m_galleryId;
}

void MastactvaAPI::setGalleryId(int galleryId_)
{
    m_galleryId = galleryId_;
}

const QJsonDocument &MastactvaAPI::getJsonDoc() const
{
    return m_jsonDoc;
}

void MastactvaAPI::setJsonDoc(const QJsonDocument &jsonDoc_)
{
    m_jsonDoc = jsonDoc_;
}
