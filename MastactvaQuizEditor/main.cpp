#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "netapi.h"
#include "galleryeditviewmodel.h"
#include "qmlmainobjects.h"
#include "mastactvaapi.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("Mastactva");
    app.setOrganizationDomain("mastactva.by");
    app.setApplicationName("Mastactva Quiz Editor Application");

    NetAPI::createSingelton(&app);
    NetAPI::getSingelton()->getDefaultRequestData().setAuth("demo", "5jNicLc36GzUAV3");
    NetAPI::getSingelton()->getDefaultRequestData().setUrlBase("http://localhost:8000/");
    QMLMainObjects::createSingelton(&app);

    //qmlRegisterType<GalleryEditViewModel>("org.mastactva", 1, 0, "GalleryEditViewModel");
    //qmlRegisterType<GalleryImagesModel>("org.mastactva", 1, 0, "GalleryImagesModel");
    //qmlRegisterType<MastactvaAPI>("org.mastactva", 1, 0, "MastactvaAPI");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.addImportPath("qrc:/Mastactva");
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
