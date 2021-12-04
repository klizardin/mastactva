#ifndef LUA_UNITTESTS_H
#define LUA_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include "lua_utils.h"
#include "luaapi.h"
#include "test_utils.h"


using namespace testing;


static const char *g_luaScriptBaseDataTestFmt =
        "function main ()\n"
        "    result = {}\n"
        "    result[\"%2\"] = %3\n"
        "    test(\"%1\", result)\n"
        "end\n";

struct DataTestData
{
    int a;
    double b;
    QString c;
};

static const auto g_DataTestDataLayout = makeDataLayout(
            makeFieldLayout("a", &DataTestData::a),
            makeFieldLayout("b", &DataTestData::b),
            makeFieldLayout("c", &DataTestData::c)
            );


TEST(Lua, base)
{
    LuaAPI api;
    //api.addTest()

    ASSERT_TRUE(true);
}


#endif // LUA_UNITTESTS_H
