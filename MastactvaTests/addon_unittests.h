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
#include "test_utils.h"


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
