#include "qmlmainobjects.h"
#include "galleryeditviewmodel.h"


QMLMainObjects::QMLMainObjects(QObject *parent_) : QObject(parent_)
{
}

QMLMainObjects *QMLMainObjects::g_singelton = nullptr;

void QMLMainObjects::createSingelton(QObject *parent_ /*= nullptr*/)
{
    Q_ASSERT(nullptr == g_singelton);
    if(!(nullptr == g_singelton))
    {
        return;
    }
    g_singelton = new QMLMainObjects(parent_);
}

QMLMainObjects *QMLMainObjects::getSingelton()
{
    return g_singelton;
}

GalleryEditViewModel *QMLMainObjects::getGalleryViewModel()
{
    if(nullptr == m_galleryViewModel)
    {
        searchQMLObjects();
    }
    Q_ASSERT(nullptr != m_galleryViewModel);
    return m_galleryViewModel;
}

void QMLMainObjects::searchQMLObjects()
{
    if(nullptr == m_galleryViewModel)
    {
        m_galleryViewModel = m_root->findChild<GalleryEditViewModel *>("GalleryModel");
    }
}

void QMLMainObjects::setRootQMLObject(QObject* root_)
{
    m_root = root_;
}
