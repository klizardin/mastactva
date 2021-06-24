#ifndef TST_INSTALLED_H
#define TST_INSTALLED_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "luaapi.h"


using namespace testing;


TEST(GTest, installed)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}


const char * g_simpleLuaScript = "width = 320";
const char * g_simpleErrorLuaScript = "b*2"; // undefined variable error


TEST(Lua, installed)
{
    LuaAPI api;
    EXPECT_TRUE(api.load(g_simpleLuaScript));
    EXPECT_FALSE(api.load(g_simpleErrorLuaScript));
}


#endif // TST_INSTALLED_H
