#ifndef UTILS_SCALEDTIME_UNITTESTS_H
#define UTILS_SCALEDTIME_UNITTESTS_H


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

class ScaledTimeEventsMock: public IScaledTimeEvents
{
public:
    MOCK_METHOD(void, onScaledTimeCycle, (int cycle), (override));
    MOCK_METHOD(void, onScaledTimeLastCycle, (), (override));
    MOCK_METHOD(void, onScaledTimeFirstCycle, (), (override));
};


TEST(Utils, ScaledTime_Cycles_mock)
{
    ScaledTimeEventsMock events_mock;
    ScaledTime scalledTime;
    scalledTime.set(&events_mock);
    QVariantList value;

    value << QVariant::fromValue(0.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(1.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(0.0);

    scalledTime.setVectorValue(value);
    const int cycles = rand() % 10 + 2;
    EXPECT_CALL(events_mock, onScaledTimeFirstCycle()).Times(testing::AtLeast(1));
    scalledTime.setCycles(cycles);
    qreal dt = 0.0;
    for(int i = 0; i < cycles; ++i)
    {
        (void)scalledTime.get(1000.0, dt);
        EXPECT_CALL(events_mock, onScaledTimeCycle(i)).Times(testing::AtLeast(1));
        if(cycles - 1 == i)
        {
            EXPECT_CALL(events_mock, onScaledTimeLastCycle()).Times(testing::AtLeast(1));
            EXPECT_CALL(events_mock, onScaledTimeFirstCycle()).Times(testing::AtLeast(1));
        }
        (void)scalledTime.get(1000.0, dt);
    }
    (void)scalledTime.get(1000.0, dt);
    (void)scalledTime.get(1000.0, dt);
}

TEST(Utils, ScaledTime_Cycles_mock2)
{
    ScaledTimeEventsMock events_mock;
    ScaledTime scalledTime;
    scalledTime.set(&events_mock);
    QVariantList value;

    value << QVariant::fromValue(0.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(1.0)
        << QVariant::fromValue(1000.0)
        << QVariant::fromValue(0.0);

    scalledTime.setVectorValue(value);
    const int cycles = rand() % 10 + 2;
    scalledTime.setCycles(-1);
    qreal dt = 0.0;
    for(int i = 0; i < cycles; ++i)
    {
        (void)scalledTime.get(1000.0, dt);
        EXPECT_CALL(events_mock, onScaledTimeCycle(-1)).Times(testing::AtLeast(1));
        (void)scalledTime.get(1000.0, dt);
        EXPECT_EQ(-1, scalledTime.getCycles());
    }
    (void)scalledTime.get(1000.0, dt);
    EXPECT_CALL(events_mock, onScaledTimeCycle(-1)).Times(testing::AtLeast(1));
    (void)scalledTime.get(1000.0, dt);
    EXPECT_EQ(-1, scalledTime.getCycles());
}

TEST(Utils,ScaledTime_pause)
{
    ScaledTime scalledTime;
    QVariantList value;
    value << QVariant::fromValue(0.0)
        << QVariant::fromValue(10.0)
        << QVariant::fromValue(10.0);

    scalledTime.setVectorValue(value);
    EXPECT_EQ(value, scalledTime.getVectorValue());

    qreal dt = 0.0;
    ASSERT_DOUBLE_EQ(scalledTime.get(1.0, dt, false), 1.0);
    ASSERT_DOUBLE_EQ(dt, 0.0);

    const qreal pause1 = 2.0;
    const int cycles1 = 9;
    for(int i = 0; i < cycles1; ++i)
    {
        ASSERT_DOUBLE_EQ(scalledTime.get(1.0 + pause1*(double)(i+1)/(double)cycles1, dt, true), 1.0);
        ASSERT_DOUBLE_EQ(dt, 0.0);
    }
    EXPECT_NEAR(scalledTime.get(1.0+pause1+1.0, dt, false), 1.0+1.0, 1e-7);
    ASSERT_DOUBLE_EQ(dt, 0.0);
}

#endif // UTILS_SCALEDTIME_UNITTESTS_H
