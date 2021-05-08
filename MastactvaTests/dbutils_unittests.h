#if !defined(DBUTILS_UNITTESTS)
#define DBUTILS_UNITTESTS

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QHash>
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/names.h"
#include "test_utils.h"


using namespace testing;


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
                    db::tableName(JsonName("table_name"), JsonName("")),
                    sum("table_name")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName(JsonName("table_name"), JsonName("reference_name")),
                    sum("table_name", g_splitTableRef, "reference_name")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName(JsonName("table-name"), JsonName("")),
                    sum("table_name")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName(JsonName("table-name"), JsonName("reference-name")),
                    sum("table_name", g_splitTableRef, "reference_name")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName(JsonName("user"), JsonName("")),
                    sum("\"user\"")
                    )
                );
    ASSERT_TRUE(equal(
                    db::tableName(JsonName("user"), JsonName("reference-name")),
                    sum("user", g_splitTableRef, "reference_name")
                    )
                );
}

TEST(DBUtils, JsonSqlFieldsList_getSqlNames)
{
    db::JsonSqlFieldsList fields = {
        { "sql-name", layout::JsonTypesEn::jt_int, false},
        { "user", layout::JsonTypesEn::jt_int, false},
    };
    const QStringList sqlNames = db::getSqlNames(fields);
    ASSERT_STRCASEEQ("sql_name", sqlNames[0].toUtf8().constData());
    ASSERT_STRCASEEQ("\"user\"", sqlNames[1].toUtf8().constData());
}

TEST(DBUtils, JsonSqlFieldsList_getBindSqlNames)
{
    db::JsonSqlFieldsList fields = {
        { "sql-name", layout::JsonTypesEn::jt_int, false},
        { "user", layout::JsonTypesEn::jt_int, false},
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
        {"json-name-int-1", layout::JsonTypesEn::jt_int, false},
        {"json-name-int-2", layout::JsonTypesEn::jt_int, false},

        {"json-name-double-3", layout::JsonTypesEn::jt_double, false},
        {"json-name-double-4", layout::JsonTypesEn::jt_double, false},
        {"json-name-double-5", layout::JsonTypesEn::jt_double, false},

        {"json-name-bool-6", layout::JsonTypesEn::jt_bool, false},
        {"json-name-bool-7", layout::JsonTypesEn::jt_bool, false},
        {"json-name-bool-8", layout::JsonTypesEn::jt_bool, false},
        {"json-name-bool-9", layout::JsonTypesEn::jt_bool, false},

        {"json-none-name-10", layout::JsonTypesEn::jt_string, false},
    };
    QHash<QString, QVariant> values = {
        {"json-name-int-1", QVariant::fromValue(QString("1.0"))},
        {"json-name-int-2", QVariant::fromValue(QString("1"))},

        {"json-name-double-3", QVariant::fromValue(2)},
        {"json-name-double-4", QVariant::fromValue(2.5)},
        {"json-name-double-5", QVariant::fromValue(QString("1.5"))},

        {"json-name-bool-6", QVariant::fromValue(QString("1"))},
        {"json-name-bool-7", QVariant::fromValue(0.5)},
        {"json-name-bool-8", QVariant::fromValue(QString("true"))},
        {"json-name-bool-9", QVariant::fromValue(QString("false"))},

        {"json-none-name-11", QVariant::fromValue(QString("some string"))},
    };
    QJsonObject res0 = QJsonObject::fromVariantHash(
    {
        {"json-name-int-1", QVariant::fromValue(1)},
        {"json-name-int-2", QVariant::fromValue(1)},

        {"json-name-double-3", QVariant::fromValue(2)},
        {"json-name-double-4", QVariant::fromValue(2.5)},
        {"json-name-double-5", QVariant::fromValue(1.5)},

        {"json-name-bool-6", QVariant::fromValue(true)},
        {"json-name-bool-7", QVariant::fromValue(true)},
        {"json-name-bool-8", QVariant::fromValue(true)},
        {"json-name-bool-9", QVariant::fromValue(false)},
    });

    QJsonObject res = db::getJsonObject(values, fields);
    ASSERT_TRUE(res == res0);
}

TEST(DBUtils, JsonSqlFieldsList_getSqlNameEqualBindSqlNameList)
{
    db::JsonSqlFieldsList fields = {
        { "sql-name", layout::JsonTypesEn::jt_int, false},
        { "user", layout::JsonTypesEn::jt_int, false},
    };
    const QStringList sqlNames = db::getSqlNameEqualBindSqlNameList(fields);
    ASSERT_TRUE(equal(
                    sqlNames[0],
                    sum("sql_name", "=", g_bindPrefix, "sql_name")
                    )
                );
    ASSERT_TRUE(equal(
                    sqlNames[1],
                    sum("\"user\"", "=", g_bindPrefix, "user")
                    )
                );
}

TEST(DBUtils, JsonSqlFieldsList_getSqlNameAndTypeList)
{
    db::JsonSqlFieldsList fields = {
        { "bool-name", layout::JsonTypesEn::jt_bool, false},
        { "int-name", layout::JsonTypesEn::jt_int, false},
        { "double-name", layout::JsonTypesEn::jt_double, false},
        { "string-name", layout::JsonTypesEn::jt_string, false},
        { "datetime-name", layout::JsonTypesEn::jt_datetime, false},
    };
    const QStringList res = db::getSqlNameAndTypeList(fields);
    ASSERT_TRUE(equal(
                    res[0],
                    sum("bool_name", g_spaceName, g_sqlInt)
                    )
                );
    ASSERT_TRUE(equal(
                    res[1],
                    sum("int_name", g_spaceName, g_sqlInt)
                    )
                );
    ASSERT_TRUE(equal(
                    res[2],
                    sum("double_name", g_spaceName, g_sqlDouble)
                    )
                );
    ASSERT_TRUE(equal(
                    res[3],
                    sum("string_name", g_spaceName, g_sqlText)
                    )
                );
    ASSERT_TRUE(equal(
                    res[4],
                    sum("datetime_name", g_spaceName, g_sqlText)
                    )
                );
}

TEST(DBUtils, JsonSqlFieldsList_getCreateTableSqlRequest)
{
    db::JsonSqlFieldsList fields = {
        { "id", layout::JsonTypesEn::jt_int, true},
        { "user", layout::JsonTypesEn::jt_int, false},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const QString jsonLayoutName{"user-list"};
    const QString jsonRefName{"user-id"};
    const QStringList refs({"user", "name"});
    const QStringList extraRefs({"age-years",});

    const QString res0 = sum(
                "CREATE TABLE IF NOT EXISTS ",
                "user_list", g_splitTableRef, "user_id",
                " ( ", "id", g_spaceName, g_sqlInt, g_insertFieldSpliter,
                      "\"user\"", g_spaceName, g_sqlInt, g_insertFieldSpliter,
                      "name", g_spaceName, g_sqlText, g_insertFieldSpliter,
                      g_refPrefix, "user", g_spaceName, g_sqlText, g_insertFieldSpliter,
                      g_refPrefix, "name", g_spaceName, g_sqlText, g_insertFieldSpliter,
                      g_refPrefix, "age_years", g_spaceName, g_sqlText, " );"
                );
    ASSERT_TRUE(
                equal(db::getCreateTableSqlRequest(
                          jsonLayoutName,
                          jsonRefName,
                          fields,
                          refs,
                          extraRefs
                          ),
                        res0
                    )
                );
}

TEST(DBUtils, findIdFields)
{
    const db::JsonSqlFieldsList fields1 = {
        { "id", layout::JsonTypesEn::jt_int, true},
        { "user", layout::JsonTypesEn::jt_int, false},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const auto idField1 = db::findIdField(fields1);
    ASSERT_TRUE(db::idFieldExist(idField1, fields1));
    ASSERT_EQ(idField1->getJsonName(),"id");

    const db::JsonSqlFieldsList fields2 = {
        { "id", layout::JsonTypesEn::jt_int, false},
    };
    const auto idField2 = db::findIdField(fields2);
    ASSERT_FALSE(db::idFieldExist(idField2, fields2));
}

TEST(DBUtils, JsonSqlFieldsList_getFindSqlRequest)
{
    db::JsonSqlFieldsList fields = {
        { "user", layout::JsonTypesEn::jt_int, true},
        { "user-id", layout::JsonTypesEn::jt_int, false},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const QString jsonLayoutName{"user-list"};
    const QString jsonRefName{"user-id"};
    const QStringList refs({"user-id", "name"});
    const QStringList extraRefs({"age-years",});

    const QString res0 = sum(
                "SELECT * FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                "\"user\"", "=", g_bindPrefix, "user", " AND ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name", " AND ",
                g_refPrefix, "age_years", "=", g_bindPrefix, g_refPrefix, "age_years",
                " LIMIT 1 ;"
                );
    const QString request = db::getFindSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields,
                refs,
                extraRefs
                );
    ASSERT_TRUE(equal(request, res0));
}

TEST(DBUtils, JsonSqlFieldsList_getInsertSqlRequest)
{
    db::JsonSqlFieldsList fields = {
        { "user", layout::JsonTypesEn::jt_int, true},
        { "user-id", layout::JsonTypesEn::jt_int, false},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const QString jsonLayoutName{"user-list"};
    const QString jsonRefName{"user-id"};
    const QStringList refs({"user-id", "name"});
    const QStringList extraRefs({"age-years",});

    const QString res0 = sum(
                "INSERT INTO ",
                "user_list", g_splitTableRef, "user_id",
                " ( ",
                g_refPrefix, "user_id", g_insertFieldSpliter,
                g_refPrefix, "name", g_insertFieldSpliter,
                g_refPrefix, "age_years", g_insertFieldSpliter,
                "\"user\"", g_insertFieldSpliter,
                "user_id", g_insertFieldSpliter,
                "name"
                " ) VALUES ( ",
                g_bindPrefix, g_refPrefix, "user_id", g_insertFieldSpliter,
                g_bindPrefix, g_refPrefix, "name", g_insertFieldSpliter,
                g_bindPrefix, g_refPrefix, "age_years", g_insertFieldSpliter,
                g_bindPrefix, "user", g_insertFieldSpliter,
                g_bindPrefix, "user_id", g_insertFieldSpliter,
                g_bindPrefix, "name"
                " ) ;"
                );
    const QString request = db::getInsertSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields,
                refs,
                extraRefs
                );
    ASSERT_TRUE(equal(request, res0));
}

TEST(DBUtils, JsonSqlFieldsList_setIdField)
{
    db::JsonSqlFieldsList fields1 = {
        { "id", layout::JsonTypesEn::jt_int, true},
    };
    QHash<QString, QVariant> values1(
                {
                    {"id", QVariant::fromValue(1)},
                    {"name", QVariant::fromValue(5)}
                });
    db::setIdField(fields1, values1, 2);
    ASSERT_EQ(values1["id"], QVariant::fromValue(2));

    db::JsonSqlFieldsList fields2 = {
        { "name", layout::JsonTypesEn::jt_int, false},
    };
    db::setIdField(fields2, values1, 3);
    ASSERT_EQ(values1["name"], QVariant::fromValue(5));
}

TEST(DBUtils, JsonSqlFieldsList_getNextIdSqlRequest)
{
    db::JsonSqlFieldsList fields = {
        { "user", layout::JsonTypesEn::jt_int, true},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const QString jsonLayoutName{"user-list"};
    const QString jsonRefName{"user-id"};

    const QString res0 = sum(
                "SELECT MAX(", "\"user\"", ") FROM ",
                "user_list", g_splitTableRef, "user_id",
                " ;"
                );
    const QString request = db::getNextIdSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields
                );
    ASSERT_TRUE(equal(request, res0));

    // return empty request string if no id field
    db::JsonSqlFieldsList fields1 = {
        { "no-id", layout::JsonTypesEn::jt_int, false},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const QString request1 = db::getNextIdSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields1
                );
    ASSERT_TRUE(equal(request1, QString{}));
}

TEST(DBUtils, JsonSqlFieldsList_getDeleteSqlRequest)
{
    db::JsonSqlFieldsList fields = {
        { "user", layout::JsonTypesEn::jt_int, true},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const QString jsonLayoutName{"user-list"};
    const QString jsonRefName{"user-id"};

    const QString res0 = sum(
                "DELETE FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                "\"user\"", "=", g_bindPrefix, "user"
                " ;"
                );
    const QString request = db::getDeleteSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields
                );
    ASSERT_TRUE(equal(request, res0));

    // return empty request string if no id field
    db::JsonSqlFieldsList fields1 = {
        { "no-id", layout::JsonTypesEn::jt_int, false},
        { "name", layout::JsonTypesEn::jt_string, false},
    };
    const QString request1 = db::getNextIdSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields1
                );
    ASSERT_TRUE(equal(request1, QString{}));
}

TEST(DBUtils, JsonSqlFieldsList_getUpdateSqlRequest)
{
    db::JsonSqlFieldsList fields = {
        { "user", layout::JsonTypesEn::jt_int, true},
        { "user-id", layout::JsonTypesEn::jt_int, false},
        { "all", layout::JsonTypesEn::jt_string, false},
    };
    const QString jsonLayoutName{"user-list"};
    const QString jsonRefName{"user-id"};

    const QString res0 = sum(
                "UPDATE ",
                "user_list", g_splitTableRef, "user_id",
                " SET ",
                "user_id", "=", g_bindPrefix, "user_id", g_insertFieldSpliter,
                "\"all\"", "=", g_bindPrefix, "all",
                " WHERE ",
                "\"user\"", "=", g_bindPrefix, "user",
                " ;"
                );
    const QString request = db::getUpdateSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields
                );
    ASSERT_TRUE(equal(request, res0));

    db::JsonSqlFieldsList fields2 = {
        { "user-id", layout::JsonTypesEn::jt_int, false},
        { "all", layout::JsonTypesEn::jt_string, false},
    };
    const QString request2 = db::getUpdateSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields2
                );
    ASSERT_TRUE(request2.isEmpty());
}

QString runGetSelectSqlRequestFoParams(const QHash<QString, QVariant> &params_)
{
    db::JsonSqlFieldsList fields = {
        { "user", layout::JsonTypesEn::jt_int, true},
        { "user-id", layout::JsonTypesEn::jt_int, false},
        { "all", layout::JsonTypesEn::jt_string, false},
    };
    const QString jsonLayoutName{"user-list"};
    const QString jsonRefName{"user-id"};
    const QStringList refs({"user-id", "name"});
    const QStringList extraRefs({"age-years",});

    QString request = db::getSelectSqlRequest(
                jsonLayoutName,
                jsonRefName,
                fields,
                refs,
                extraRefs,
                params_
                );
    return request;
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest)
{
    const QString res = sum(
                "SELECT  ",
                "\"user\"", g_insertFieldSpliter,
                "user_id", g_insertFieldSpliter,
                "\"all\"",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name", " AND ",
                g_refPrefix, "age_years", "=", g_bindPrefix, g_refPrefix, "age_years",
                "   ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                QHash<QString, QVariant>{}
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest_filterFields)
{
    const QHash<QString, QVariant> paramsFilterFields = {
        {g_procedureFilterNamesName, QVariant::fromValue(QStringList({"user-id", "all"}))}
    };
    const QString res = sum(
                "SELECT  ",
                "user_id", g_insertFieldSpliter,
                "\"all\"",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name", " AND ",
                g_refPrefix, "age_years", "=", g_bindPrefix, g_refPrefix, "age_years",
                "   ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                paramsFilterFields
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest_filterFields_fieldsFunction)
{
    const QHash<QString, QVariant> paramsFilterFields = {
        {g_procedureFilterNamesName, QVariant::fromValue(QStringList({"user-id", "all"}))},
        {g_procedureArgFunctionName, QVariant::fromValue(QString("SUM"))},
    };
    const QString res = sum(
                "SELECT  ",
                "SUM(", "user_id", ")", g_insertFieldSpliter,
                "SUM(", "\"all\"", ")",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name", " AND ",
                g_refPrefix, "age_years", "=", g_bindPrefix, g_refPrefix, "age_years",
                "   ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                paramsFilterFields
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest_filterFields_selectFunction)
{
    const QHash<QString, QVariant> paramsFilterFields = {
        {g_procedureFilterNamesName, QVariant::fromValue(QStringList({"user-id", "all"}))},
        {g_procedureSelectFunctionName, QVariant::fromValue(QString("DISTINCT"))},
    };
    const QString res = sum(
                "SELECT ",
                "DISTINCT ",
                "user_id", g_insertFieldSpliter,
                "\"all\"",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name", " AND ",
                g_refPrefix, "age_years", "=", g_bindPrefix, g_refPrefix, "age_years",
                "   ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                paramsFilterFields
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest_filterFields_orderBy)
{
    const QHash<QString, QVariant> paramsFilterFields = {
        { g_procedureFilterNamesName, QVariant::fromValue(QStringList({"user-id", "all"})) },
        { g_procedureOrderByName, QVariant::fromValue(QString{"\"all\""}) }
    };
    const QString res = sum(
                "SELECT  ",
                "user_id", g_insertFieldSpliter,
                "\"all\"",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name", " AND ",
                g_refPrefix, "age_years", "=", g_bindPrefix, g_refPrefix, "age_years",
                " ",
                g_procedureOrderByName, " ", "\"all\"",
                "  ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                paramsFilterFields
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest_filterFields_orderByAndLimit)
{
    const QHash<QString, QVariant> paramsFilterFields = {
        { g_procedureFilterNamesName, QVariant::fromValue(QStringList({"user-id", "all"})) },
        { g_procedureOrderByName, QVariant::fromValue(QString{"\"all\""}) },
        { g_procedureLimitName, QVariant::fromValue(QString{"1"}) },
    };
    const QString res = sum(
                "SELECT  ",
                "user_id", g_insertFieldSpliter,
                "\"all\"",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name", " AND ",
                g_refPrefix, "age_years", "=", g_bindPrefix, g_refPrefix, "age_years",
                " ",
                g_procedureOrderByName, " ", "\"all\"",
                " ",
                g_procedureLimitName, " ", "1",
                " ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                paramsFilterFields
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest_filterFields_orderByAndLimitProcFilterCond)
{
    const QHash<QString, QVariant> paramsFilterFields = {
        { g_procedureFilterNamesName, QVariant::fromValue(QStringList({"user-id", "all"})) },
        { g_procedureOrderByName, QVariant::fromValue(QString{"\"all\""}) },
        { g_procedureLimitName, QVariant::fromValue(QString{"1"}) },
        { g_procedureFilterConditionsName, QVariant::fromValue(QList<QVariant>(
          {QVariant::fromValue(QString("user-id")), QVariant::fromValue(QString("name"))})) },
    };
    const QString res = sum(
                "SELECT  ",
                "user_id", g_insertFieldSpliter,
                "\"all\"",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name",
                " ",
                g_procedureOrderByName, " ", "\"all\"",
                " ",
                g_procedureLimitName, " ", "1",
                " ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                paramsFilterFields
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

TEST(DBUtils, JsonSqlFieldsList_getSelectSqlRequest_filterFields_orderByAndLimitProcFilterCondUseCond)
{
    const QHash<QString, QVariant> paramsFilterFields = {
        { g_procedureFilterNamesName, QVariant::fromValue(QStringList({"user-id", "all"})) },
        { g_procedureOrderByName, QVariant::fromValue(QString{"\"all\""}) },
        { g_procedureLimitName, QVariant::fromValue(QString{"1"}) },
        { g_procedureFilterConditionsName, QVariant::fromValue(QList<QVariant>(
          {QVariant::fromValue(QString("user-id")), QVariant::fromValue(QString("name"))})) },
        { g_procedureConditionName, QVariant::fromValue(QString{"0 < 1"}) },
    };
    const QString res = sum(
                "SELECT  ",
                "user_id", g_insertFieldSpliter,
                "\"all\"",
                " FROM ",
                "user_list", g_splitTableRef, "user_id",
                " WHERE ",
                g_refPrefix, "user_id", "=", g_bindPrefix, g_refPrefix, "user_id", " AND ",
                g_refPrefix, "name", "=", g_bindPrefix, g_refPrefix, "name",
                " AND ( ", "0 < 1", " )"
                " ",
                g_procedureOrderByName, " ", "\"all\"",
                " ",
                g_procedureLimitName, " ", "1",
                " ;"
                );
    const QString request = runGetSelectSqlRequestFoParams(
                paramsFilterFields
                );
    qDebug() << request;
    qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

#endif
