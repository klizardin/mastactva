#ifndef LUAAPI_H
#define LUAAPI_H


#include <QString>


class LuaAPI
{
public:
    bool run(const QString &script_) const;
};


#endif // LUAAPI_H
