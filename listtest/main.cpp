#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "netapi.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    NetAPI::createSingelton(&app);
    NetAPI::getSingelton()->getDefaultRequestData().setAuth("demo", "sBnd4nPU28BWvN7");
    NetAPI::getSingelton()->getDefaultRequestData().setUrlBase("http://localhost:8000/");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
