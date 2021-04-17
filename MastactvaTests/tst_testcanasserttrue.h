#ifndef TST_TESTCANASSERTTRUE_H
#define TST_TESTCANASSERTTRUE_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(GoogleTests, TestCanAssertTrue)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

#endif // TST_TESTCANASSERTTRUE_H
