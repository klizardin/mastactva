#include "luaapi.h"
#include <QDebug>
#include <lua.hpp>


QHash<lua_State *, LuaAPI *> LuaAPI::s_apis;


LuaAPI::LuaAPI()
{
    m_luaState = luaL_newstate();
    luaL_openlibs(m_luaState);
    s_apis.insert(m_luaState, this);
}

LuaAPI::~LuaAPI()
{
    s_apis.remove(m_luaState);
    lua_close(m_luaState);
    m_luaState = nullptr;
}

bool LuaAPI::run(const QString &script_) const
{
    return loadScript(script_);
}

bool LuaAPI::call(
        const QString &script_,
        const QString &functionName_,
        std::map<QString, QVector<double>> &result_
        ) const
{
    if(!loadScript(script_))
    {
        return false;
    }
    initFunctions();
    if(!callFunction(functionName_))
    {
        return false;
    }
    if(!getNewVariables(result_))
    {
        qDebug() << "result type is not table";
        return false;
    }
    return true;
}

void LuaAPI::set(std::shared_ptr<IVariablesGetter> variablesGetter_)
{
    m_variablesGetter = variablesGetter_;
}

bool LuaAPI::pushVariableValue(const QString &name_) const
{
    QVector<double> value;
    if(!m_variablesGetter->get(name_, value))
    {
        return false;
    }
    push(value);
    return true;
}

LuaAPI *LuaAPI::getByState(lua_State *luaState_)
{
    if(s_apis.contains(luaState_))
    {
        return s_apis.value(luaState_);
    }
    else
    {
        return nullptr;
    }
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

bool LuaAPI::getNewVariables(std::map<QString, QVector<double>> &result_) const
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
            QVector<double> variableValues;
            lua_pushnil(m_luaState);
            while(lua_next(m_luaState, -2) != 0)
            {
                if(lua_isnumber(m_luaState, -1))
                {
                    variableValues.push_back(lua_tonumber(m_luaState, -1));
                }
                lua_pop(m_luaState, 1);
            }
            result_.insert({std::move(variableName), std::move(variableValues)});
        }
        lua_pop(m_luaState, 1);
    }
    lua_pop(m_luaState, 1);
    return true;
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

int l_getVariable(lua_State *luaState_)
{
    const QString name = lua_isstring(luaState_, -1)
            ? lua_tostring(luaState_, -1)
            : QString()
              ;
    lua_pop(luaState_, 1);
    LuaAPI *api = LuaAPI::getByState(luaState_);
    if(!api)
    {
        lua_pushnil(luaState_);
        return 1;
    }
    if(!api->pushVariableValue(name))
    {
        lua_pushnil(luaState_);
    }
    return 1;
}

void LuaAPI::initFunctions() const
{
    lua_pushcfunction(m_luaState, l_getVariable);
    lua_setglobal(m_luaState, "getVariable");
}
