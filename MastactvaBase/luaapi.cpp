/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "luaapi.h"
#include <math.h>
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
    m_variables->add(nullptr, std::move(result));
    m_variables->add(nullptr, std::move(resultStrs));
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
    m_variables = std::move(variables_);
}

void LuaAPI::addTest(std::unique_ptr<LuaAPITest> &&test_)
{
    m_tests.push_back(std::move(test_));
}

void LuaAPI::setTestObserver(std::shared_ptr<TestObserver> testObserver_)
{
    m_testObserver = testObserver_;
}

void LuaAPI::set(std::shared_ptr<AddonModules> addons_)
{
    m_addons = std::move(addons_);
    initAddonFunctions(m_addons ? m_addons->getNames() : QStringList{});
}

bool LuaAPI::initAddonFunctions(const QStringList &names_)
{
    m_addonsNames = names_;
    return true;
}

void LuaAPI::dumpStack() const
{
    LuaAPIUtils::dumpStack(m_luaState);
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

std::tuple<bool, QVector<double>, QStringList> LuaAPI::getVariableValue(const QString &name_) const
{
    std::tuple<bool, QVector<double>, QStringList> value;
    std::get<0>(value)
            = m_variables->get(name_, m_artefactPosition,  std::get<1>(value))
                || m_variables->get(name_, m_artefactPosition, std::get<2>(value))
            ;
    return value;
}

void LuaAPI::processTests(const QString &name_, int position_) const
{
    for(const std::unique_ptr<LuaAPITest> &test_ : m_tests)
    {
        if(!test_
                || test_->getName() != name_)
        {
            continue;
        }
        const bool result = test_->test(m_luaState, position_);
        if(m_testObserver)
        {
            m_testObserver->onTest(test_->getName(), result);
        }
    }
}

bool LuaAPI::processAddon(const QString &name_, int position_) const
{
    if(!m_addons)
    {
        QJsonDocument result{};
        pushTable(m_luaState, result);
        return true;
    }

    QJsonDocument arguments;
    getTable(m_luaState, position_, arguments);
    lua_pop(m_luaState, 2);
    QJsonDocument result = m_addons->call(name_, arguments);
    pushTable(m_luaState, result);
    return true;
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
    auto value = getVariableValue(name);
    if(!std::get<0>(value))
    {
        processStack(0, 1);
    }
    else if(!std::get<2>(value).isEmpty())
    {
        pushArguments(m_luaState, std::get<2>(value));
    }
    else
    {
        pushArguments(m_luaState, std::get<1>(value));
    }
}

void LuaAPI::setVariableImpl() const
{
    QString name;
    std::tuple<QVector<double>, QStringList> values;
    if(!getArguments(m_luaState, name, values))
    {
        processStack(2, 0);
        return;
    }
    const bool hasNumbers = !std::get<0>(values).isEmpty();
    const bool hasStrings = !std::get<1>(values).isEmpty();
    if(hasNumbers)
    {
        m_variables->add(name, nullptr, std::move(std::get<0>(values)));
    }
    else if(hasStrings)
    {
        m_variables->add(name, nullptr, std::move(std::get<1>(values)));
    }
    processStack(2, 0);
}

void LuaAPI::setVariableWithPositionImpl() const
{
    QString name;
    std::tuple<QVector<double>, QStringList> values;
    QString objectName;
    double objectPosition = 0;
    double artefactPosition = 0;
    if(!getArguments(m_luaState, name, values, objectName, objectPosition, artefactPosition))
    {
        processStack(5, 0);
        return;
    }
    const bool hasNumbers = !std::get<0>(values).isEmpty();
    const bool hasStrings = !std::get<1>(values).isEmpty();
    if(hasNumbers)
    {
        m_variables->add(
                    name,
                    objectName,
                    int(objectPosition),
                    int(artefactPosition),
                    std::move(std::get<0>(values))
                    );
    }
    else if(hasStrings)
    {
        m_variables->add(
                    name,
                    objectName,
                    int(objectPosition),
                    int(artefactPosition),
                    std::move(std::get<1>(values))
                    );
    }
    processStack(5, 0);
}

void LuaAPI::testImpl() const
{
    QString name;
    if(!getArguments(m_luaState, name))
    {
        processStack(2, 0);
        return;
    }
    processTests(name, 1);
    processStack(2, 0);
}

void LuaAPI::addonCallImpl() const
{
    QString name;
    if(!getArguments(m_luaState, name))
    {
        processStack(2, 1);
        return;
    }
    if(!processAddon(name, 1))
    {
        processStack(2, 1);
    }
}

void LuaAPI::addonGetNamesImpl() const
{
    pushArguments(m_luaState, m_addonsNames);
}

void LuaAPI::addonHasNameImpl() const
{
    QString name;
    if(!getArguments(m_luaState, name))
    {
        processStack(1, 1);
        return;
    }
    lua_pop(m_luaState, 1);
    const bool has = std::find(
                std::begin(m_addonsNames),
                std::end(m_addonsNames),
                name.trimmed()
                ) != std::end(m_addonsNames);
    pushArguments(m_luaState, has);
}

void LuaAPI::matrixIdentityImpl() const
{
    // arg1 - table with size, example {4,4}
    // result1 - table (matrix as array)

    QVector<double> size;
    if(!getArguments(m_luaState, size)
            || size.empty()
            )
    {
        processStack(1, 1);
        return;
    }
    lua_pop(m_luaState, 1);
    if(size.size() == 1)
    {
        switch(static_cast<int>(size.at(0)))
        {
        case 2:
        {
            QMatrix2x2 m;
            m.setToIdentity();
            pushArguments(m_luaState, m);
            break;
        }
        case 3:
        {
            QMatrix3x3 m;
            m.setToIdentity();
            pushArguments(m_luaState, m);
            break;
        }
        case 4:
        {
            QMatrix4x4 m;
            m.setToIdentity();
            pushArguments(m_luaState, m);
            break;
        }
        default:
            processStack(0, 1);
            break;
        }
    }
    else if(size.size() >= 2)
    {
        if(size.at(0) == 2 && size.at(1) == 2)
        {
            QMatrix2x2 m;
            m.setToIdentity();
            pushArguments(m_luaState, m);
        }
        else if(size.at(0) == 3 && size.at(1) == 3)
        {
            QMatrix3x3 m;
            m.setToIdentity();
            pushArguments(m_luaState, m);
        }
        else if(size.at(0) == 4 && size.at(1) == 4)
        {
            QMatrix4x4 m;
            m.setToIdentity();
            pushArguments(m_luaState, m);
        }
        else
        {
            processStack(0, 1);
        }
    }
    else
    {
        processStack(0, 1);
    }
}

void LuaAPI::matrixIsIdentityImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - bool

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    if(!getArguments(m_luaState, matrix)
            || 0 == std::get<0>(matrix)
            )
    {
        processStack(1, 1);
        return;
    }

    lua_pop(m_luaState, 1);

    bool result = false;
    switch(std::get<0>(matrix))
    {
    case 2:
        result = std::get<1>(matrix).isIdentity();
        break;
    case 3:
        result = std::get<2>(matrix).isIdentity();
        break;
    case 4:
        result = std::get<3>(matrix).isIdentity();
        break;
    default:
        processStack(0, 1);
        return;
    }
    pushArguments(m_luaState, result);
}

void LuaAPI::matrixDeterminantImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - number

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    if(!getArguments(m_luaState, matrix)
            || 0 == std::get<0>(matrix)
            )
    {
        processStack(1, 1);
        return;
    }

    lua_pop(m_luaState, 1);

    double result = false;
    switch(std::get<0>(matrix))
    {
    case 4:
        result = std::get<3>(matrix).determinant();
        break;
    default:
        processStack(0, 1);
        return;
    }
    pushArguments(m_luaState, result);
}

void LuaAPI::matrixInvertedImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    if(!getArguments(m_luaState, matrix)
            || 0 == std::get<0>(matrix)
            )
    {
        processStack(1, 1);
        return;
    }

    lua_pop(m_luaState, 1);

    bool invertible = false;
    switch(std::get<0>(matrix))
    {
    case 4:
        std::get<3>(matrix) = std::get<3>(matrix).inverted(&invertible);
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixIsInvertibleImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - bool

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    if(!getArguments(m_luaState, matrix)
            || 0 == std::get<0>(matrix)
            )
    {
        processStack(1, 1);
        return;
    }

    lua_pop(m_luaState, 1);

    bool invertible = false;
    switch(std::get<0>(matrix))
    {
    case 4:
        std::get<3>(matrix).inverted(&invertible);
        pushArguments(m_luaState, invertible);
        break;
    default:
        pushArguments(m_luaState, invertible);
        return;
    }
}

void LuaAPI::matrixRotateImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (angles to rotate by axis)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 4)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).rotate(params.at(0), params.at(1), params.at(2), params.at(3));
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixScaleImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (scale value by axis, 1.0 is default)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 3)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).scale(params.at(0), params.at(1), params.at(2));
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
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

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 3)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).translate(params.at(0), params.at(1), params.at(2));
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixFrustumImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 6)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).frustum(params.at(0), params.at(1), params.at(2),
                                      params.at(3), params.at(4), params.at(5));
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixIsAphineImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - bool

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    if(!getArguments(m_luaState, matrix)
            || 0 == std::get<0>(matrix)
            )
    {
        processStack(1, 1);
        return;
    }

    lua_pop(m_luaState, 1);

    bool isAphine = false;
    switch(std::get<0>(matrix))
    {
    case 2:
    case 3:
        break;
    case 4:
        isAphine = std::get<3>(matrix).isAffine();
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
    pushArguments(m_luaState, isAphine);
}

void LuaAPI::matrixLookAtImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 9)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).lookAt(QVector3D(params.at(0), params.at(1), params.at(2)),
                                    QVector3D(params.at(3), params.at(4), params.at(5)),
                                    QVector3D(params.at(6), params.at(7), params.at(8))
                                    );
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixNormalImpl() const
{
    // arg1 - table (matrix as array)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    if(!getArguments(m_luaState, matrix)
            || 0 == std::get<0>(matrix))
    {
        processStack(1, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 4:
    {
        pushArguments(m_luaState, std::get<3>(matrix).normalMatrix());
        break;
    }
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixOrthoImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 6)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).ortho(params.at(0), params.at(1), params.at(2),
                                      params.at(3), params.at(4), params.at(5));
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixPerspectiveImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 4)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).perspective(params.at(0), params.at(1), params.at(2), params.at(3));
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::matrixViewportImpl() const
{
    // arg1 - table (matrix as array)
    // arg2 - array of arguments (? ?named by position)
    // result1 - table (matrix as array)

    std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> matrix;
    QVector<double> params;
    if(!getArguments(m_luaState, matrix, params)
            || 0 == std::get<0>(matrix)
            || params.size() < 6)
    {
        processStack(2, 1);
        return;
    }

    lua_pop(m_luaState, 2);

    switch(std::get<0>(matrix))
    {
    case 2:
        pushArguments(m_luaState, std::get<1>(matrix));
        break;
    case 3:
        pushArguments(m_luaState, std::get<2>(matrix));
        break;
    case 4:
        std::get<3>(matrix).viewport(params.at(0), params.at(1), params.at(2),
                                      params.at(3), params.at(4), params.at(5));
        pushArguments(m_luaState, std::get<3>(matrix));
        break;
    default:
        Q_ASSERT(false); // wrong value
        processStack(0, 1);
        return;
    }
}

void LuaAPI::effectGetFromImageUrlImpl() const
{
    processStack(0, 1);
}

void LuaAPI::effectGetToImageUrlImpl() const
{
    processStack(0, 1);
}

void LuaAPI::effectGetCurrentImpl() const
{
    processStack(0, 1);
}

void LuaAPI::effectFindObjectImpl() const
{
    processStack(2, 1);
}

void LuaAPI::effectFindObjectArtefactImpl() const
{
    processStack(4, 1);
}

void LuaAPI::effectAddArgSetImpl() const
{
    processStack(3, 1);
}

void LuaAPI::effectGetArgSetArgumentsImpl() const
{
    processStack(1, 1);
}

void LuaAPI::effectAddArgValueImpl() const
{
    processStack(3, 1);
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
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::setVariableWithPosition>() const
{
    setVariableWithPositionImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::test>() const
{
    testImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::addonCall>() const
{
    addonCallImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::addonGetNames>() const
{
    addonGetNamesImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::addonHasName>() const
{
    addonHasNameImpl();
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

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectGetFromImageUrl>() const
{
    effectGetFromImageUrlImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectGetToImageUrl>() const
{
    effectGetToImageUrlImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectGetCurrent>() const
{
    effectGetCurrentImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectFindObject>() const
{
    effectFindObjectImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectFindObjectArtefact>() const
{
    effectFindObjectArtefactImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectAddArgSet>() const
{
    effectAddArgSetImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectGetArgSetArguments>() const
{
    effectGetArgSetArgumentsImpl();
}

template<>
void LuaAPI::functionImplementationDispatch<LuaAPI::FunctionImplEn::effectAddArgValue>() const
{
    effectAddArgValueImpl();
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
        {"setVariableWithPosition", l_implementation<LuaAPI::FunctionImplEn::setVariableWithPosition, 5, 0>},
        {"test", l_implementation<LuaAPI::FunctionImplEn::test, 2, 0>},
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
        },
        {
            "effect",
            {
                { "getFromImageUrl", l_implementation<LuaAPI::FunctionImplEn::effectGetFromImageUrl, 0, 1> },
                { "getToImageUrl", l_implementation<LuaAPI::FunctionImplEn::effectGetToImageUrl, 0, 1> },
                { "getCurrent", l_implementation<LuaAPI::FunctionImplEn::effectGetCurrent, 0, 1> },
                { "findObject", l_implementation<LuaAPI::FunctionImplEn::effectFindObject, 2, 1> },
                { "findObjectArtefact", l_implementation<LuaAPI::FunctionImplEn::effectFindObjectArtefact, 4, 1> },
                { "addArgSet", l_implementation<LuaAPI::FunctionImplEn::effectAddArgSet, 3, 1> },
                { "getArgSetArguments", l_implementation<LuaAPI::FunctionImplEn::effectGetArgSetArguments, 1, 1> },
                { "addArgValue", l_implementation<LuaAPI::FunctionImplEn::effectAddArgValue, 3, 1> },
            }
        },
        {
            "addon",
            {
                { "call", l_implementation<LuaAPI::FunctionImplEn::addonCall, 2, 1> },
                { "getNames", l_implementation<LuaAPI::FunctionImplEn::addonGetNames, 0, 1> },
                { "hasName", l_implementation<LuaAPI::FunctionImplEn::addonHasName, 1, 1> },
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
