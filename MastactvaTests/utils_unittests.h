#ifndef UTILS_UNITTESTS_H
#define UTILS_UNITTESTS_H


#include <stdlib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/utils_scaledtime.h"


TEST(Utils,ScaledTime_smoke)
{
    ScaledTime scalledTime;
    QVariantList value;
    value << QVariant::fromValue(0.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(1.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(0.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(1.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(0.0);

    scalledTime.setVectorValue(value);
    EXPECT_EQ(value, scalledTime.getVectorValue());

    qreal dt = 0.0;
    for(int i = 0; i < 10; i++)
    {
        ASSERT_DOUBLE_EQ(dt, 0.0);
        ASSERT_DOUBLE_EQ(scalledTime.get(100.0*(i+1), dt), 0.1*(i+1));
    }
    ASSERT_DOUBLE_EQ(dt, 1000.0);

    // to debug, new tests
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 0.0);
    ASSERT_DOUBLE_EQ(dt, 2000.0);
    for(int i = 0; i < 10; i++)
    {
        ASSERT_DOUBLE_EQ(dt, 2000.0);
        ASSERT_DOUBLE_EQ(scalledTime.get(100.0*(i+1), dt), 0.1*(i+1));
    }
    for(int i = 0; i < 10; i++)
    {
        ASSERT_DOUBLE_EQ(dt, 3000.0);
        ASSERT_DOUBLE_EQ(scalledTime.get(100.0*(i+1), dt), 1.0 - 0.1*(i+1));
    }
    ASSERT_DOUBLE_EQ(dt, 4000.0);
}

TEST(Utils,ScaledTime_border)
{
    ScaledTime scalledTime;
    QVariantList value;

    value << QVariant::fromValue(0.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(1.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(0.0);

    scalledTime.setVectorValue(value);
    EXPECT_EQ(value, scalledTime.getVectorValue());

    qreal dt = 0.0;
    ASSERT_DOUBLE_EQ(dt, 0.0);
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 1.0);
    ASSERT_DOUBLE_EQ(dt, 1000.0);
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 0.0);
    ASSERT_DOUBLE_EQ(dt, 2000.0);
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 0.0);
    ASSERT_DOUBLE_EQ(dt, 3000.0);
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 0.0);
    ASSERT_DOUBLE_EQ(dt, 4000.0);
}

TEST(Utils, ScaledTime_Cycles_default)
{
    ScaledTime scalledTime;
    QVariantList value;

    value << QVariant::fromValue(0.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(1.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(0.0);

    scalledTime.setVectorValue(value);
    const int cycles = rand() % 10 + 2;
    scalledTime.setCycles(cycles);
    qreal dt = 0.0;
    for(int i = 0; i < cycles; ++i)
    {
        ASSERT_DOUBLE_EQ(dt, 0.0 + 2000.0*i);
        ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 1.0);
        ASSERT_DOUBLE_EQ(dt, 1000.0 + 2000.0*i);
        ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 0.0);
        ASSERT_DOUBLE_EQ(dt, 2000.0 + 2000.0*i);
    }
    ASSERT_DOUBLE_EQ(dt, 0.0 + 2000.0*cycles);
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 1.0);
    ASSERT_DOUBLE_EQ(dt, 1000.0 + 2000.0*cycles);
    ASSERT_DOUBLE_EQ(scalledTime.get(1000.0, dt), 0.0);
    ASSERT_DOUBLE_EQ(dt, 2000.0 + 2000.0*cycles);
}


#endif // UTILS_UNITTESTS_H
