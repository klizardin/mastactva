#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "../MastactvaBase/quizimageqwindowsinglethreaddemo.h"
#include "../MastactvaBase/names.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("wall-walk-prepare-data");
    QCoreApplication::setApplicationVersion("0.1");

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
    QVector<QStringList> bunchOfImages;
    bunchOfImages.push_back(QStringList());
    int i = 0;
    for(const QString& imageName : images)
    {
        if(++i>bunchSize)
        {
            i = 1;
            bunchOfImages.push_back(QStringList());
        }
        bunchOfImages.back().push_back(imageName);
    }
    //qDebug() << "Bunch of images: " << bunchOfImages;
    QVector<QPair<QString, QString>> pairs;
    for(i = 0; i < bunchSize; i++)
    {
        for(int j = 1; j < bunchOfImages.size(); ++j)
        {
            //qDebug() << "pair (" << (j-1)*bunchSize + i << "," << j*bunchSize + i << "):" << bunchOfImages[j-1].at(i) << "-" << bunchOfImages[j].at(i);
            if(i >= 0 && i < bunchOfImages[j-1].size() && i < bunchOfImages[j].size())
            {
                pairs.push_back(qMakePair(bunchOfImages[j-1].at(i),bunchOfImages[j].at(i)));
            }
        }
    }
    for(int j = 0; j < bunchOfImages.size(); ++j)
    {
        for(i = 1; i < bunchSize; i++)
        {
            //qDebug() << "pair (" << j*bunchSize + i - 1 << "," << j*bunchSize + i << "):" << bunchOfImages[j].at(i - 1) << "-" << bunchOfImages[j].at(i);
            if(i>=1 && i < bunchOfImages[j].size())
            {
                pairs.push_back(qMakePair(bunchOfImages[j].at(i-1),bunchOfImages[j].at(i)));
            }
        }
    }

    qInfo() << "All pairs : " << pairs.size();

    QJsonDocument configuration;
    QFile configurationFile(g_demoConfigurationJsonFileName);
    configurationFile.open(QFile::ReadOnly);
    configuration.fromJson(configurationFile.read(configurationFile.size()));

    QuizImageQWindowSingleThread window(QStringLiteral("qrc:/qml/quizImage0.qml"), false);
    // set default size
    window.resize(800, 600);
    // show
    window.show();

    return app.exec();
}
