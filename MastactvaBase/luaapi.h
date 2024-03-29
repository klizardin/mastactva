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


/*
 * interface to test LUA API functionality
*/
class LuaAPITest
{
public:
    /*
     * return name of the test
    */
    virtual QString getName() const = 0;

    /*
     * run test for the data of the lua state in the stack at the position
    */
    virtual bool test(lua_State *luaState_, int position_) const = 0;
};


/*
 * LuaAPITest interface simple implementaion
*/
template<typename DataType_>
class LuaAPIDataTest : public LuaAPITest
{
public:
    LuaAPIDataTest(
            const QString &name_,   // test name
            const DataType_ &data_  // test data that should be places in the lua stack
            )
        : m_name(name_),
          m_data(data_)
    {
    }

    /*
     * name of test
    */
    QString getName() const override
    {
        return m_name;
    }

    /*
     * test with check the data structure item in the lua stack
    */
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


/*
 * interface to notifyu about tests results
*/
class TestObserver
{
public:
    /*
     * test with name is passed or is not
     * name_ - name of test
     * result_ - result of test with a name name_
    */
    virtual void onTest(const QString &name_, bool result_) const = 0;
};


class LuaAPI : public LuaAPIUtils
{
private:
    enum class FunctionImplEn
    {
        getVariable,                // get variable from variables data and put it to the lua stck
        setVariable,                // get value from the lua stack and put it to the variable
        setVariableWithPosition,    // set variable with position from the stack
        test,                       // run test with name and data from lua stack
        matrixIdentity,             // put identity matrix to the lua stack
        matrixIsIdentity,           // put true to the lua stack if matrix in the lua stack is identity matrix
        matrixDeterminant,          // put true to the lua stack if matrix in the lua stack has determinant
        matrixInverted,             // put interted matrix to the lua stack from the original matrix in the lua stack
        matrixIsInvertible,         // put true to the lua stack if matrix in the lua stack is invertible matrix
        matrixRotate,               // put rotate matrix to the lua stack from the argument from the lua stack
        matrixScale,                // put scale matrix to the lua stack from the argument from the lua stack
        matrixShear,                // put shear matrix to the lua stack from the argument from the lua stack TODO: implement?
        matrixTranslate,            // put translate matrix to the lua stack from the argument from the lua stack
        matrixFrustrum,             // put frustum matrix to the lua stack from the argument from the lua stack
        matrixIsAphine,             // put true to the lua stack if matrix in the lua stack is afine matrix
        matrixLookAt,               // put look at matrix to the lua stack from the argument from the lua stack
        matrixNormal,               // put normal matrix to the lua stack from the argument from the lua stack
        matrixOrtho,                // put ortho matrix to the lua stack from the argument from the lua stack
        matrixPerspective,          // put perspective matrix to the lua stack from the argument from the lua stack
        matrixViewport,             // put viewport matrix to the lua stack from the argument from the lua stack
        effectGetFromImageUrl,      // put name of the from image to the lua stack
        effectGetToImageUrl,        // put name of the to image to the lua stack
        effectGetCurrent,           // put current effect info to the table in the lua stack
        effectFindObject,           // put object's info to the lua stack as a table
        effectFindObjectArtefact,   // put object's artefact's info to the lua stack as a table
        effectAddArgSet,            // set arg set value from the arguments in the lua stack
        effectGetArgSetArguments,   // put arg set arguments value info to the lua stack as a table
        effectAddArgValue,          // put arg set arguments value to the lua stack as a table
        addonCall,                  // call addon from lua with arguments from the lua stack
        addonGetNames,              // put addons names to the lua stack
        addonHasName,               // put true if addons has name specified in the lua stack
        setTextureFileName,         // set texture file for the specific texture name from the arguments from the lua stack
        setTextureFromCurrentFrameBuffer    // set up texture with argument name with the current frame buffer
    };

public:
    LuaAPI();
    ~LuaAPI();

    bool load(const QString &script_) const;                // load script and initialize lua api class for this script
    bool call(
            const QString &functionName_,                   // function name to call
            drawingdata::IPosition *position_,              // current position in the effect drawing data
            std::map<QString, QVector<double>> &result_,    // result as vectors of double
            std::map<QString, QStringList> &resultStrs_     // result as vectors of strings
            ) const;                                        // make call to the script
    bool callArtefact(drawingdata::IPosition *position_);                   // call lua artefact -- with modification of variables by result
    bool callArtefactAtRuntime(drawingdata::IPosition *position_) const;    // call lua artefact in the runtime -- ignore result
    void set(std::shared_ptr<drawingdata::IVariables> variables_);          // set up shred interface
    void set(std::shared_ptr<drawingdata::ITextures> textures_);            // set up shared interface
    void addTest(std::unique_ptr<LuaAPITest> &&test_);                      // add test to the test's set
    void setTestObserver(std::shared_ptr<TestObserver> testObserver_);      // set up test observer interface
    void set(std::shared_ptr<IAddonModules> addons_);                       // set up addons interface

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
    lua_State *m_luaState = nullptr;                                // the current lua state
    std::shared_ptr<drawingdata::IVariables> m_variables;           // interface to the IVariables
    std::shared_ptr<drawingdata::ITextures> m_textures;             // interface to the ITextures
    mutable drawingdata::IPosition *m_artefactPosition = nullptr;   // position in the effect drawing data
    static QHash<lua_State *, LuaAPI *> s_apis;                     // static list of all APIs to serach implementation details by lua_State pointer
    std::vector<std::unique_ptr<LuaAPITest>> m_tests;               // tests
    std::shared_ptr<TestObserver> m_testObserver;                   // test's observer
    QStringList m_addonsNames;                                      // addon names
    std::shared_ptr<IAddonModules> m_addons;                        // addon interface

    template<FunctionImplEn impl_, int inputArgs_, int outputArgs_>
    friend int l_implementation(lua_State *luaState_);
};


#endif // LUAAPI_H
