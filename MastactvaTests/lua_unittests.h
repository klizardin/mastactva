#ifndef LUA_UNITTESTS_H
#define LUA_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include "test_utils.h"


using namespace testing;


TEST(Lua, base)
{
    ASSERT_TRUE(true);
}


#endif // LUA_UNITTESTS_H
