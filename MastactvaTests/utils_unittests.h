#ifndef UTILS_UNITTESTS_H
#define UTILS_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include "../MastactvaBase/utils.h"


TEST(Utils,ScalledTime)
{
    ScalledTime scalledTime;
    QVariantList value;
    value << QVariant::fromValue(0.0);
    value << QVariant::fromValue(1000.0);
    value << QVariant::fromValue(1.0);
    value << QVariant::fromValue(1000.0);
    value << QVariant::fromValue(0.0);
    value << QVariant::fromValue(1000.0);
    value << QVariant::fromValue(1.0);
    value << QVariant::fromValue(1000.0);
    value << QVariant::fromValue(0.0);

    scalledTime.setVectorValue(value);
    EXPECT_EQ(value, scalledTime.getVectorValue());

    qreal dt = 0.0;
    for(int i = 0; i < 10; i++)
    {
        ASSERT_DOUBLE_EQ(dt, 0.0);
        ASSERT_DOUBLE_EQ(scalledTime.get(100.0*(i+1), dt), 0.1*(i+1));
    }
    ASSERT_DOUBLE_EQ(dt, 1000.0);

    // to test
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 0.0);
    ASSERT_DOUBLE_EQ(dt, 1000.0);
    for(int i = 0; i < 10; i++)
    {
        ASSERT_DOUBLE_EQ(dt, 0.0);
        ASSERT_DOUBLE_EQ(scalledTime.get(100.0*(i+1), dt), 0.1*(i+1));
    }
    for(int i = 0; i < 10; i++)
    {
        ASSERT_DOUBLE_EQ(scalledTime.get(100.0, dt), 1.0 - 0.1*(i+1));
        ASSERT_DOUBLE_EQ(dt, 0.1);
    }
}

#endif // UTILS_UNITTESTS_H
