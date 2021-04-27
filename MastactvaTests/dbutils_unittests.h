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

static int leftInListAfterFilter(const QStringList &names_, const QVariantList &filterList_)
{
    const QStringList res = db::filterNames(
                names_,
                filterList_
                );
    return res.size();
}

TEST(DBUtils, filterNames)
{
    QStringList names({"some_name", "\"another_name\"", "1"});

    QVariantList filterEmptyList;
    ASSERT_EQ(3, leftInListAfterFilter(names, filterEmptyList));

    QVariantList filterList1;
    filterList1.push_back(QVariant::fromValue(1));
    ASSERT_EQ(1, leftInListAfterFilter(names, filterList1));

    QVariantList filterList2;
    filterList2.push_back(QVariant::fromValue(1));
    filterList2.push_back(QVariant::fromValue(QString("some_name")));
    ASSERT_EQ(2, leftInListAfterFilter(names, filterList2));
}

TEST(DBUtils, applyFunction)
{
    const QStringList res = db::applyFunction(
                QStringList({"some_name", "another_name", "\"quoted\""}),
                QString("SUM")
                );
    ASSERT_STRCASEEQ("SUM(some_name)", res[0].toUtf8().constData());
    ASSERT_STRCASEEQ("SUM(another_name)", res[1].toUtf8().constData());
    ASSERT_STRCASEEQ("SUM(\"quoted\")", res[2].toUtf8().constData());
}

TEST(DBUtils, isQuotedName)
{
    ASSERT_FALSE(db::isQuotedName("name"));
    ASSERT_TRUE(db::isQuotedName("\"name\""));
}

TEST(DBUtils, isRefName)
{
    ASSERT_FALSE(db::isRefName("name"));
    ASSERT_TRUE(db::isRefName("ref_name"));
}

TEST(DBUtils, isBindName)
{
    ASSERT_FALSE(db::isBindName("name"));
    ASSERT_TRUE(db::isBindName(":name"));
}

TEST(DBUtils, quotName)
{
    ASSERT_STRCASEEQ("\"name\"", db::quotName("name").toUtf8().constData());
    ASSERT_STRCASEEQ("\"name\"", db::quotName("\"name\"").toUtf8().constData());
    ASSERT_STRCASEEQ("ref_name", db::quotName("ref_name").toUtf8().constData());
    ASSERT_STRCASEEQ(":name", db::quotName(":name").toUtf8().constData());
}

TEST(DBUtils, unquotName)
{
    ASSERT_STRCASEEQ("name", db::unquotName("\"name\"").toUtf8().constData());
    ASSERT_STRCASEEQ("name", db::unquotName("name").toUtf8().constData());
}



#endif
