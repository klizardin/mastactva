#ifndef FORMAT_UNITTESTS_H
#define FORMAT_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QList>
#include <QVector>
#include "../MastactvaBase/format.h"
#include "test_utils.h"


using namespace testing;


TEST(Format, toString_simpleTypes)
{
    ASSERT_TRUE(equal(fmt::toString(0), sum("0")));
    ASSERT_TRUE(equal(fmt::toString(1), sum("1")));
    ASSERT_TRUE(equal(fmt::toString(QString("2")), sum("2")));
    ASSERT_TRUE(equal(fmt::toString(2.5), sum("2.5")));
}

TEST(Format, toString_stringConverteble)
{
    ASSERT_TRUE(equal(fmt::toString("2"), sum("2")));
}

class Name : protected QString
{
public:
    using QString::QString;

    QString toString() const
    {
        return *this;
    }
};

TEST(Format, toString_compound)
{
    ASSERT_TRUE(equal(fmt::toString(Name("str")), sum("str")));
}

#endif // FORMAT_UNITTESTS_H
