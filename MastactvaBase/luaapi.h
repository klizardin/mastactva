/*
    Copyright 2021

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

#ifndef LUAAPI_H
#define LUAAPI_H


#include <memory>
#include <map>
#include <QString>
#include <QStringList>
#include <QVector>
#include <lua.hpp>
#include "../MastactvaBase/lua_utils.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/addonmodulelist.h"


class LuaAPITest
{
public:
    virtual QString getName() const = 0;
    virtual bool test(lua_State *luaState_, int position_) const = 0;
};


template<typename DataType_>
class LuaAPIDataTest : public LuaAPITest
{
public:
    LuaAPIDataTest(
            const QString &name_,
            const DataType_ &data_
            )
        : m_name(name_),
          m_data(data_)
    {
    }

    QString getName() const override
    {
        return m_name;
    }

    bool test(lua_State *luaState_, int position_) const override
    {
        DataType_ data{};
        getStructFromTable<DataType_>(luaState_, position_, data);
        return m_data == data;
    }

private:
    QString m_name;
    DataType_ m_data;
};


class TestObserver
{
public:
    virtual void onTest(const QString &name_, bool result_) const = 0;
};


class LuaAPI : public LuaAPIUtils
{
private:
    enum class FunctionImplEn
    {
        getVariable,
        setVariable,
        setVariableWithPosition,
        test,
        matrixIdentity,
        matrixIsIdentity,
        matrixDeterminant,
        matrixInverted,
        matrixIsInvertible,
        matrixRotate,
        matrixScale,
        matrixShear,
        matrixTranslate,
        matrixFrustrum,
        matrixIsAphine,
        matrixLookAt,
        matrixNormal,
        matrixOrtho,
        matrixPerspective,
        matrixViewport,
        effectGetFromImageUrl,
        effectGetToImageUrl,
        effectGetCurrent,
        effectFindObject,
        effectFindObjectArtefact,
        effectAddArgSet,
        effectGetArgSetArguments,
        effectAddArgValue,
        addonCall,
        addonGetNames,
        addonHasName,
        setTextureFileName,
        setTextureFromCurrentFrameBuffer
    };

public:
    LuaAPI();
    ~LuaAPI();

    bool load(const QString &script_) const;
    bool call(
            const QString &functionName_,
            drawingdata::IPosition *position_,
            std::map<QString, QVector<double>> &result_,
            std::map<QString, QStringList> &resultStrs_
            ) const;
    bool callArtefact(drawingdata::IPosition *position_);
    bool callArtefactAtRuntime(drawingdata::IPosition *position_) const;
    void set(std::shared_ptr<drawingdata::IVariables> variables_);
    void set(std::shared_ptr<drawingdata::ITextures> textures_);
    void addTest(std::unique_ptr<LuaAPITest> &&test_);
    void setTestObserver(std::shared_ptr<TestObserver> testObserver_);
    void set(std::shared_ptr<IAddonModules> addons_);

private:
    bool initAddonFunctions(const QStringList &names_);
    void dumpStack() const;
    bool ok(int error_, bool errorStrAtTop_ = true) const;
    bool loadScript(const QString &script_) const;
    bool callFunction(const QString &functionName_) const;
    bool getReturnVariables(
            std::map<QString, QVector<double>> &result_,
            std::map<QString, QStringList> &resultStrs_
            ) const;
    std::tuple<QVector<double>, QStringList> getList() const;
    std::tuple<bool, QVector<double>, QStringList> getVariableValue(const QString &name_) const;
    static LuaAPI *getByState(lua_State *luaState_);
    void processTests(const QString &name_, int position_) const;
    bool processAddon(const QString &name_, int position_) const;
    void getVariableImpl() const;
    void setVariableImpl() const;
    void setVariableWithPositionImpl() const;
    void testImpl() const;
    void addonCallImpl() const;
    void addonGetNamesImpl() const;
    void addonHasNameImpl() const;
    void matrixIdentityImpl() const;
    void matrixIsIdentityImpl() const;
    void matrixDeterminantImpl() const;
    void matrixInvertedImpl() const;
    void matrixIsInvertibleImpl() const;
    void matrixRotateImpl() const;
    void matrixScaleImpl() const;
    void matrixShearImpl() const;
    void matrixTranslateImpl() const;
    void matrixFrustumImpl() const;
    void matrixIsAphineImpl() const;
    void matrixLookAtImpl() const;
    void matrixNormalImpl() const;
    void matrixOrthoImpl() const;
    void matrixPerspectiveImpl() const;
    void matrixViewportImpl() const;
    void effectGetFromImageUrlImpl() const;
    void effectGetToImageUrlImpl() const;
    void effectGetCurrentImpl() const;
    void effectFindObjectImpl() const;
    void effectFindObjectArtefactImpl() const;
    void effectAddArgSetImpl() const;
    void effectGetArgSetArgumentsImpl() const;
    void effectAddArgValueImpl() const;
    void setTextureFileNameImpl() const;
    void setTextureFromCurrentFrameBufferImpl() const;
    void processStack(int inputArgsCount_, int outputArgsCount_) const;
    void hideLibsBlackList();
    template<FunctionImplEn func_>
    void functionImplementationDispatch() const;
    void initFunctions() const;

private:
    lua_State *m_luaState = nullptr;
    std::shared_ptr<drawingdata::IVariables> m_variables;
    std::shared_ptr<drawingdata::ITextures> m_textures;
    mutable drawingdata::IPosition *m_artefactPosition = nullptr;
    static QHash<lua_State *, LuaAPI *> s_apis;
    std::vector<std::unique_ptr<LuaAPITest>> m_tests;
    std::shared_ptr<TestObserver> m_testObserver;
    QStringList m_addonsNames;
    std::shared_ptr<IAddonModules> m_addons;

    template<FunctionImplEn impl_, int inputArgs_, int outputArgs_>
    friend int l_implementation(lua_State *luaState_);
};


#endif // LUAAPI_H
