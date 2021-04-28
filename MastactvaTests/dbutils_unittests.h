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


bool comparePrefixedStringAndResult(
        const char *prefix_,
        const char *str_,
        const QString &result_
        )
{
    return QString(prefix_) + QString(str_) == result_;
}

template<typename Arg_>
QString sumStrings(Arg_ arg_)
{
    return QString(arg_);
}

template<typename Arg_, typename ... Args_>
QString sumStrings(Arg_ arg_, Args_ ... args_)
{
    return QString(arg_) + sumStrings(args_ ...);
}

template<typename ... Args_>
bool compareResultAndStringsSum(const QString &result_, Args_ ... args_)
{
    return result_ == sumStrings(args_ ...);
}

TEST(DBUtils, refName)
{
    ASSERT_TRUE(comparePrefixedStringAndResult(g_refPrefix, "some_name", db::refName("some_name")));
    ASSERT_TRUE(comparePrefixedStringAndResult(g_refPrefix, "some_name", db::refName("\"some_name\"")));
}

TEST(DBUtils, refNames)
{
    const QStringList res = db::refNames(QStringList({"some_name", "another_name", "\"quoted\""}));
    ASSERT_TRUE(comparePrefixedStringAndResult(g_refPrefix, "some_name", res[0]));
    ASSERT_TRUE(comparePrefixedStringAndResult(g_refPrefix, "another_name", res[1]));
    ASSERT_TRUE(comparePrefixedStringAndResult(g_refPrefix, "quoted", res[2]));
}

TEST(DBUtils, toBindName)
{
    ASSERT_TRUE(comparePrefixedStringAndResult(g_bindPrefix, "simple", db::toBindName("simple")));
    ASSERT_TRUE(comparePrefixedStringAndResult(g_bindPrefix, "quoted", db::toBindName("\"quoted\"")));
}

TEST(DBUtils, equalToValueConditionListFromSqlNameList)
{
    const QStringList res = db::equalToValueConditionListFromSqlNameList(
                QStringList({"some_name", "\"another_name\""})
                );
    ASSERT_TRUE(
                compareResultAndStringsSum(
                    res[0],
                    g_refPrefix, "some_name", "=", g_bindPrefix, g_refPrefix, "some_name"
                )
            );
    ASSERT_TRUE(
                compareResultAndStringsSum(
                    res[1],
                    g_refPrefix, "another_name", "=", g_bindPrefix, g_refPrefix, "another_name"
                )
            );
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
    ASSERT_TRUE(db::isRefName(QString(g_refPrefix) + "name"));
}

TEST(DBUtils, isBindName)
{
    ASSERT_FALSE(db::isBindName("name"));
    ASSERT_TRUE(db::isBindName(QString(g_bindPrefix) + "name"));
}

TEST(DBUtils, quotName)
{
    ASSERT_STRCASEEQ("\"name\"", db::quotName("name").toUtf8().constData());
    ASSERT_STRCASEEQ("\"name\"", db::quotName("\"name\"").toUtf8().constData());
    ASSERT_TRUE(comparePrefixedStringAndResult(g_refPrefix, "name", db::quotName(QString(g_refPrefix) + "name")));
    ASSERT_TRUE(comparePrefixedStringAndResult(g_bindPrefix, "name", db::quotName(QString(g_bindPrefix) + "name")));
}

TEST(DBUtils, unquotName)
{
    ASSERT_STRCASEEQ("name", db::unquotName("\"name\"").toUtf8().constData());
    ASSERT_STRCASEEQ("name", db::unquotName("name").toUtf8().constData());
}

TEST(DBUtils, jsonToSqlNaming)
{
    ASSERT_STRCASEEQ("name", db::jsonToSql("name").toUtf8().constData());
    ASSERT_STRCASEEQ("name_1", db::jsonToSql("name-1").toUtf8().constData());
}

TEST(DBUtils, tableName)
{
    ASSERT_TRUE(compareResultAndStringsSum(
                    db::tableName("table_name", ""),
                    "table_name"
                    )
                );
    ASSERT_TRUE(compareResultAndStringsSum(
                    db::tableName("table_name", "reference_name"),
                    "table_name", g_splitTableRef, "reference_name"
                    )
                );
    ASSERT_TRUE(compareResultAndStringsSum(
                    db::tableName("table-name", ""),
                    "table_name"
                    )
                );
    ASSERT_TRUE(compareResultAndStringsSum(
                    db::tableName("table-name", "reference-name"),
                    "table_name", g_splitTableRef, "reference_name"
                    )
                );
}

TEST(DBUtils, JsonSqlFieldsList_getSqlNames)
{
    db::JsonSqlFieldsList fields = {
        {"json-name-1", "sql_name", layout::JsonTypesEn::jt_int, false},
        {"json-name-2", "user", layout::JsonTypesEn::jt_int, false},
    };
    const QStringList sqlNames = db::getSqlNames(fields);
    ASSERT_STRCASEEQ("sql_name", sqlNames[0].toUtf8().constData());
    ASSERT_STRCASEEQ("\"user\"", sqlNames[1].toUtf8().constData());
}

TEST(DBUtils, JsonSqlFieldsList_getBindSqlNames)
{
    db::JsonSqlFieldsList fields = {
        {"json-name-1", "sql_name", layout::JsonTypesEn::jt_int, false},
        {"json-name-2", "user", layout::JsonTypesEn::jt_int, false},
    };
    const QStringList sqlNames = db::getBindSqlNames(fields);
    ASSERT_TRUE(compareResultAndStringsSum(
                    sqlNames[0],
                    g_bindPrefix, "sql_name"
                    )
                );
    ASSERT_TRUE(compareResultAndStringsSum(
                    sqlNames[1],
                    g_bindPrefix, "user"
                    )
                );
}


#endif
