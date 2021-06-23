#ifndef LUAAPI_H
#define LUAAPI_H


#include <map>
#include <QString>
#include <QVector>
#include <lua.hpp>


class LuaAPI
{
public:
    LuaAPI();
    ~LuaAPI();

    bool run(const QString &script_) const;
    bool call(
            const QString &script_,
            const QString &functionName_,
            std::map<QString, QVector<double>> &result_
            ) const;

private:
    void dumpStack() const;
    bool getNewVariables(std::map<QString, QVector<double>> &result_) const;
    bool check(int error_, bool errorStrAtTop_ = true) const;

private:
    lua_State *m_luaState = nullptr;
};


#endif // LUAAPI_H
