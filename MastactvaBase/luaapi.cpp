#include "luaapi.h"
#include <QDebug>
#include <lua.hpp>
#include "../MastactvaBase/names.h"


QHash<lua_State *, LuaAPI *> LuaAPI::s_apis;


LuaAPI::LuaAPI()
{
    m_luaState = luaL_newstate();
    luaL_openlibs(m_luaState);
    hideLibsBlackList();
    s_apis.insert(m_luaState, this);
}

LuaAPI::~LuaAPI()
{
    s_apis.remove(m_luaState);
    lua_close(m_luaState);
    m_luaState = nullptr;
}

LuaAPI *LuaAPI::getByState(lua_State *luaState_)
{
    return s_apis.value(luaState_, nullptr);
}

bool LuaAPI::load(const QString &script_) const
{
    if(!loadScript(script_))
    {
        return false;
    }
    initFunctions();
    return true;
}

bool LuaAPI::call(
        const QString &functionName_,
        drawingdata::IPosition *position_,
        std::map<QString, QVector<double>> &result_,
        std::map<QString, QStringList> &resultStrs_
        ) const
{
    m_artefactPosition = position_;
    if(!callFunction(functionName_))
    {
        return false;
    }
    if(!getReturnVariables(result_, resultStrs_))
    {
        qDebug() << "result type is not table";
        return false;
    }
    return true;
}

bool LuaAPI::callArtefact(drawingdata::IPosition *position_)
{
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStrs;
    const bool callResult = call(g_luaArtefactMainFunctionName, position_, result, resultStrs);
    if(!callResult)
    {
        return false;
    }
    if(!m_variables.operator bool())
    {
        return false;
    }
    m_variables->add(position_, std::move(result));
    m_variables->add(position_, std::move(resultStrs));
    return true;
}

bool LuaAPI::callArtefactAtRuntime(drawingdata::IPosition *position_) const
{
    std::map<QString, QVector<double>> result;
    std::map<QString, QStringList> resultStrs;
    const bool callResult = call(g_luaArtefactMainFunctionName, position_, result, resultStrs);
    Q_UNUSED(result);
    Q_UNUSED(resultStrs);
    return callResult;
}

void LuaAPI::set(std::shared_ptr<drawingdata::IVariables> variables_)
{
    m_variables = variables_;
}

void LuaAPI::dumpStack() const
{
    int top = lua_gettop(m_luaState);
    for (int i=1; i <= top; i++)
    {
        qDebug() << i << luaL_typename(m_luaState,i);
        switch (lua_type(m_luaState, i))
        {
        case LUA_TNUMBER:
            qDebug() << lua_tonumber(m_luaState,i);
            break;
        case LUA_TSTRING:
            qDebug() << lua_tostring(m_luaState,i);
            break;
        case LUA_TBOOLEAN:
            qDebug() << (lua_toboolean(m_luaState, i) ? "true" : "false");
            break;
        case LUA_TNIL:
            qDebug() << "nil";
            break;
        default:
            qDebug() << lua_topointer(m_luaState,i);
            break;
        }
    }
}

bool LuaAPI::ok(int error_, bool errorStrAtTop_ /*= true*/) const
{
    if(error_)
    {
        if(errorStrAtTop_)
        {
            qDebug() << QString(lua_tostring(m_luaState, -1));
        }
        return false;
    }
    return true;
}

bool LuaAPI::loadScript(const QString &script_) const
{
    if(!ok(luaL_loadstring(m_luaState, script_.toUtf8().constData()), false))
    {
        return false;
    }
    if(!ok(lua_pcall(m_luaState, 0, 0, 0)))
    {
        return false;
    }
    return true;
}

bool LuaAPI::callFunction(const QString &functionName_) const
{
    lua_getglobal(m_luaState, functionName_.toUtf8().constData());
    if(!ok(lua_pcall(m_luaState, 0, 1, 0)))
    {
        return false;
    }
    return true;
}

bool LuaAPI::getReturnVariables(
        std::map<QString, QVector<double>> &result_,
        std::map<QString, QStringList> &resultStrs_
        ) const
{
    result_.clear();
    if(!lua_istable(m_luaState, -1))
    {
        return false;
    }
    lua_pushnil(m_luaState);
    while(lua_next(m_luaState, -2) != 0)
    {
        if(lua_isstring(m_luaState, -2)
                && lua_istable(m_luaState, -1))
        {
            QString variableName = lua_tostring(m_luaState, -2);
            auto variableValues = getList();
            const bool hasNumbers = !std::get<0>(variableValues).isEmpty();
            const bool hasStrings = !std::get<1>(variableValues).isEmpty();
            if(hasNumbers)
            {
                result_.insert({
                                std::move(variableName),
                                std::move(std::get<0>(variableValues))
                            });
            }
            else if(hasStrings)
            {
                resultStrs_.insert({
                                std::move(variableName),
                                std::move(std::get<1>(variableValues))
                            });
            }
        }
        lua_pop(m_luaState, 1);
    }
    lua_pop(m_luaState, 1);
    return true;
}

void LuaAPI::push(const QVector<double> &value_) const
{
    lua_newtable(m_luaState);
    int index = 1;
    for(double v_ : value_)
    {
        lua_pushnumber(m_luaState, index);
        lua_pushnumber(m_luaState, v_);
        lua_settable(m_luaState, -3);
        ++index;
    }
}

void LuaAPI::push(const QStringList &value_) const
{
    lua_newtable(m_luaState);
    int index = 1;
    for(const QString &v_ : value_)
    {
        lua_pushnumber(m_luaState, index);
        lua_pushstring(m_luaState, v_.toUtf8().constData());
        lua_settable(m_luaState, -3);
        ++index;
    }
}

std::tuple<QVector<double>, QStringList> LuaAPI::getList() const
{
    std::tuple<QVector<double>, QStringList> values;
    lua_pushnil(m_luaState);
    if(0 == lua_next(m_luaState, -2))
    {
        return values;
    }

    const int firstItemType = lua_type(m_luaState, -1);
    if(LUA_TNUMBER == firstItemType)
    {
        std::get<0>(values).push_back(lua_tonumber(m_luaState, -1));
        lua_pop(m_luaState, 1);

        while(lua_next(m_luaState, -2) != 0)
        {
            if(lua_isnumber(m_luaState, -1))
            {
                std::get<0>(values).push_back(lua_tonumber(m_luaState, -1));
            }
            lua_pop(m_luaState, 1);
        }
    }
    else if(LUA_TSTRING == firstItemType)
    {
        std::get<1>(values).push_back(lua_tostring(m_luaState, -1));
        lua_pop(m_luaState, 1);

        while(lua_next(m_luaState, -2) != 0)
        {
            if(lua_isstring(m_luaState, -1))
            {
                std::get<1>(values).push_back(lua_tostring(m_luaState, -1));
            }
            lua_pop(m_luaState, 1);
        }
    }
    return values;
}

bool LuaAPI::pushVariableValue(const QString &name_) const
{
    QVector<double> value;
    QStringList valueStr;
    if(m_variables->get(name_, m_artefactPosition,  value))
    {
        push(value);
    }
    else if(m_variables->get(name_, m_artefactPosition, valueStr))
    {
        push(valueStr);
    }
    else
    {
        return false;
    }
    return true;
}

void LuaAPI::getVariableImpl() const
{
    // check arguments types
    if(!lua_isstring(m_luaState, -1))
    {
        qDebug() << "wrong argument types:"
            << lua_type(m_luaState, -1) << "(should be string)"
            ;
        processStack(1, 1);
        return;
    }
    const QString name = lua_tostring(m_luaState, -1);
    lua_pop(m_luaState, 1);
    if(!pushVariableValue(name))
    {
        processStack(0, 1);
    }
}

void LuaAPI::setVariableImpl() const
{
    // check arguments types
    if(!lua_isstring(m_luaState, -2)
            || !lua_istable(m_luaState, -1))
    {
        qDebug() << "wrong argument types:"
            << lua_type(m_luaState, -2) << "(should be string)"
            << lua_type(m_luaState, -1) << "(should be table of numbers)"
            ;
        processStack(2, 0);
        return;
    }
    if(!m_variables.operator bool())
    {
        processStack(2, 0);
        return;
    }
    const QString name = lua_tostring(m_luaState, -2);
    auto values = getList();
    const bool hasNumbers = !std::get<0>(values).isEmpty();
    const bool hasStrings = !std::get<1>(values).isEmpty();
    if(hasNumbers)
    {
        m_variables->add(name, m_artefactPosition, std::move(std::get<0>(values)));
    }
    else if(hasStrings)
    {
        m_variables->add(name, m_artefactPosition, std::move(std::get<1>(values)));
    }
    processStack(2, 0);
}

void LuaAPI::processStack(int inputArgsCount_, int outputArgsCount_) const
{
    lua_pop(m_luaState, inputArgsCount_);
    for(int i = 0; i < outputArgsCount_; ++i)
    {
        lua_pushnil(m_luaState);
    }
}

void LuaAPI::hideLibsBlackList()
{
    using GlobalNameType = std::tuple<const char *>;
    static const GlobalNameType s_globalNames[] =
    {
        {"io"},
        {"debug"},
        {"dofile"},
        {"load"},
        {"loadfile"}
    };
    for(const GlobalNameType &name_ : s_globalNames)
    {
        lua_pushnil(m_luaState);
        lua_setglobal(m_luaState, std::get<0>(name_));
    }
    using SpecificName2Type = std::tuple<const char *, const char *>;
    static const SpecificName2Type s_specificNames[] =
    {
        {"os", "execute"},
        {"os", "exit"},
        {"os", "getenv"},
        {"os", "remove"},
        {"os", "rename"},
        {"os", "setlocale"},
        {"os", "tmpname"},
    };
    for(const SpecificName2Type &name_ : s_specificNames)
    {
        lua_getglobal(m_luaState, std::get<0>(name_));
        lua_pushnil(m_luaState);
        lua_setfield(m_luaState, -2, std::get<1>(name_));
        lua_pop(m_luaState, 1);
    }
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::getVariable>() const
{
    getVariableImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::setVariable>() const
{
    setVariableImpl();
}

template<LuaAPI::FunctionImplEn function_, int inputArgsCount_, int outputArgsCount_>
int l_implementation(lua_State *luaState_)
{
    LuaAPI *api = LuaAPI::getByState(luaState_);
    if(!api)
    {
        lua_pop(luaState_, inputArgsCount_);
        for(int i = 0; i < outputArgsCount_; ++i)
        {
            lua_pushnil(luaState_);
        }
        return outputArgsCount_;
    }
    api->functionImplementationDispatch<function_>();
    return outputArgsCount_;
}

void LuaAPI::initFunctions() const
{
    using FunctionInfo = std::tuple<const char *, lua_CFunction>;
    static const FunctionInfo functions[] = {
        {"getVariable", l_implementation<LuaAPI::FunctionImplEn::getVariable, 1, 1>},
        {"setVariable", l_implementation<LuaAPI::FunctionImplEn::setVariable, 2, 0>},
    };

    for(const auto &val_ : functions)
    {
        lua_pushcfunction(m_luaState, std::get<1>(val_));
        lua_setglobal(m_luaState, std::get<0>(val_));
    }
}
