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

    std::map<QString, QVector<double>> g_variables =
    {
        {
            QString("a"),
            {1.0, 2.0, 3.0}
        },
    };
    std::map<QString, QVector<double>> g_empty;
}


TEST(LuaAPI, callReturn)
{
    LuaAPI luaAPI;
    std::map<QString, QVector<double>> result;
    EXPECT_TRUE(luaAPI.load(g_simpleCallTestCode));
    EXPECT_TRUE(luaAPI.call(g_functionName, result));
    ASSERT_THAT(result, Eq(g_variables));
}


class VariablesGetterMock : public IVariablesGetter
{
public:
    MOCK_METHOD(bool, get, (const QString &, QVector<double> &), (const, override));
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
    EXPECT_CALL(*variablesGetterMock, get(QString("a"), _))
            .WillOnce(&returnA);
    EXPECT_TRUE(luaAPI.call(g_functionName, result));
    ASSERT_THAT(result, Eq(g_variables));
}

class VariablesSetterMock : public IVariablesSetter
{
public:
    MOCK_METHOD(bool, add, (const QString &, const QVector<double> &), (override));
    MOCK_METHOD(bool, add, (const QString &, QVector<double> &&), (override));
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
    EXPECT_CALL(*variablesGetterMock, get(QString("a"), _))
            .WillOnce(&returnA);
    EXPECT_CALL(*variablesSetterMock, add(QString("b"), g_variables["a"]))
            .WillOnce(Return(true));
    EXPECT_TRUE(luaAPI.call(g_functionName, result));
    ASSERT_THAT(result, Eq(g_empty));
}

#endif // TST_LUAAPI_H
