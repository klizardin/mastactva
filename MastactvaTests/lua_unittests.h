#ifndef LUA_UNITTESTS_H
#define LUA_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <memory>
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
                && data_.b == b
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

TEST(Lua, base)
{
    using TestType = LuaAPIDataTest<DataTestData, typename std::remove_cv<decltype(g_DataTestDataLayout)>::type>;

    LuaAPI api;
    api.addTest(std::make_unique<TestType>
                    ("a", DataTestData{1, 0.0, ""}, g_DataTestDataLayout)
                );
    api.addTest(std::make_unique<TestType>
                    ("b", DataTestData{0, 2.0, ""}, g_DataTestDataLayout)
                );
    api.addTest(std::make_unique<TestType>
                    ("c", DataTestData{0, 0.0, "str"}, g_DataTestDataLayout)
                );

    api.load(
                QString(g_luaScriptBaseDataTestFmt).arg("a", "a", "1")
                );
    std::shared_ptr<TestObserverMock> mock = std::make_shared<TestObserverMock>();
    api.setTestObserver(mock);
    EXPECT_CALL(*mock, onTest(QString("a"), true));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStrs;
    api.call("main", nullptr, result, resultStrs);

    api.load(
                QString(g_luaScriptBaseDataTestFmt).arg("b", "b", "2.0")
                );
    EXPECT_CALL(*mock, onTest(QString("b"), true));
    api.call("main", nullptr, result, resultStrs);

    api.load(
                QString(g_luaScriptBaseDataTestFmt).arg("c", "c", "\"str\"")
                );
    EXPECT_CALL(*mock, onTest(QString("c"), true));
    api.call("main", nullptr, result, resultStrs);
}


#endif // LUA_UNITTESTS_H
