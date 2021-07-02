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
        setVariable
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

private:
    void dumpStack() const;
    bool ok(int error_, bool errorStrAtTop_ = true) const;
    bool loadScript(const QString &script_) const;
    bool callFunction(const QString &functionName_) const;
    bool getReturnVariables(
            std::map<QString, QVector<double>> &result_,
            std::map<QString, QStringList> &resultStrs_
            ) const;
    void push(const QVector<double> &value_) const;
    void push(const QStringList &value_) const;
    std::tuple<QVector<double>, QStringList> getList() const;
    bool pushVariableValue(const QString &name_) const;
    static LuaAPI *getByState(lua_State *luaState_);
    void getVariableImpl() const;
    void setVariableImpl() const;
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
