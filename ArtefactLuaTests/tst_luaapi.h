#ifndef TST_LUAAPI_H
#define TST_LUAAPI_H


#include <QString>
#include <QVector>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "luaapi.h"


using namespace testing;

namespace {
    const char *g_callTestCode =
            "function main (...)\n"
            "   result = {}\n"
            "   result[\"a\"] = {1,2,3}\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_callTestFunctionName = "main";
}


TEST(LuaAPI, callReturn)
{
    LuaAPI luaAPI;
    std::map<QString, QVector<double>> resultValue = {{QString("a"), {1.0, 2.0, 3.0}},};
    std::map<QString, QVector<double>> result;
    EXPECT_TRUE(luaAPI.call(g_callTestCode, g_callTestFunctionName, result));
    //ASSERT_THAT(result, Eq(resultValue));
}


#endif // TST_LUAAPI_H
