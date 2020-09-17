#ifndef QMLMAINOBJECTS_H
#define QMLMAINOBJECTS_H

#include <QObject>

class GalleryEditViewModel;


class QMLMainObjects : public QObject
{
    Q_OBJECT
public:
    explicit QMLMainObjects(QObject *parent_ = nullptr);

    void setRootQMLObject(QObject* root_);
    GalleryEditViewModel *getGalleryViewModel();

    static void createSingelton(QObject *parent_ = nullptr);
    static QMLMainObjects *getSingelton();

private:
    void searchQMLObjects();

signals:

private:
    static QMLMainObjects *g_singelton;

    QObject* m_root = nullptr;
    GalleryEditViewModel *m_galleryViewModel = nullptr;
};

#endif // QMLMAINOBJECTS_H
