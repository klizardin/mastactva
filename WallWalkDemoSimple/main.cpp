#include <QGuiApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "quizimageqwindowsinglethreaddemo.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils_file.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("wall-walk-prepare-data");
    QGuiApplication::setApplicationVersion("0.1");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "WallWalkDemoSimple_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QCommandLineParser cmdsParser;
    cmdsParser.setApplicationDescription("Preparing data for the folder of images");
    cmdsParser.addHelpOption();
    cmdsParser.addVersionOption();

    QCommandLineOption sourceDirOption(
                QStringList() << "d" << "directory",
                QCoreApplication::translate("main", "Work with <directory> of images."),
                QCoreApplication::translate("main", "directory")
                );
    cmdsParser.addOption(sourceDirOption);
    QCommandLineOption bunchSizeOption(
                QStringList() << "b" << "bunch",
                QCoreApplication::translate("main", "Size of the <bunch> of the images"),
                QCoreApplication::translate("main", "bunch")
                );
    cmdsParser.addOption(bunchSizeOption);
    cmdsParser.process(app);

    const QString sourceDir = cmdsParser.value(sourceDirOption);
    const int bunchSize = cmdsParser.value(bunchSizeOption).toInt();

    qInfo() << "Source directory: " << sourceDir << " bunch size: " << bunchSize;

    QDir sourceImageDir(sourceDir);
    QStringList images = sourceImageDir.entryList(QStringList() << "*.jpg" << "*.JPG", QDir::Files, QDir::Name);

    const auto bunchOfImages = getBunchOfImages(images, bunchSize);
    //qDebug() << "Bunch of images: " << bunchOfImages;
    const auto pairs = getImagesPairs(bunchOfImages, bunchSize);
    qInfo() << "All pairs : " << pairs.size();

    QJsonDocument configuration;
    const QString configurationFilename = sourceImageDir.filePath(g_demoConfigurationJsonFileName);
    QFile configurationFile(configurationFilename);
    configurationFile.open(QFile::ReadOnly | QFile::Text);
    QJsonParseError jsonError;
    const QByteArray configurationData = configurationFile.readAll();
    configuration = QJsonDocument::fromJson(configurationData, &jsonError);
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << jsonError.errorString();
        return -1;
    }

    QuizImageQWindowSingleThread window(QStringLiteral("qrc:/qml/quizImage.qml"), false);
    QVector<QPair<QString, QString>> resultPairs;
    auto coordinates = getCoordinates(configuration, resultPairs);
    window.initData(resultPairs, coordinates, sourceImageDir);
    // set default size
    window.resize(800, 600);
    // show
    window.show();

    return app.exec();
}
