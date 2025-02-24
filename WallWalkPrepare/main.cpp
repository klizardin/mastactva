#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QDir>
#include <QStringList>
#include <QSet>
#include <QPair>
#include "../MastactvaBase/addonmodulelist.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"


bool findDynamicLibrariesDir(const QDir &dir_, QDir &result_)
{
    QFileInfoList files = dir_.entryInfoList(QStringList{} << "*.so", QDir::Files);
    if(!files.isEmpty())
    {
        result_ = dir_;
        return true;
    }
    const QFileInfoList dirs = dir_.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);
    for(const QFileInfo &fi_ : qAsConst(dirs))
    {
        if(!fi_.isDir())
        {
            continue;
        }
        if(findDynamicLibrariesDir(QDir(fi_.absoluteFilePath()), result_))
        {
            return true;
        }
    }
    return false;
}

static const char * g_inputJson =
        "{\"from_image\":\"%1\","
            "\"to_image\":\"%2\","
            "\"log_path\":\"%3\","
            "\"line_extract_min_area\":2E-2,"
            "\"line_extract_scale_coef\":2E-1,"
            "\"find_rects_min_rect_area\":4E-2,"
            "\"find_rects_max_rect_area\":5E-1,"
            "\"find_rects_rect_size_coef\":9E-1,"
            "\"find_rects_max_rect_count\":10,"
            "\"find_transform_size_coef\":5E-1,"
            "\"trace_operations_with_images\":false,"
            "\"trace_operations_with_messages\":false,"
            "\"mode\":\"generated_rects\","
            "\"test_is_convex\":true,"
            "\"generate_rects_rows\":4,"
            "\"generate_rects_cols\":4,"
            "\"rects\":[],"
            "\"output_rows\":10,"
            "\"output_cols\":10"
            "}";

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
        if(++i>bunchSize)
        {
            i = 1;
            bunchOfImages.push_back(QStringList());
        }
        bunchOfImages.back().push_back(imageName);
    }
    //qDebug() << "Bunch of images: " << bunchOfImages;
    QSet<QPair<QString, QString>> pairs;
    for(i = 0; i < bunchSize; i++)
    {
        for(int j = 1; j < bunchOfImages.size(); ++j)
        {
            //qDebug() << "pair (" << (j-1)*bunchSize + i << "," << j*bunchSize + i << "):" << bunchOfImages[j-1].at(i) << "-" << bunchOfImages[j].at(i);
            if(i >= 0 && i < bunchOfImages[j-1].size() && i < bunchOfImages[j].size())
            {
                pairs.insert(qMakePair(bunchOfImages[j-1].at(i),bunchOfImages[j].at(i)));
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
                pairs.insert(qMakePair(bunchOfImages[j].at(i-1),bunchOfImages[j].at(i)));
            }
        }
    }
    qInfo() << "All pairs : " << pairs.size();
    for(const auto& pair : pairs)
    {
        QDir addonsDir;
        findDynamicLibrariesDir(QDir("./"), addonsDir);
        auto modules = std::make_shared<AddonModules>();
        modules->create(addonsDir);

        const QString inputJson = QString(g_inputJson).arg(
                    sourceImageDir.filePath(pair.first),
                    sourceImageDir.filePath(pair.second),
                    absoluteHomePath("~/tmp/")
                    );

        QJsonDocument result = modules->call("WalkEffect", QJsonDocument::fromJson(inputJson.toUtf8()));
        qInfo() << result.isObject();
    }

    //return a.exec();
    return 0;
}
