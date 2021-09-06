/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#if !defined(DBUTILS_UNITTESTS)
#define DBUTILS_UNITTESTS

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <memory>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QHash>
#include "../MastactvaBase/dbutils.h"
#include "../MastactvaBase/localdataapinocache_default.h"
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
    //qDebug() << request;
    //qDebug() << res;
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
    //qDebug() << request;
    //qDebug() << res;
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
    //qDebug() << request;
    //qDebug() << res;
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
    //qDebug() << request;
    //qDebug() << res;
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
    //qDebug() << request;
    //qDebug() << res;
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
    //qDebug() << request;
    //qDebug() << res;
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
    //qDebug() << request;
    //qDebug() << res;
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
    //qDebug() << request;
    //qDebug() << res;
    ASSERT_TRUE(equal(request, res));
}

QHash<QString, QVariant> keysToKeyEmptyValue(const QStringList &keys_)
{
    QHash<QString, QVariant> result;
    for(const QString &key_ : keys_)
    {
        result.insert(key_, QVariant::fromValue(QString()));
    }
    return result;
}

void initForTest(
        DBRequestBase &requestInfo_,
        const QString &jsonLayoutName_,
        const QString &jsonReferenceName_,
        const QList<db::JsonSqlField> &jsonFieldInfo_,
        const QStringList &refs_,
        const QStringList &extraRefKeys_
        )
{
    requestInfo_.setTableName(jsonLayoutName_);
    requestInfo_.setCurrentRef(jsonReferenceName_);
    requestInfo_.setTableFieldsInfo(jsonFieldInfo_);
    requestInfo_.setRefs(refs_);
    requestInfo_.setExtraFields(keysToKeyEmptyValue(extraRefKeys_));
}

class SqlQueryMock : public db::ISqlQuery
{
public:
    MOCK_METHOD(bool, prepare, (const QString &), (override));
    MOCK_METHOD(bool, exec, (const QString &), (override));
    MOCK_METHOD(bool, exec, (), (override));
    MOCK_METHOD(bool, first, (), (override));
    MOCK_METHOD(bool, next, (), (override));
    MOCK_METHOD(QSqlError, lastError, (), (const, override));
    MOCK_METHOD(void, bindValue, (const QString &, const QVariant &), (override));
    MOCK_METHOD(QVariant, value, (const QString &), (const, override));
};

class SqlQueryOnMock: public db::ISqlQuery
{
public:
    SqlQueryOnMock(db::ISqlQuery *mock_):m_mock(mock_){}
    bool prepare(const QString &request_) override
    {
        return m_mock->prepare(request_);
    }
    bool exec(const QString &request_) override
    {
        return m_mock->exec(request_);
    }
    bool exec() override
    {
        return m_mock->exec();
    }
    bool first() override
    {
        return m_mock->first();
    }
    bool next() override
    {
        return m_mock->next();
    }
    QSqlError lastError() const override
    {
        return m_mock->lastError();
    }
    void bindValue(const QString& placeholder_, const QVariant& val_) override
    {
        m_mock->bindValue(placeholder_, val_);
    }
    QVariant value(const QString& name_) const override
    {
        return m_mock->value(name_);
    }
private:
    db::ISqlQuery *m_mock = nullptr;
};

template<typename SqlQueryMockType_>
class MockSingeltonSqlFactory : public ISqlQueryFactory
{
public:
    SqlQueryMockType_ mock;

    std::unique_ptr<db::ISqlQuery> getRequest(const DBRequestBase *r_) override
    {
        Q_UNUSED(r_);
        return std::make_unique<SqlQueryOnMock>(&mock);
    }

    std::pair<std::unique_ptr<db::ISqlQuery>,
        std::unique_ptr<db::ISqlQuery>> getRequestsPair(const DBRequestBase *r_) override
    {
        Q_UNUSED(r_);
        return std::make_pair(
            std::make_unique<SqlQueryOnMock>(&mock),
            std::make_unique<SqlQueryOnMock>(&mock)
            );
    }
};

TEST(LocalDataApiNoCache, getList)
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
    SaveDBRequest dbrequest;
    initForTest(dbrequest,
                jsonLayoutName,
                jsonRefName,
                fields,
                refs,
                extraRefs
                );

    auto mockFactory = std::make_shared<MockSingeltonSqlFactory<SqlQueryMock>>();
    LocalDataAPINoCacheImpl impl;
    impl.setQueryFactory(mockFactory);

    EXPECT_CALL(mockFactory->mock, exec(res0));
    impl.getListImpl(&dbrequest);
}

#endif
