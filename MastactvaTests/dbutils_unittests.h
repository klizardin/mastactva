#if !defined(DBUTILS_UNITTESTS)
#define DBUTILS_UNITTESTS

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/names.h"


using namespace testing;


TEST(DBUtils, refName)
{
    const QString res = db::refName("some_name");
    ASSERT_STRCASEEQ("ref_some_name", res.toUtf8().constData());
}

TEST(DBUtils, refNames)
{
    const QStringList res = db::refNames(QStringList({"some_name", "another_name"}));
    ASSERT_STRCASEEQ("ref_some_name", res[0].toUtf8().constData());
    ASSERT_STRCASEEQ("ref_another_name", res[1].toUtf8().constData());
}

TEST(DBUtils, toBindName)
{
    const QString simple = db::toBindName("simple");
    ASSERT_STRCASEEQ(":simple", simple.toUtf8().constData());
    const QString quoted = db::toBindName("\"quoted\"");
    ASSERT_STRCASEEQ(":quoted", quoted.toUtf8().constData());
}

#endif
