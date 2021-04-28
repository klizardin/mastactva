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


bool equal(
        const QString &result_,
        const QString &stringsSum_
        )
{
    return result_ == stringsSum_;
}

template<typename Arg_>
QString sum(Arg_ arg_)
{
    return QString(arg_);
}

template<typename Arg_, typename ... Args_>
QString sum(Arg_ arg_, Args_ ... args_)
{
    return QString(arg_) + sum(args_ ...);
}

TEST(DBUtils, refName)
{
    ASSERT_TRUE(
                equal(
                    db::refName("some_name"),
                    sum(g_refPrefix, "some_name")
                    )
                );
    ASSERT_TRUE(
                equal(
                    db::refName("\"some_name\""),
                    sum(g_refPrefix, "some_name")
                    )
                );
}

TEST(DBUtils, refNames)
{
    const QStringList res = db::refNames(QStringList({"some_name", "another_name", "\"quoted\""}));
    ASSERT_TRUE(
                equal(
                    res[0],
                    sum(g_refPrefix, "some_name")
                    )
                );
    ASSERT_TRUE(
                equal(
                    res[1],
                    sum(g_refPrefix, "another_name")
                    )
                );
    ASSERT_TRUE(
                equal(
                    res[2],
                    sum(g_refPrefix, "quoted")
                )
            );
}

TEST(DBUtils, toBindName)
{
    ASSERT_TRUE(
                equal(
                    db::toBindName("simple"),
                    sum(g_bindPrefix, "simple")
                    )
                );
    ASSERT_TRUE(
                equal(
                    db::toBindName("\"quoted\""),
                    sum(g_bindPrefix, "quoted")
                    )
                );
}

TEST(DBUtils, equalToValueConditionListFromSqlNameList)
{
    const QStringList res = db::equalToValueConditionListFromSqlNameList(
                QStringList({"some_name", "\"another_name\""})
                );
    ASSERT_TRUE(
                equal(
                    res[0],
                    sum(g_refPrefix, "some_name", "=", g_bindPrefix, g_refPrefix, "some_name")
                )
            );
    ASSERT_TRUE(
                equal(
                    res[1],
                    sum(g_refPrefix, "another_name", "=", g_bindPrefix, g_refPrefix, "another_name")
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
    ASSERT_TRUE(
                equal(
                    db::quotName(sum(g_refPrefix, "name")),
                    sum(g_refPrefix, "name")
                    )
                );
    ASSERT_TRUE(
                equal(
                    db::quotName(sum(g_bindPrefix, "name")),
                    sum(g_bindPrefix, "name")
                    )
                );
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
    ASSERT_TRUE(equal(
                    db::tableName("table_name", ""),
                    sum("table_name")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName("table_name", "reference_name"),
                    sum("table_name", g_splitTableRef, "reference_name")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName("table-name", ""),
                    sum("table_name")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName("table-name", "reference-name"),
                    sum("table_name", g_splitTableRef, "reference_name")
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
    ASSERT_TRUE(equal(
                    sqlNames[0],
                    sum(g_bindPrefix, "sql_name")
                    )
                );
    ASSERT_TRUE(equal(
                    sqlNames[1],
                    sum(g_bindPrefix, "user")
                    )
                );
}

TEST(DBUtils, JsonSqlFieldsList_getJsonObject)
{
    db::JsonSqlFieldsList fields = {
        {"json-name-1", "sql_name", layout::JsonTypesEn::jt_int, false},
        {"json-name-2", "sql_name", layout::JsonTypesEn::jt_int, false},

        {"json-name-3", "user", layout::JsonTypesEn::jt_double, false},
        {"json-name-4", "user", layout::JsonTypesEn::jt_double, false},
        {"json-name-5", "user", layout::JsonTypesEn::jt_double, false},

        {"json-name-6", "user", layout::JsonTypesEn::jt_bool, false},
        {"json-name-7", "user", layout::JsonTypesEn::jt_bool, false},
        {"json-name-8", "user", layout::JsonTypesEn::jt_bool, false},
        {"json-name-9", "user", layout::JsonTypesEn::jt_bool, false},
    };
    QHash<QString, QVariant> values = {
        {"json-name-1", QVariant::fromValue(QString("1.0"))},
        {"json-name-2", QVariant::fromValue(QString("1"))},

        {"json-name-3", QVariant::fromValue(2)},
        {"json-name-4", QVariant::fromValue(2.5)},
        {"json-name-5", QVariant::fromValue(QString("1.5"))},

        {"json-name-6", QVariant::fromValue(QString("1"))},
        {"json-name-7", QVariant::fromValue(0.5)},
        {"json-name-8", QVariant::fromValue(QString("true"))},
        {"json-name-9", QVariant::fromValue(QString("false"))},
    };
    QJsonObject res0 = QJsonObject::fromVariantHash(
    {
        {"json-name-1", QVariant::fromValue(1)},
        {"json-name-2", QVariant::fromValue(1)},

        {"json-name-3", QVariant::fromValue(2)},
        {"json-name-4", QVariant::fromValue(2.5)},
        {"json-name-5", QVariant::fromValue(1.5)},

        {"json-name-6", QVariant::fromValue(true)},
        {"json-name-7", QVariant::fromValue(true)},
        {"json-name-8", QVariant::fromValue(true)},
        {"json-name-9", QVariant::fromValue(false)},
    });

    QJsonObject res = db::getJsonObject(values, fields);
    ASSERT_TRUE(res == res0);
}

#endif
