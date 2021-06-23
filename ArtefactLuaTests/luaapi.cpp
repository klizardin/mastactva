#include "luaapi.h"
#include <QDebug>
#include <lua.hpp>


LuaAPI::LuaAPI()
{
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
}

LuaAPI::~LuaAPI()
{
    lua_close(luaState);
    luaState = nullptr;
}

bool LuaAPI::run(const QString &script_) const
{
    int error = luaL_loadstring(luaState, script_.toUtf8().constData())
            || lua_pcall(luaState, 0, 0, 0);
    if(error)
    {
        qDebug() << QString(lua_tostring(luaState, -1));
        return false;
    }
    return true;
}

void dumpStack(lua_State *L)
{
    int top=lua_gettop(L);
    for (int i=1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L,i));
        switch (lua_type(L, i))
        {
        case LUA_TNUMBER:
            qDebug() << lua_tonumber(L,i);
            break;
        case LUA_TSTRING:
            qDebug() << lua_tostring(L,i);
            break;
        case LUA_TBOOLEAN:
            qDebug() << (lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNIL:
            qDebug() << "nil";
            break;
        default:
            qDebug() << lua_topointer(L,i);
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
    int error = luaL_loadstring(luaState, script_.toUtf8().constData())
            || lua_pcall(luaState, 0, 0, 0)
            ;
    if(error)
    {
        qDebug() << QString(lua_tostring(luaState, -1));
        return false;
    }
    dumpStack(luaState);
    lua_getglobal(luaState, functionName_.toUtf8().constData());
    dumpStack(luaState);
    error = lua_pcall(luaState, 0, 1, 0);
    if(error)
    {
        qDebug() << QString(lua_tostring(luaState, -1));
        return false;
    }
    if(!lua_istable(luaState, -1))
    {
        qDebug() << "result type is not table";
        return false;
    }
    lua_pushnil(luaState);
    while(lua_next(luaState, -2) != 0)
    {
        if(lua_isstring(luaState, -2)
                && lua_istable(luaState, -1))
        {
            const QString variableName = lua_tostring(luaState, -2);
            QVector<double> variableValues;
            lua_pushnil(luaState);
            while(lua_next(luaState, -2) != 0)
            {
                if(lua_isnumber(luaState, -1))
                {
                    variableValues.push_back(lua_tonumber(luaState, -1));
                }
                lua_pop(luaState, 1);
            }
            result_.insert({std::move(variableName), std::move(variableValues)});
        }
        lua_pop(luaState, 1);
    }
    lua_pop(luaState, 1);
    return true;
}
