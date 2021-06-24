#ifndef LUAAPI_H
#define LUAAPI_H


#include <memory>
#include <map>
#include <QString>
#include <QStringList>
#include <QVector>
#include <lua.hpp>


class IVariablesGetter
{
public:
    virtual ~IVariablesGetter() = default;
    virtual bool get(const QString &name_, QVector<double> &data_) const = 0;
    virtual bool get(const QString &name_, QStringList &data_) const = 0;
};


class IVariablesSetter
{
public:
    virtual ~IVariablesSetter() = default;
    virtual bool add(const QString &name_, const QVector<double> &data_) = 0;
    virtual bool add(const QString &name_, QVector<double> &&data_) = 0;
    virtual bool add(const QString &name_, const QStringList &data_) = 0;
    virtual bool add(const QString &name_, QStringList &&data_) = 0;
    bool add(const std::map<QString, QVector<double>> &variables_);
    bool add(std::map<QString, QVector<double>> &&variables_);
    bool add(const std::map<QString, QStringList> &variables_);
    bool add(std::map<QString, QStringList> &&variables_);

private:
    template<typename DataType_>
    bool addT(const std::map<QString, DataType_> &variables_);
    template<typename DataType_>
    bool addT(std::map<QString, DataType_> &&variables_);
};


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
            std::map<QString, QVector<double>> &result_,
            std::map<QString, QStringList> &resultStrs_
            ) const;
    void set(std::shared_ptr<IVariablesGetter> variablesGetter_);
    void set(std::shared_ptr<IVariablesSetter> variablesSetter_);

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
    std::shared_ptr<IVariablesGetter> m_variablesGetter;
    std::shared_ptr<IVariablesSetter> m_variablesSetter;
    static QHash<lua_State *, LuaAPI *> s_apis;

    template<FunctionImplEn impl_, int inputArgs_, int outputArgs_>
    friend int l_implementation(lua_State *luaState_);
};


#endif // LUAAPI_H