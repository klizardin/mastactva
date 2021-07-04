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

namespace detail
{

template<typename Arg_>
auto countOf(Arg_ &&) -> char(*)[1];
template<typename Arg_, typename ... Args_>
auto countOf(Arg_ &&, Args_ &&... args_) -> char(*)[sizeof(decltype(*countOf(args_...))) + 1];

template<typename Arg_>
bool getArgument(lua_State *luaState_, int position_, Arg_ &arg_);

template<typename Arg_>
void traceArgument(lua_State *luaState_, int position_, Arg_ &arg_);

template<> inline
bool getArgument<QString>(lua_State *luaState_, int position_, QString &arg_)
{
    if(!lua_isstring(luaState_, position_))
    {
        return false;
    }

    arg_ = lua_tostring(luaState_, position_);
    return true;
}

template<> inline
void traceArgument<QString>(lua_State *luaState_, int position_, QString &arg_)
{
    Q_UNUSED(arg_);
    qDebug() << lua_type(luaState_, position_) << "(should be string)";
}

template<typename Arg_> inline
bool getArguments(lua_State *luaState_, int count_, int position_, Arg_ &arg_)
{
    return getArgument(luaState_, position_ - count_, arg_);
}

template<typename Arg_> inline
void traceArguments(lua_State *luaState_, int count_, int position_, Arg_ &arg_)
{
    traceArgument(luaState_, position_ - count_, arg_);
}

template<typename Arg_, typename ... Args_> inline
bool getArguments(lua_State *luaState_, int count_, int position_,  Arg_ &arg_, Args_ &... args_)
{
    if(!getArgument(luaState_, position_ - count_, arg_))
    {
        return false;
    }
    getArguments(luaState_, count_, position_ + 1, args_ ...);
}

template<typename Arg_, typename ... Args_> inline
void traceArguments(lua_State *luaState_, int count_, int position_, Arg_ &arg_, Args_ &... args_)
{
    traceArgument(luaState_, position_ - count_, arg_);
    traceArguments(luaState_, position_ - count_, args_ ...);
}

}

template<typename ... Args_> inline
bool getArguments(lua_State *luaState_, Args_ &... args_)
{
    constexpr int argumentsCount = sizeof(decltype(*detail::countOf(args_ ...)));
    const bool success = detail::getArguments(luaState_, argumentsCount, 0, args_ ...);
    if(!success)
    {
        qDebug() << "wrong argument type(s) : ";
        detail::traceArguments(luaState_, argumentsCount, 0, args_ ...);
    }
    return success;
}

void LuaAPI::getVariableImpl() const
{
    QString name;
    if(!getArguments(m_luaState, name))
    {
        processStack(1, 1);
        return;
    }
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

void LuaAPI::matrixIdentityImpl() const
{
    // arg1 - table with size, example {4,4}
    // result1 - table (matrix as array)
}

void LuaAPI::matrixIsIdentityImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - bool
}

void LuaAPI::matrixDeterminantImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - number
}

void LuaAPI::matrixInvertedImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixIsInvertibleImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - bool
}

void LuaAPI::matrixRotateImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (angles to rotate by axis)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixScaleImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (scale value by axis, 1.0 is default)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixShearImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (?)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixTranslateImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (shift value, default 0.0)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixFrustumImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixIsAphineImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - bool
}

void LuaAPI::matrixLookAtImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixNormalImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixOrthoImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixPerspectiveImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)
}

void LuaAPI::matrixViewportImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)
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

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixIdentity>() const
{
    matrixIdentityImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixIsIdentity>() const
{
    matrixIsIdentityImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixDeterminant>() const
{
    matrixDeterminantImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixInverted>() const
{
    matrixInvertedImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixIsInvertible>() const
{
    matrixIsInvertibleImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixRotate>() const
{
    matrixRotateImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixScale>() const
{
    matrixScaleImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixShear>() const
{
    matrixShearImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixTranslate>() const
{
    matrixTranslateImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixFrustrum>() const
{
    matrixFrustumImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixIsAphine>() const
{
    matrixIsAphineImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixLookAt>() const
{
    matrixLookAtImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixNormal>() const
{
    matrixNormalImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixOrtho>() const
{
    matrixOrthoImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixPerspective>() const
{
    matrixPerspectiveImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::matrixViewport>() const
{
    matrixViewportImpl();
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

    using ModuleFunctions = std::tuple<const char *, std::vector<FunctionInfo>>;
    static const ModuleFunctions moduleFunctions[] = {
        {
            "matrix",
            {
                {"identity", l_implementation<LuaAPI::FunctionImplEn::matrixIdentity, 1, 1>},
                {"isIdentity", l_implementation<LuaAPI::FunctionImplEn::matrixIsIdentity, 1, 1>},
                {"determinant", l_implementation<LuaAPI::FunctionImplEn::matrixDeterminant, 1, 1>},
                {"inverted", l_implementation<LuaAPI::FunctionImplEn::matrixInverted, 1, 1>},
                {"isInvertible", l_implementation<LuaAPI::FunctionImplEn::matrixIsInvertible, 1, 1>},
                {"rotate", l_implementation<LuaAPI::FunctionImplEn::matrixRotate, 2, 1>},
                {"scale", l_implementation<LuaAPI::FunctionImplEn::matrixScale, 2, 1>},
                {"shear", l_implementation<LuaAPI::FunctionImplEn::matrixShear, 2, 1>},
                {"translate", l_implementation<LuaAPI::FunctionImplEn::matrixTranslate, 2, 1>},
                {"frustrum", l_implementation<LuaAPI::FunctionImplEn::matrixFrustrum, 2, 1>},
                {"isAphine", l_implementation<LuaAPI::FunctionImplEn::matrixIsAphine, 1, 1>},
                {"lookAt", l_implementation<LuaAPI::FunctionImplEn::matrixLookAt, 2, 1>},
                {"normal", l_implementation<LuaAPI::FunctionImplEn::matrixNormal, 1, 1>},
                {"ortho", l_implementation<LuaAPI::FunctionImplEn::matrixOrtho, 2, 1>},
                {"perspective", l_implementation<LuaAPI::FunctionImplEn::matrixPerspective, 2, 1>},
                {"viewport", l_implementation<LuaAPI::FunctionImplEn::matrixViewport, 2, 1>},
            }
        }
    };
    for(const auto &mf_ : moduleFunctions)
    {
        lua_newtable(m_luaState);
        for(const auto &fi_ : std::get<1>(mf_))
        {
            const lua_CFunction func = std::get<1>(fi_);
            const char * funcName = std::get<0>(fi_);
            lua_pushcfunction(m_luaState, func);
            lua_setfield(m_luaState, -2, funcName);
        }
        const char *moduleName = std::get<0>(mf_);
        lua_setglobal(m_luaState, moduleName);
    }
}
