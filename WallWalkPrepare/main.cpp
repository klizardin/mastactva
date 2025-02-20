#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QDir>
#include <QStringList>


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

    qInfo() << "Source directory: " << sourceDir << " bunch size: " << bunchSize;

    QDir sourceImageDir(sourceDir);
    QStringList images = sourceImageDir.entryList(QStringList() << "*.jpg" << "*.JPG", QDir::Files, QDir::Name);
    QVector<QStringList> bunchOfImages;
    bunchOfImages.push_back(QStringList());
    int i = 0;
    for(const QString& imageName : images)
    {
        if(++i>=bunchSize)
        {
            i = 0;
            bunchOfImages.push_back(QStringList());
        }
        bunchOfImages.back().push_back(imageName);
    }
    qInfo() << "Bunch of images: " << bunchOfImages;
    for(i = 0; i < bunchSize; i++)
    {
        for(int j = 1; j < bunchOfImages.size(); ++j)
        {
            qInfo() << "pair (" << j << "," << i<< "):" << bunchOfImages[j-1].at(i) << "-" << bunchOfImages[j].at(i);
        }
    }

    //return a.exec();
    return 0;
}
