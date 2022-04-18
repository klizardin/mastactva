#ifndef TST_LUAAPI_H
#define TST_LUAAPI_H


#include <map>
#include <memory>
#include <QString>
#include <QVector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include "luaapi.h"


using namespace testing;


namespace
{
    const char *g_simpleCallTestCode =
            "function main ()\n"
            "   result = {}\n"
            "   result[\"a\"] = {1,2,3}\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_functionName = "main";
    const char *g_variablesCallTestCode =
            "function main ()\n"
            "   result = {}\n"
            "   result[\"a\"] = getVariable(\"a\")\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_setVariablesCallTestCode =
            "function main ()\n"
            "   setVariable(\"b\", getVariable(\"a\"))\n"
            "   return {}\n"
            "end\n"
            ;
    const char *g_setVariablesStrsCallTestCode =
            "function main ()\n"
            "   setVariable(\"b\", getVariable(\"a\"))\n"
            "   strResult = {\"1.0\", \"b\", \"c\"}\n"
            "   result = {}\n"
            "   result[\"a\"] = strResult\n"
            "   return result\n"
            "end\n"
            ;
    const char *g_badLibCallTestCode =
            "os.execute()\n"
            ;
    const char *g_badLibCallTest2Code =
            "debug.debug()\n"
            ;
    const char *g_matrixFunctionsSmokeCallTestCode =
            "function main ()\n"
            "   local pi = {4, 4}\n"
            "   local m = matrix.identity(pi)\n"
            "   local a = getVariable(\"a\")\n"
            "   local pry = {a[1], 0.0, 1.0, 0.0}\n"
            "   m = matrix.rotate(m, pry)\n"
            "   local prx = {a[1], 1.0, 0.0, 0.0}\n"
            "   m = matrix.rotate(m, prx)\n"
            "   local prz = {a[1], 0.0, 0.0, 1.0}\n"
            "   m = matrix.rotate(m, prz)\n"
            "   local ps = {2.0, 2.0, 2.0}\n"
            "   m = matrix.scale(m , ps)\n"
            "   local pt = {0.0, -0.2, 0.0}\n"
            "   m = matrix.translate(m , pt)\n"
            "   setVariable(\"m\", m)\n"
            "   result = {}\n"
            "   return result\n"
            "end\n"
            ;

    std::map<QString, QVector<double>> g_variables =
    {
        {
            QString("a"),
            {1.0, 2.0, 3.0}
        },
    };
    std::map<QString, QVector<double>> g_empty;
    std::map<QString, QStringList> g_emptyStrs;
    std::map<QString, QStringList> g_variablesStrs =
    {
        {
            QString("a"),
            {QString("1.0"), QString("b"), QString("c")}
        },
    };
}


TEST(LuaAPI, callReturn)
{
    LuaAPI luaAPI;
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_TRUE(luaAPI.load(g_simpleCallTestCode));
    EXPECT_TRUE(luaAPI.call(g_functionName, nullptr, result, resultStr));
    ASSERT_THAT(result, Eq(g_variables));
}


class VariablesMock : public drawingdata::IVariables
{
public:
    bool add(const QString &name_, const drawingdata::IPosition *position_, QVector<double> &&data_) override
    {
        Q_UNUSED(position_);
        return add_rvref(name_, std::move(data_));
    }

    bool add(const QString &name_, const drawingdata::IPosition *position_, QStringList &&data_) override
    {
        Q_UNUSED(position_);
        return add_rvrefsl(name_, std::move(data_));
    }
    MOCK_METHOD(bool, get, (const QString &, const drawingdata::IPosition *, QVector<int> &), (const, override));
    MOCK_METHOD(bool, get, (const QString &, const drawingdata::IPosition *, QVector<float> &), (const, override));
    MOCK_METHOD(bool, get, (const QString &, const drawingdata::IPosition *, QVector<double> &), (const, override));
    MOCK_METHOD(bool, get, (const QString &, const drawingdata::IPosition *, QStringList &), (const, override));
    MOCK_METHOD(void, add, (const QJsonDocument &), (override));
    MOCK_METHOD(bool, add, (const QString &, const drawingdata::IPosition *, const QVector<double> &), (override));
    MOCK_METHOD(bool, add, (const QString &, const drawingdata::IPosition *, const QStringList &), (override));
    MOCK_METHOD2(add_rvref, bool(const QString &, QVector<double>));
    MOCK_METHOD2(add_rvrefsl, bool(const QString &, QStringList));
    MOCK_METHOD(void, addAliases, (const QJsonDocument &, const drawingdata::IPosition *), (override));
    MOCK_METHOD(bool, getObjectsList, (QStringList &), (const, override));
    MOCK_METHOD(void, clear, (), (override));
};


bool returnA(const QString &, const drawingdata::IPosition *, QVector<double> &data_)
{
    data_ = g_variables["a"];
    return true;
}


TEST(LuaAPI, getVariable)
{
    std::shared_ptr<VariablesMock> variablesMock = std::make_shared<VariablesMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesMock);
    EXPECT_TRUE(luaAPI.load(g_variablesCallTestCode));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_CALL(*variablesMock, get(QString("a"), _, Matcher<QVector<double> &>(_)))
            .WillOnce(&returnA);
    EXPECT_TRUE(luaAPI.call(g_functionName, nullptr, result, resultStr));
    ASSERT_THAT(result, Eq(g_variables));
    ASSERT_THAT(resultStr, Eq(g_emptyStrs));
}

TEST(LuaAPI, setVariable)
{
    std::shared_ptr<VariablesMock> variablesMock = std::make_shared<VariablesMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesMock);
    EXPECT_TRUE(luaAPI.load(g_setVariablesCallTestCode));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_CALL(*variablesMock, get(QString("a"), _, Matcher<QVector<double> &>(_)))
            .WillOnce(&returnA);
    EXPECT_CALL(*variablesMock, add_rvref(QString("b"), g_variables["a"]))
            .WillOnce(Return(true));
    EXPECT_TRUE(luaAPI.call(g_functionName, nullptr, result, resultStr));
    ASSERT_THAT(result, Eq(g_empty));
    ASSERT_THAT(resultStr, Eq(g_emptyStrs));
}

TEST(LuaAPI, hideLibs)
{
    LuaAPI luaAPI;
    EXPECT_FALSE(luaAPI.load(g_badLibCallTestCode));
    EXPECT_FALSE(luaAPI.load(g_badLibCallTest2Code));
}

bool returnAStr(const QString &, const drawingdata::IPosition *, QStringList &data_)
{
    data_ = g_variablesStrs["a"];
    return true;
}

TEST(LuaAPI, strVariables)
{
    std::shared_ptr<VariablesMock> variablesMock = std::make_shared<VariablesMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesMock);
    EXPECT_TRUE(luaAPI.load(g_setVariablesStrsCallTestCode));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_CALL(*variablesMock, get(QString("a"), _, Matcher<QVector<double> &>(_)))
            .WillOnce(Return(false));
    EXPECT_CALL(*variablesMock, get(QString("a"), _, Matcher<QStringList &>(_)))
            .WillOnce(&returnAStr);
    EXPECT_CALL(*variablesMock, add_rvrefsl(QString("b"), g_variablesStrs["a"]))
            .WillOnce(Return(true));
    EXPECT_TRUE(luaAPI.call(g_functionName, nullptr, result, resultStr));
    ASSERT_THAT(result, Eq(g_empty));
    ASSERT_THAT(resultStr, Eq(g_variablesStrs));
}

TEST(LuaAPI, callArtefact)
{
    std::shared_ptr<VariablesMock> variablesMock = std::make_shared<VariablesMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesMock);
    EXPECT_TRUE(luaAPI.load(g_variablesCallTestCode));
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStr;
    EXPECT_CALL(*variablesMock, get(QString("a"), _, Matcher<QVector<double> &>(_)))
            .WillOnce(&returnA);
    EXPECT_CALL(*variablesMock, add_rvref(QString("a"), g_variables["a"]))
            .WillOnce(Return(true));
    EXPECT_TRUE(luaAPI.callArtefact(nullptr));
}

template<int angle_>
bool returnAngle(const QString &, const drawingdata::IPosition *, QVector<double> &data_)
{
    data_ = {static_cast<double>(angle_),};
    return true;
}

TEST(LuaAPI, matrixSmoke)
{
    std::shared_ptr<VariablesMock> variablesMock = std::make_shared<VariablesMock>();
    LuaAPI luaAPI;
    luaAPI.set(variablesMock);
    EXPECT_TRUE(luaAPI.load(g_matrixFunctionsSmokeCallTestCode));
    EXPECT_CALL(*variablesMock, get(QString("a"), _, Matcher<QVector<double> &>(_)))
            .WillOnce(&returnAngle<10>);

    qreal fScale = 2.0;
    qreal fAngle = 10.0;
    QMatrix4x4 modelview;
    modelview.rotate(fAngle, 0.0f, 1.0f, 0.0f);
    modelview.rotate(fAngle, 1.0f, 0.0f, 0.0f);
    modelview.rotate(fAngle, 0.0f, 0.0f, 1.0f);
    modelview.scale(fScale);
    modelview.translate(0.0f, -0.2f, 0.0f);
    QVector<double> matrixLuaRes;
    matrixLuaRes.reserve(4*4);
    std::copy(modelview.constData(), modelview.constData() + 4*4,
              std::back_inserter(matrixLuaRes));

    EXPECT_CALL(*variablesMock, add_rvref(QString("m"), matrixLuaRes))
            .WillOnce(Return(true));
    EXPECT_TRUE(luaAPI.callArtefact(nullptr));
}

#endif // TST_LUAAPI_H
