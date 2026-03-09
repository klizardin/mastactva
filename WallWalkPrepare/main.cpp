#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QDir>
#include <QStringList>
#include <QSet>
#include <QPair>
#include <QVector4D>
#include <QJsonObject>
#include <QJsonArray>
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
            "\"generate_rects_rows\":2,"
            "\"generate_rects_cols\":2,"
            "\"rects\":[],"
            "\"output_rows\":10,"
            "\"output_cols\":10"
            "}";

void convertJsonResultToCoordinates(const QJsonObject &object_, std::vector<QVector4D> &values_, QJsonArray& resultPack)
{
    QJsonArray empty;
    resultPack.swap(empty);
    values_.clear();
    for(int i = 0;;i++)
    {
        if(!object_.contains(QString::number(i)))
        {
            return;
        }
        QVector4D vec;
        const QJsonObject val = object_.value(QString::number(i)).toObject();
        if(!val.contains("x") || !val.contains("y"))
        {
            return;
        }
        QJsonObject resultVal;
        vec.setX(val.value("x").toDouble());
        vec.setY(val.value("y").toDouble());
        vec.setZ(-0.5);
        vec.setW(1.0);
        values_.push_back(vec);
        resultVal.insert("x", QJsonValue::fromVariant(QVariant::fromValue(vec.x())));
        resultVal.insert("y", QJsonValue::fromVariant(QVariant::fromValue(vec.y())));
        resultVal.insert("z", QJsonValue::fromVariant(QVariant::fromValue(vec.z())));
        resultVal.insert("w", QJsonValue::fromVariant(QVariant::fromValue(vec.w())));
        resultPack.append(resultVal);
    }
}


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

    QDir addonsDir;
    findDynamicLibrariesDir(QDir("./"), addonsDir);
    auto modules = std::make_shared<AddonModules>();
    modules->create(addonsDir);

    QJsonDocument result;
    QJsonArray jsonPairs;

    qInfo() << "All pairs : " << pairs.size();
    int index = 0;
    for(const auto& pair : pairs)
    {
        const QString inputJson = QString(g_inputJson).arg(
                    sourceImageDir.filePath(pair.first),
                    sourceImageDir.filePath(pair.second),
                    absoluteHomePath("~/tmp/")
                    );
        qInfo() << index << ":" << pair.first << "," << pair.second;

        QJsonDocument result = modules->call("WalkEffect", QJsonDocument::fromJson(inputJson.toUtf8()));

        std::vector<QVector4D> fromValues, toValues;
        QJsonArray fromValuesArray, toValuesArray;
        convertJsonResultToCoordinates(result.object().value("1").toObject(), fromValues, fromValuesArray);
        convertJsonResultToCoordinates(result.object().value("0").toObject(), toValues, toValuesArray);

        QJsonObject jsonPair;
        if(fromValues.empty() || toValues.empty())
        {
            //const QString inputJson2 = QString(g_inputJson).arg(
            //            sourceImageDir.filePath(pair.second),
            //            sourceImageDir.filePath(pair.first),
            //            absoluteHomePath("~/tmp/")
            //            );
            qInfo() << index << ":" << pair.second << "," << pair.first;

            QJsonDocument result = modules->call("WalkEffect", QJsonDocument::fromJson(inputJson.toUtf8()));

            convertJsonResultToCoordinates(result.object().value("0").toObject(), fromValues, fromValuesArray);
            convertJsonResultToCoordinates(result.object().value("1").toObject(), toValues, toValuesArray);

            if(!fromValues.empty() && !toValues.empty())
            {
                jsonPair.insert(g_demoJsonFieldFromFile, pair.second);
                jsonPair.insert(g_demoJsonFieldToFile, pair.first);
            }
        }
        else
        {
            jsonPair.insert(g_demoJsonFieldFromFile, pair.first);
            jsonPair.insert(g_demoJsonFieldToFile, pair.second);
        }

        if(!fromValues.empty() && !toValues.empty())
        {
            jsonPair.insert(g_demoJsonFieldFromValues, fromValuesArray);
            jsonPair.insert(g_demoJsonFieldToValues, toValuesArray);
            jsonPairs.append(jsonPair);
        }
        else
        {
            qWarning() << "not processed!" << index << ":" << pair.second << "," << pair.first;
        }
        ++index;
    }

    result.setArray(jsonPairs);
    QFile resultFile(sourceImageDir.filePath(g_demoConfigurationJsonFileName));
    resultFile.open(QFile::WriteOnly);
    resultFile.write(result.toJson());

    //return a.exec();
    return 0;
}
