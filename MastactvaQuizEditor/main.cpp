#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "netapi.h"
#include "galleryeditviewmodel.h"
#include "qmlmainobjects.h"
#include "mastactvaapi.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    NetAPI::createSingelton(&app);
    NetAPI::getSingelton()->getDefaultRequestData().setAuth("demo", "sBnd4nPU28BWvN7");
    NetAPI::getSingelton()->getDefaultRequestData().setUrlBase("http://localhost:8000/");
    QMLMainObjects::createSingelton(&app);

    qmlRegisterType<GalleryEditViewModel>("org.mastactva", 1, 0, "GalleryEditViewModel");
    qmlRegisterType<GalleryImagesModel>("org.mastactva", 1, 0, "GalleryImagesModel");
    qmlRegisterType<MastactvaAPI>("org.mastactva", 1, 0, "MastactvaAPI");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if(url == objUrl)
        {
            if (nullptr == obj)
            {
                QCoreApplication::exit(-1);
            }
            else
            {
                QMLMainObjects *mainObjects = QMLMainObjects::getSingelton();
                Q_ASSERT(nullptr != mainObjects);
                if(nullptr != mainObjects)
                {
                    mainObjects->setRootQMLObject(obj);
                }
            }
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();

}
