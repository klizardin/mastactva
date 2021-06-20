#include "luaapi.h"
#include <QDebug>
#include <lua.hpp>


bool LuaAPI::run(const QString &script_) const
{
    lua_State *luaState = luaL_newstate();
    luaL_openlibs(luaState);
    int error = luaL_loadstring(luaState, script_.toUtf8().constData())
            || lua_pcall(luaState, 0, 0, 0);
    if(error)
    {
        qDebug() << QString(lua_tostring(luaState, -1));
        return false;
    }
    lua_close(luaState);
    luaState = nullptr;
    return true;
}
