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
#include "../MastactvaBase/drawingdata_utils.h"


class LuaAPI
{
private:
    enum class FunctionImplEn
    {
        getVariable,
        setVariable,
        setVariableWithPosition,
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

    static QString type2String(int type_);

private:
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
    void getVariableImpl() const;
    void setVariableImpl() const;
    void setVariableWithPositionImpl() const;
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
    void processStack(int inputArgsCount_, int outputArgsCount_) const;
    void hideLibsBlackList();
    template<FunctionImplEn func_>
    void functionImplementationDispatch() const;
    void initFunctions() const;

private:
    lua_State *m_luaState = nullptr;
    std::shared_ptr<drawingdata::IVariables> m_variables;
    mutable drawingdata::IPosition *m_artefactPosition = nullptr;
    static QHash<lua_State *, LuaAPI *> s_apis;

    template<FunctionImplEn impl_, int inputArgs_, int outputArgs_>
    friend int l_implementation(lua_State *luaState_);
};


#endif // LUAAPI_H
