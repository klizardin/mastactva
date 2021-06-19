#ifndef TST_INSTALLED_H
#define TST_INSTALLED_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>


using namespace testing;


TEST(GTest, installed)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

#endif // TST_INSTALLED_H
