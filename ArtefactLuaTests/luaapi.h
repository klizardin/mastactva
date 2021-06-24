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
    virtual bool add(const QString &name_, const QVector<double> &data_) = 0;
    bool add(const std::map<QString, QVector<double>> &variables_);
};


class LuaAPI
{
public:
    LuaAPI();
    ~LuaAPI();

    bool load(const QString &script_) const;
    bool call(
            const QString &functionName_,
            std::map<QString, QVector<double>> &result_
            ) const;
    void set(std::shared_ptr<IVariablesGetter> variablesGetter_);
    void set(std::shared_ptr<IVariablesSetter> variablesSetter_);
    bool pushVariableValue(const QString &name_) const;
    void getVariableImpl() const;
    void setVariableImpl() const;

    static LuaAPI *getByState(lua_State *luaState_);

private:
    void dumpStack() const;
    bool getNewVariables(std::map<QString, QVector<double>> &result_) const;
    bool ok(int error_, bool errorStrAtTop_ = true) const;
    bool loadScript(const QString &script_) const;
    bool callFunction(const QString &functionName_) const;
    void push(const QVector<double> &value_) const;
    QVector<double> getNumberList() const;
    void initFunctions() const;

private:
    lua_State *m_luaState = nullptr;
    std::shared_ptr<IVariablesGetter> m_variablesGetter;
    std::shared_ptr<IVariablesSetter> m_variablesSetter;
    static QHash<lua_State *, LuaAPI *> s_apis;
};


#endif // LUAAPI_H
