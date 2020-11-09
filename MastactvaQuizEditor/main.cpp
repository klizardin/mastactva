#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QTranslator>
#include "netapi.h"
#include "galleryeditviewmodel.h"
#include "qmlmainobjects.h"
#include "mastactvaapi.h"


void switchTranslator(QGuiApplication &app, QTranslator& translator, const QString& filename)
{
    app.removeTranslator(&translator);
    if(translator.load(filename, "."))
    {
        app.installTranslator(&translator);
    }
}

int main(int argc, char *argv[])
{
    QTranslator m_translator; // contains the translations for this application
    QTranslator m_translatorQt; // contains the translations for qt

    QString lang("be_BY");
    QLocale locale = QLocale(lang);
    QLocale::setDefault(locale);
    QString languageName = QLocale::languageToString(locale.language());

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    switchTranslator(app, m_translator, QString("MastactvaQuizEditor_%1.qm").arg(lang));
    switchTranslator(app, m_translatorQt, QString("qt_%1.qm").arg(lang));

    app.setOrganizationName("Mastactva");
    app.setOrganizationDomain("mastactva.by");
    app.setApplicationName("Mastactva Quiz Editor Application");

    NetAPI::createSingelton(&app);
    QMLMainObjects::createSingelton(&app);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/Login.qml"));
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
