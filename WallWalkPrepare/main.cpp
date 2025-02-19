#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("wall-walk-prepare-data");
    QCoreApplication::setApplicationVersion("0.1");


    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "WallWalkPrepare_" + QLocale(locale).name();
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

    qInfo() << "Source directory: " << sourceDir << " bunch size: " << bunchSize << "\n";

    //return a.exec();
    return 0;
}
