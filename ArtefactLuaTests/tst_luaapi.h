#ifndef TST_LUAAPI_H
#define TST_LUAAPI_H


#include <map>
#include <QString>
#include <QVector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include "luaapi.h"


using namespace testing;


namespace
{
    const char *g_simpleCallTestCode =
            "function main ()\n"
            "   result = {}\n"
            "   result[\"a\"] = {1,2,3}\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_functionName = "main";
    const char *g_variablesCallTestCode =
            "function main ()\n"
            "   result = {}\n"
            "   result[\"a\"] = getVariable(\"a\")\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_setVariablesCallTestCode =
            "function main ()\n"
            "   setVariable(\"b\", getVariable(\"a\"))\n"
            "   return {}\n"
            "end\n"
            ;
    const char *g_setVariablesStrsCallTestCode =
            "function main ()\n"
            "   setVariable(\"b\", getVariable(\"a\"))\n"
            "   strResult = {\"1.0\", \"b\", \"c\"}\n"
            "   result = {}\n"
            "   result[\"a\"] = strResult\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_badLibCallTestCode =
            "os.execute()\n"
            ;
    const char *g_badLibCallTest2Code =
            "debug.debug()\n"
            ;

    std::map<QString, QVector<double>> g_variables =
    {
        {
            QString("a"),
            {1.0, 2.0, 3.0}
        },
    };
    std::map<QString, QVector<double>> g_empty;
    std::map<QString, QStringList> g_emptyStrs;
    std::map<QString, QStringList> g_variablesStrs =
    {
        {
            QString("a"),
            {QString("1.0"), QString("b"), QString("c")}
        },
    };
}


TEST(LuaAPI, callReturn)
{
    LuaAPI luaAPI;
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_TRUE(luaAPI.load(g_simpleCallTestCode));
    EXPECT_TRUE(luaAPI.call(g_functionName, result, resultStr));
    ASSERT_THAT(result, Eq(g_variables));
}


class VariablesGetterMock : public IVariablesGetter
{
public:
    MOCK_METHOD(bool, get, (const QString &, QVector<double> &), (const, override));
    MOCK_METHOD(bool, get, (const QString &, QStringList &), (const, override));
};


bool returnA(const QString &, QVector<double> &data_)
{
    data_ = g_variables["a"];
    return true;
}


TEST(LuaAPI, getVariable)
{
    std::shared_ptr<VariablesGetterMock> variablesGetterMock = std::make_shared<VariablesGetterMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesGetterMock);
    EXPECT_TRUE(luaAPI.load(g_variablesCallTestCode));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_CALL(*variablesGetterMock, get(QString("a"), Matcher<QVector<double> &>(_)))
            .WillOnce(&returnA);
    EXPECT_TRUE(luaAPI.call(g_functionName, result, resultStr));
    ASSERT_THAT(result, Eq(g_variables));
    ASSERT_THAT(resultStr, Eq(g_emptyStrs));
}

class VariablesSetterMock : public IVariablesSetter
{
public:
    bool add(const QString &name_, QVector<double> &&data_) override
    {
        return add_rvref(name_, std::move(data_));
    }

    bool add(const QString &name_, QStringList &&data_) override
    {
        return add_rvrefsl(name_, std::move(data_));
    }

    MOCK_METHOD(bool, add, (const QString &, const QVector<double> &), (override));
    MOCK_METHOD(bool, add, (const QString &, const QStringList &), (override));
    MOCK_METHOD2(add_rvref, bool(const QString &, QVector<double>));
    MOCK_METHOD2(add_rvrefsl, bool(const QString &, QStringList));
};

TEST(LuaAPI, setVariable)
{
    std::shared_ptr<VariablesGetterMock> variablesGetterMock = std::make_shared<VariablesGetterMock>();
    std::shared_ptr<VariablesSetterMock> variablesSetterMock = std::make_shared<VariablesSetterMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesGetterMock);
    luaAPI.set(variablesSetterMock);
    EXPECT_TRUE(luaAPI.load(g_setVariablesCallTestCode));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_CALL(*variablesGetterMock, get(QString("a"), Matcher<QVector<double> &>(_)))
            .WillOnce(&returnA);
    EXPECT_CALL(*variablesSetterMock, add_rvref(QString("b"), g_variables["a"]))
            .WillOnce(Return(true));
    EXPECT_TRUE(luaAPI.call(g_functionName, result, resultStr));
    ASSERT_THAT(result, Eq(g_empty));
    ASSERT_THAT(resultStr, Eq(g_emptyStrs));
}

TEST(LuaAPI, hideLibs)
{
    LuaAPI luaAPI;
    EXPECT_FALSE(luaAPI.load(g_badLibCallTestCode));
    EXPECT_FALSE(luaAPI.load(g_badLibCallTest2Code));
}

bool returnAStr(const QString &, QStringList &data_)
{
    data_ = g_variablesStrs["a"];
    return true;
}

TEST(LuaAPI, strVariables)
{
    std::shared_ptr<VariablesGetterMock> variablesGetterMock = std::make_shared<VariablesGetterMock>();
    std::shared_ptr<VariablesSetterMock> variablesSetterMock = std::make_shared<VariablesSetterMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesGetterMock);
    luaAPI.set(variablesSetterMock);
    EXPECT_TRUE(luaAPI.load(g_setVariablesStrsCallTestCode));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_CALL(*variablesGetterMock, get(QString("a"), Matcher<QVector<double> &>(_)))
            .WillOnce(Return(false));
    EXPECT_CALL(*variablesGetterMock, get(QString("a"), Matcher<QStringList &>(_)))
            .WillOnce(&returnAStr);
    EXPECT_CALL(*variablesSetterMock, add_rvrefsl(QString("b"), g_variablesStrs["a"]))
            .WillOnce(Return(true));
    EXPECT_TRUE(luaAPI.call(g_functionName, result, resultStr));
    ASSERT_THAT(result, Eq(g_empty));
    ASSERT_THAT(resultStr, Eq(g_variablesStrs));
}


#endif // TST_LUAAPI_H
