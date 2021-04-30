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

class NameQString : public QString
{
public:
    using QString::QString;
};

TEST(Format, toString_compoundWithQStringConversion)
{
    ASSERT_TRUE(equal(fmt::toString(NameQString("str")), sum("str")));
}

TEST(Format, classFormat)
{
    ASSERT_TRUE(equal(fmt::toString(fmt::Private::Format<>("str")), sum("str")));
    ASSERT_TRUE(equal(fmt::toString(fmt::format("str1")), sum("str1")));
}

TEST(Format, formatArguments)
{
    ASSERT_TRUE(equal(fmt::toString(fmt::format("%1=%2", 1, "1")), sum("1", "=" "1")));
    ASSERT_TRUE(equal(fmt::toString(fmt::format("%1=%2", Name("a"), NameQString("2"))), sum("a", "=" "2")));
}

TEST(Format, classList)
{
    ASSERT_TRUE(equal(fmt::toString(fmt::Private::List<int>({1,2,3}, ",")), sum("1", "," "2", ",", "3")));
    ASSERT_TRUE(equal(fmt::toString(fmt::list(QVector<int>({1,2,3}), " , ")), sum("1", " , " "2", " , ", "3")));
}

namespace fmt
{

int toType(const Name &val_, int *)
{
    return QVariant::fromValue(val_.toString()).toInt();
}

}

TEST(Format, toType)
{
    ASSERT_EQ(fmt::toType(Name("1"), static_cast<int *>(nullptr)), 1);
}

#endif // FORMAT_UNITTESTS_H
