#ifndef LUA_UNITTESTS_H
#define LUA_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <memory>
#include <math.h>
#include <type_traits>
#include <QString>
#include "lua_utils.h"
#include "luaapi.h"
#include "test_utils.h"


using namespace testing;


static const char *g_luaScriptBaseDataTestFmt =
        "function main ()\n"
        "    result = {}\n"
        "    result[\"%2\"] = %3\n"
        "    test(result, \"%1\")\n"
        "end\n";

static const char *g_luaScriptBaseDataTest3Fmt =
        "function main ()\n"
        "    result = {}\n"
        "    result[\"%2\"] = %3\n"
        "    result[\"%4\"] = %5\n"
        "    result[\"%6\"] = %7\n"
        "    test(result, \"%1\")\n"
        "end\n";

struct DataTestData
{
    int a = 0;
    double b = 0.0;
    QString c;

    DataTestData(int a_ = 0, double b_ = 0.0, const QString &c_ = QString{})
        : a(a_), b(b_), c(c_)
    {
    }

    bool operator == (const DataTestData &data_) const
    {
        return data_.a == a
                && fabs(data_.b - b) <= std::max(fabs(data_.b), fabs(b)) * 1e-6
                && data_.c == c
                ;
    }
};

static const auto g_DataTestDataLayout = makeDataLayout(
            makeFieldLayout("a", &DataTestData::a),
            makeFieldLayout("b", &DataTestData::b),
            makeFieldLayout("c", &DataTestData::c)
            );

class TestObserverMock : public TestObserver
{
public:
    MOCK_METHOD(void, onTest, (const QString &, bool), (const ,override));
};

TEST(Lua, utils)
{
    using TestType = LuaAPIDataTest<DataTestData, typename std::remove_cv<decltype(g_DataTestDataLayout)>::type>;

    LuaAPI api;
    api.addTest(std::make_unique<TestType>
                    ("t1", DataTestData{2, 0.0, ""}, g_DataTestDataLayout)
                );
    api.addTest(std::make_unique<TestType>
                    ("t2", DataTestData{0, 3.0, ""}, g_DataTestDataLayout)
                );
    api.addTest(std::make_unique<TestType>
                    ("t3", DataTestData{0, 0.0, "str"}, g_DataTestDataLayout)
                );
    api.addTest(std::make_unique<TestType>
                    ("t4", DataTestData{-5, 10.5, "somestr"}, g_DataTestDataLayout)
                );

    std::shared_ptr<TestObserverMock> mock = std::make_shared<TestObserverMock>();
    api.setTestObserver(mock);

    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStrs;

    api.load(QString(g_luaScriptBaseDataTestFmt).arg("t1", "a", "2"));
    EXPECT_CALL(*mock, onTest(QString("t1"), true));
    api.call("main", nullptr, result, resultStrs);

    api.load(QString(g_luaScriptBaseDataTestFmt).arg("t2", "b", "3.0"));
    EXPECT_CALL(*mock, onTest(QString("t2"), true));
    api.call("main", nullptr, result, resultStrs);

    api.load(QString(g_luaScriptBaseDataTestFmt).arg("t3", "c", "\"str\""));
    EXPECT_CALL(*mock, onTest(QString("t3"), true));
    api.call("main", nullptr, result, resultStrs);

    api.load(QString(g_luaScriptBaseDataTest3Fmt).arg("t4", "a", "-5", "b", "10.5", "c", "\"somestr\""));
    EXPECT_CALL(*mock, onTest(QString("t4"), true));
    api.call("main", nullptr, result, resultStrs);
}


TEST(Lua, utilDetails)
{
    QJsonObject obj1;
    obj1.insert("2", QJsonValue::fromVariant(QVariant::fromValue(1)));
    obj1.insert("1", QJsonValue::fromVariant(QVariant::fromValue(2)));
    EXPECT_TRUE(detail::isArray(obj1));

    QJsonObject obj2;
    obj2.insert("2str", QJsonValue::fromVariant(QVariant::fromValue(1)));
    obj2.insert("1", QJsonValue::fromVariant(QVariant::fromValue(2)));
    EXPECT_FALSE(detail::isArray(obj2));

    QJsonObject obj3;
    obj3.insert("3", QJsonValue::fromVariant(QVariant::fromValue(1)));
    obj3.insert("1", QJsonValue::fromVariant(QVariant::fromValue(2)));
    EXPECT_FALSE(detail::isArray(obj3));

    QJsonObject obj4;
    obj4.insert("1", QJsonValue::fromVariant(QVariant::fromValue(1)));
    obj4.insert("0", QJsonValue::fromVariant(QVariant::fromValue(2)));
    EXPECT_FALSE(detail::isArray(obj4));
}


class AddonTestData
{
public:
    QString key;
    QString random;

    bool operator == (const AddonTestData &data_) const
    {
        return data_.key == key
                && data_.random == random
                ;
    }
};

static const auto g_AddonTestDataLayout = makeDataLayout(
            makeFieldLayout("key", &AddonTestData::key),
            makeFieldLayout("random", &AddonTestData::random)
            );

static const char *g_luaScriptAddonDataTestFmt =
        "function main ()\n"
        "    arg = {}\n"
        "    arg[\"%2\"] = \"%3\"\n"
        "    arg[\"%4\"] = \"%5\"\n"
        "    result = addon.call(arg, \"%6\")\n"
        "    test(result, \"%1\")\n"
        "end\n";


class AddonTestData2
{
public:
    bool value = false;

    bool operator == (const AddonTestData2 &data_) const
    {
        return data_.value == value
                ;
    }
};

static const auto g_AddonTestData2Layout = makeDataLayout(
            makeFieldLayout("value", &AddonTestData2::value)
            );

static const char *g_luaScriptAddonDataTest2Fmt =
        "function main ()\n"
        "    result = {}\n"
        "    result[\"%2\"] = addon.hasName(\"%3\")\n"
        "    test(result, \"%1\")\n"
        "end\n";

static const char *g_luaScriptAddonDataTest3Fmt =
        "function main ()\n"
        "    result[\"0\"] = addon.getNames()\n"
        "    return result\n"
        "end\n";


TEST(Lua, addons)
{
    QDir addonsDir;
    ASSERT_TRUE(findDynLibs(QDir("../"), addonsDir));

    auto modules = std::make_shared<AddonModules>();
    ASSERT_TRUE(modules->create(addonsDir));

    MergeId randomValue{};
    QString value{"value"};

    using TestType = LuaAPIDataTest<AddonTestData,
        typename std::remove_cv<decltype(g_AddonTestDataLayout)>::type
        >;
    using TestType2 = LuaAPIDataTest<AddonTestData2,
        typename std::remove_cv<decltype(g_AddonTestData2Layout)>::type
        >;


    LuaAPI api;
    api.addTest(std::make_unique<TestType>
                    ("t1", AddonTestData{value, randomValue}, g_AddonTestDataLayout)
                );
    api.addTest(std::make_unique<TestType2>
                    ("t2", AddonTestData2{false}, g_AddonTestData2Layout)
                );
    api.addTest(std::make_unique<TestType2>
                    ("t3", AddonTestData2{true}, g_AddonTestData2Layout)
                );

    std::shared_ptr<TestObserverMock> mock = std::make_shared<TestObserverMock>();
    api.setTestObserver(mock);

    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStrs;

    auto test1 = [&api, &value, &randomValue]()
    {
        api.load(QString(g_luaScriptAddonDataTestFmt)
                 .arg(
                     "t1",
                     "key", value,
                     "random", static_cast<const QString &>(randomValue),
                     "echo"
                    )
                 );
    };
    test1();
    EXPECT_CALL(*mock, onTest(QString("t1"), false));
    api.call("main", nullptr, result, resultStrs);

    auto test2 = [&api](const char * testName_)
    {
        api.load(QString(g_luaScriptAddonDataTest2Fmt).arg(testName_, "value", "echo"));
    };
    test2("t2");
    EXPECT_CALL(*mock, onTest(QString("t2"), true));
    api.call("main", nullptr, result, resultStrs);

    api.set(modules);

    test1();
    EXPECT_CALL(*mock, onTest(QString("t1"), true));
    api.call("main", nullptr, result, resultStrs);

    test2("t3");
    EXPECT_CALL(*mock, onTest(QString("t3"), true));
    api.call("main", nullptr, result, resultStrs);

    api.load(g_luaScriptAddonDataTest3Fmt);
    api.call("main", nullptr, result, resultStrs);
    std::map<QString, QStringList> resultStrsExpected;
    resultStrsExpected.insert({QString("0"), QStringList{} << "echo"});
    EXPECT_EQ(resultStrs, resultStrsExpected);
}


#endif // LUA_UNITTESTS_H
