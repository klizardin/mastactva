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
    const QString res = refName("some_name");
    ASSERT_STRCASEEQ("ref_some_name", res.toUtf8().constData());
}

#endif
