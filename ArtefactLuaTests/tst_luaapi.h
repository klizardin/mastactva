#ifndef TST_LUAAPI_H
#define TST_LUAAPI_H


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
            "function main (...)\n"
            "   result = {}\n"
            "   result[\"a\"] = {1,2,3}\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_callTestFunctionName = "main";
    const char *g_variablesCallTestCode =
            "function main (...)\n"
            "   result = {}\n"
            "   result[\"a\"] = getVariable(\"a\")\n"
            "   return result\n"
            "end\n"
            ;

    std::map<QString, QVector<double>> g_variables =
    {
        {
            QString("a"),
            {1.0, 2.0, 3.0}
        },
    };
}


TEST(LuaAPI, callReturn)
{
    LuaAPI luaAPI;
    std::map<QString, QVector<double>> result;
    EXPECT_TRUE(luaAPI.call(g_simpleCallTestCode, g_callTestFunctionName, result));
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
    std::map<QString, QVector<double>> result;
    EXPECT_CALL(*variablesGetterMock, get(QString("a"), _))
            .WillOnce(&returnA);
    EXPECT_TRUE(luaAPI.call(g_variablesCallTestCode, g_callTestFunctionName, result));
    ASSERT_THAT(result, Eq(g_variables));
}

#endif // TST_LUAAPI_H
