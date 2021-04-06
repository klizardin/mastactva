#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QTranslator>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/netappconsts.h"
//#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"


void switchTranslator(QGuiApplication &app, QTranslator& translator, const QString& filename)
{
    app.removeTranslator(&translator);
    if(translator.load(filename, "."))
    {
        app.installTranslator(&translator);
    }
}

void switchLanguage(const QString & lang_, QTranslator& translator_, QTranslator& translatorQt_, QGuiApplication &app_)
{
    QString lang;
    if(g_belarusLanguage == lang_)
    {
        lang = "be_BY";
    }
    else if(g_englishLanguage == lang_)
    {
        lang = "en_US";
    }
    else
    {
        return;
    }

    QLocale locale = QLocale(lang);
    QLocale::setDefault(locale);

    switchTranslator(app_, translator_, QString("ShaderTest_%1.qm").arg(lang));
    switchTranslator(app_, translatorQt_, QString("qt_%1.qm").arg(lang));
}


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QTranslator translator(&app);
    QTranslator translatorQt(&app);
    switchLanguage(g_belarusLanguage, translator, translatorQt, app);

    app.setOrganizationName("ShaderTest");
    app.setOrganizationDomain("mastactva.by");
    app.setApplicationName("Shader Test For The Mastactva Quiz");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.addImportPath("qrc:/ShaderTestQML");
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
                //QMLObjects::getInstance().setRoot(obj);
            }
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
