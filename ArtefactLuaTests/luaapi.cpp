#include "luaapi.h"
#include <QDebug>
#include <lua.hpp>


LuaAPI::LuaAPI()
{
    m_luaState = luaL_newstate();
    luaL_openlibs(m_luaState);
}

LuaAPI::~LuaAPI()
{
    lua_close(m_luaState);
    m_luaState = nullptr;
}

bool LuaAPI::run(const QString &script_) const
{
    int error = luaL_loadstring(m_luaState, script_.toUtf8().constData())
            || lua_pcall(m_luaState, 0, 0, 0);
    if(error)
    {
        qDebug() << QString(lua_tostring(m_luaState, -1));
        return false;
    }
    return true;
}

void LuaAPI::dumpStack()
{
    int top = lua_gettop(m_luaState);
    for (int i=1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(m_luaState,i));
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

bool LuaAPI::call(
        const QString &script_,
        const QString &functionName_,
        std::map<QString, QVector<double>> &result_
        ) const
{
    result_.clear();
    int error = luaL_loadstring(m_luaState, script_.toUtf8().constData())
            || lua_pcall(m_luaState, 0, 0, 0)
            ;
    if(error)
    {
        qDebug() << QString(lua_tostring(m_luaState, -1));
        return false;
    }
    lua_getglobal(m_luaState, functionName_.toUtf8().constData());
    error = lua_pcall(m_luaState, 0, 1, 0);
    if(error)
    {
        qDebug() << QString(lua_tostring(m_luaState, -1));
        return false;
    }
    if(!lua_istable(m_luaState, -1))
    {
        qDebug() << "result type is not table";
        return false;
    }
    lua_pushnil(m_luaState);
    while(lua_next(m_luaState, -2) != 0)
    {
        if(lua_isstring(m_luaState, -2)
                && lua_istable(m_luaState, -1))
        {
            const QString variableName = lua_tostring(m_luaState, -2);
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
