#if !defined(DBUTILS_UNITTESTS)
#define DBUTILS_UNITTESTS

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/names.h"


using namespace testing;


TEST(DBUtils, refName)
{
    ASSERT_STRCASEEQ("ref_some_name", db::refName("some_name").toUtf8().constData());
    ASSERT_STRCASEEQ("ref_some_name", db::refName("\"some_name\"").toUtf8().constData());
}

TEST(DBUtils, refNames)
{
    const QStringList res = db::refNames(QStringList({"some_name", "another_name", "\"quoted\""}));
    ASSERT_STRCASEEQ("ref_some_name", res[0].toUtf8().constData());
    ASSERT_STRCASEEQ("ref_another_name", res[1].toUtf8().constData());
    ASSERT_STRCASEEQ("ref_quoted", res[2].toUtf8().constData());
}

TEST(DBUtils, toBindName)
{
    ASSERT_STRCASEEQ(":simple", db::toBindName("simple").toUtf8().constData());
    ASSERT_STRCASEEQ(":quoted", db::toBindName("\"quoted\"").toUtf8().constData());
}

TEST(DBUtils, equalToValueConditionListFromSqlNameList)
{
    const QStringList res = db::equalToValueConditionListFromSqlNameList(QStringList({"some_name", "\"another_name\""}));
    ASSERT_STRCASEEQ("ref_some_name=:ref_some_name", res[0].toUtf8().constData());
    ASSERT_STRCASEEQ("ref_another_name=:ref_another_name", res[1].toUtf8().constData());
}

TEST(DBUtils, filterNames)
{
    QVariantList filterEmptyList;
    QStringList names({"some_name", "\"another_name\"", "1"});
    const QStringList res1 = db::filterNames(
                names,
                filterEmptyList
                );
    ASSERT_EQ(res1.size(), 3);
    QVariantList filterList1;
    filterList1.push_back(QVariant::fromValue(1));
    const QStringList res2 = db::filterNames(
                names,
                filterList1
                );
    ASSERT_EQ(res2.size(), 1);
    QVariantList filterList2;
    filterList2.push_back(QVariant::fromValue(1));
    filterList2.push_back(QVariant::fromValue(QString("some_name")));
    const QStringList res3 = db::filterNames(
                names,
                filterList2
                );
    ASSERT_EQ(res3.size(), 2);
}

#endif
