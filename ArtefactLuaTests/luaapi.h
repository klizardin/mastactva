#ifndef LUAAPI_H
#define LUAAPI_H


#include <memory>
#include <map>
#include <QString>
#include <QVector>
#include <lua.hpp>


class IVariablesGetter
{
public:
    virtual ~IVariablesGetter() = default;
    virtual bool get(const QString &name_, QVector<double> &data_) const = 0;
};


class IVariablesSetter
{
public:
    virtual ~IVariablesSetter() = default;
    virtual bool add(const std::map<QString, QVector<double>> &variables_) = 0;
};


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
    void set(std::shared_ptr<IVariablesGetter> variablesGetter_);

private:
    void dumpStack() const;
    bool getNewVariables(std::map<QString, QVector<double>> &result_) const;
    bool ok(int error_, bool errorStrAtTop_ = true) const;
    bool loadScript(const QString &script_) const;
    bool callFunction(const QString &functionName_) const;

private:
    lua_State *m_luaState = nullptr;
    std::shared_ptr<IVariablesGetter> m_variablesGetter;
};


#endif // LUAAPI_H
