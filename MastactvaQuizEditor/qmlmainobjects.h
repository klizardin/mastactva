#ifndef QMLMAINOBJECTS_H
#define QMLMAINOBJECTS_H

#include <QObject>


class GalleryImagesModel;
class GalleryEditViewModel;
class DescriptionModel;


class QMLMainObjects : public QObject
{
    Q_OBJECT
public:
    explicit QMLMainObjects(QObject *parent_ = nullptr);

    void setRootQMLObject(QObject* root_);
    GalleryEditViewModel *getGalleryViewModel();
    GalleryImagesModel *getGalleryAllImagesModel();
    DescriptionModel *getImageOfGalleryDescriptionModel();

    static void createSingelton(QObject *parent_ = nullptr);
    static QMLMainObjects *getSingelton();

private:
    void searchQMLObjects();

signals:

private:
    static QMLMainObjects *g_singelton;

    QObject* m_root = nullptr;
    GalleryEditViewModel *m_galleryViewModel = nullptr;
    GalleryImagesModel *m_galleryAllImagesModel = nullptr;
    DescriptionModel *m_imageOfGalleryDescriptionModel = nullptr;
};

#endif // QMLMAINOBJECTS_H
