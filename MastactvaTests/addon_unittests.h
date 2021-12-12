#ifndef ADDON_UNITTESTS_H
#define ADDON_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <QJsonObject>
#include <QJsonDocument>
#include "../MastactvaBase/addonmodule.h"
#include "../MastactvaBase/addonmodulelist.h"
#include "../MastactvaBase/mergeid.h"


bool findDynLibs(const QDir &dir_, QDir &result_)
{
    QFileInfoList files = dir_.entryInfoList(QStringList{} << "*.so", QDir::Files);
    if(!files.isEmpty())
    {
        result_ = dir_;
        return true;
    }
    QFileInfoList dirs = dir_.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);
    for(const QFileInfo &fi_ : dirs)
    {
        if(!fi_.isDir())
        {
            continue;
        }
        if(findDynLibs(QDir(fi_.absoluteFilePath()), result_))
        {
            return true;
        }
    }
    return false;
}


TEST(Addon, base)
{
    QDir addonsDir;
    ASSERT_TRUE(findDynLibs(QDir("../"), addonsDir));

    AddonModules modules;
    ASSERT_TRUE(modules.create(addonsDir));
    QJsonObject obj;
    obj.insert("key", QJsonValue::fromVariant(QVariant::fromValue(QString("value"))));
    obj.insert("random", QJsonValue::fromVariant(QVariant::fromValue(static_cast<const QString &>(MergeId{}))));
    QJsonDocument args(obj);
    QJsonDocument result = modules.call("echo", args);
    ASSERT_EQ(args, result);

    modules.setDefault("echo");
    QJsonDocument result1 = modules.call("unknown", args);
    ASSERT_EQ(args, result1);
}


#endif // ADDON_UNITTESTS_H
