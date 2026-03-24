#include <QGuiApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "../MastactvaBase/quizimageqwindowsinglethreaddemo.h"
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
    QFile configurationFile(sourceImageDir.filePath(g_demoConfigurationJsonFileName));
    configurationFile.open(QFile::ReadOnly);
    configuration.fromJson(configurationFile.read(configurationFile.size()));

    QuizImageQWindowSingleThread window(QStringLiteral("qrc:/qml/quizImage.qml"), false);
    // set default size
    window.resize(800, 600);
    // show
    window.show();

    return app.exec();
}
