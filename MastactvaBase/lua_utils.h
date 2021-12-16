#ifndef LUA_UTILS_H
#define LUA_UTILS_H


#include <memory>
#include <map>
#include <math.h>
#include <lua.hpp>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>


static const int g_luaStartIndex = 1;


class LuaAPIUtils
{
public:
    static QString type2String(int type_);
    static void dumpStack(lua_State *luaState_);
};


inline
QString LuaAPIUtils::type2String(int type_)
{
    switch (type_)
    {
    case LUA_TNUMBER:
        return "LUA_TNUMBER";
        break;
    case LUA_TSTRING:
        return "LUA_TSTRING";
        break;
    case LUA_TBOOLEAN:
        return "LUA_TBOOLEAN";
        break;
    case LUA_TNIL:
        return "LUA_TNIL";
        break;
    default:
        return QString("unknown %1").arg(type_);
        break;
    }
}

inline
void LuaAPIUtils::dumpStack(lua_State *luaState_)
{
    int top = lua_gettop(luaState_);
    for (int i=1; i <= top; i++)
    {
        qDebug() << i << "(" << (i - top - 1) << ")" << luaL_typename(luaState_, i);
        switch (lua_type(luaState_, i))
        {
        case LUA_TNUMBER:
            qDebug() << lua_tonumber(luaState_, i);
            break;
        case LUA_TSTRING:
            qDebug() << lua_tostring(luaState_, i);
            break;
        case LUA_TBOOLEAN:
            qDebug() << (lua_toboolean(luaState_, i) ? "true" : "false");
            break;
        case LUA_TNIL:
            qDebug() << "nil";
            break;
        default:
            qDebug() << lua_topointer(luaState_, i);
            break;
        }
    }
}

template<class DataType_, typename Arg_>
struct FieldLayout
{
    using DataType = DataType_;
    using FieldType = Arg_;

    FieldLayout(const char *name_, Arg_ DataType_::* field_)
        : m_name(name_),
          m_field(field_)
    {
    }

    const char * getName() const
    {
        return m_name;
    }

    Arg_ & getDataRef(DataType_ &data_) const
    {
        return data_.*m_field;
    }

private:
    const char *m_name = nullptr;
    Arg_ DataType_::*m_field;
};

template<typename ... Args_>
struct DataLayout
{
    using NextLayout = void *;
};

template<typename Arg_, typename ... Args_>
struct DataLayout<Arg_, Args_ ...> : public DataLayout<Args_ ...>
{
    using NextLayout = DataLayout<Args_ ...>;
    using DataType = typename Arg_::DataType;
    using FieldType = typename Arg_::FieldType;

    DataLayout(Arg_ &&field_, Args_ &&...values_)
        : DataLayout<Args_ ...>(std::move(values_) ...),
          m_field(std::move(field_))
    {
    }

    const char * getName() const
    {
        return m_field.getName();
    }

    FieldType & getDataRef(DataType &data_) const
    {
        return m_field.getDataRef(data_);
    }

    operator void * () const
    {
        return nullptr;
    }

private:
    Arg_ m_field;
};

template<typename Arg_>
struct DataLayout<Arg_>
{
    using NextLayout = void *;
    using DataType =  typename Arg_::DataType;
    using FieldType = typename Arg_::FieldType;

    DataLayout(Arg_ &&field_)
        : m_field(std::move(field_))
    {
    }

    const char * getName() const
    {
        return m_field.getName();
    }

    FieldType & getDataRef(DataType &data_) const
    {
        return m_field.getDataRef(data_);
    }

    operator void * () const
    {
        return nullptr;
    }

private:
    Arg_ m_field;
};


template<class DataType_, typename Arg_>
FieldLayout<DataType_, Arg_> makeFieldLayout(const char *name_, Arg_ DataType_::* field_)
{
    return FieldLayout<DataType_, Arg_>(name_, field_);
}

template<typename Arg_, typename ... Args_>
DataLayout<Arg_, Args_ ...> makeDataLayout(Arg_ &&field_, Args_ &&...values_)
{
    return DataLayout<Arg_, Args_ ...>(std::move(field_), std::move(values_) ...);
}

template<typename Arg_>
DataLayout<Arg_> makeDataLayout(Arg_ &&field_)
{
    return DataLayout<Arg_>(std::move(field_));
}

namespace detail
{

template<typename ... Args_>
struct countOf;

template<typename Arg_>
struct countOf<Arg_>
{
    static auto get() -> char(*)[1];
};

template<typename Arg_, typename ... Args_>
struct countOf<Arg_, Args_ ...>
{
    static auto get() -> char(*)[sizeof(decltype(*countOf<Args_...>::get())) + 1];
};

template<typename Arg_> inline
bool getArgument(lua_State *luaState_, int position_, Arg_ &arg_);

template<typename Arg_> inline
void traceArgument(lua_State *luaState_, int position_, Arg_ &arg_);

template<typename Arg_> inline
void pushArgument(lua_State *luaState_, const Arg_ &arg_);

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
bool getArgument<bool>(lua_State *luaState_, int position_, bool &arg_)
{
    if(!lua_isboolean(luaState_, position_))
    {
        return false;
    }

    arg_ = lua_toboolean(luaState_, position_) != 0;
    return true;
}

template<> inline
bool getArgument<double>(lua_State *luaState_, int position_, double &arg_)
{
    if(!lua_isnumber(luaState_, position_))
    {
        return false;
    }

    arg_ = lua_tonumber(luaState_, position_);
    return true;
}

template<> inline
bool getArgument<int>(lua_State *luaState_, int position_, int &arg_)
{
    if(!lua_isnumber(luaState_, position_))
    {
        return false;
    }

    arg_ = int(lua_tonumber(luaState_, position_));
    return true;
}

template<> inline
bool getArgument<QVector<double>>(lua_State *luaState_, int position_, QVector<double> &arg_)
{
    if(!lua_istable(luaState_, position_))
    {
        return false;
    }

    arg_.clear();
    lua_pushnil(luaState_);
    if(0 == lua_next(luaState_, position_ - 1))
    {
        return true;
    }

    const int firstItemType = lua_type(luaState_, -1);
    if(LUA_TNUMBER == firstItemType)
    {
        arg_.push_back(lua_tonumber(luaState_, -1));
        lua_pop(luaState_, 1);

        while(lua_next(luaState_, position_ - 1) != 0)
        {
            if(lua_isnumber(luaState_, -1))
            {
                arg_.push_back(lua_tonumber(luaState_, -1));
            }
            lua_pop(luaState_, 1);
        }
    }

    return true;
}

template<> inline
bool getArgument<std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4>>(
        lua_State *luaState_,
        int position_,
        std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> &arg_
        )
{
    std::get<0>(arg_) = 0;
    QVector<double> data;
    if(!getArgument(luaState_, position_, data))
    {
        return false;
    }

    if(data.size() == 2*2)
    {
        std::get<0>(arg_) = 2;
        for(int i = 0; i < data.size(); ++i)
        {
            std::get<1>(arg_).data()[i] = data[i];
        }
        return true;
    }
    else if(data.size() == 3*3)
    {
        std::get<0>(arg_) = 3;
        for(int i = 0; i < data.size(); ++i)
        {
            std::get<2>(arg_).data()[i] = data[i];
        }
        return true;
    }
    else if(data.size() == 4*4)
    {
        std::get<0>(arg_) = 4;
        for(int i = 0; i < data.size(); ++i)
        {
            std::get<3>(arg_).data()[i] = data[i];
        }
        return true;
    }

    return false;
}

template<> inline
bool getArgument<QStringList>(lua_State *luaState_, int position_, QStringList &arg_)
{
    if(!lua_istable(luaState_, position_))
    {
        return false;
    }

    arg_.clear();
    lua_pushnil(luaState_);
    if(0 == lua_next(luaState_, position_ - 1))
    {
        return true;
    }

    const int firstItemType = lua_type(luaState_, -1);
    if(LUA_TSTRING == firstItemType)
    {
        arg_.push_back(lua_tostring(luaState_, -1));
        lua_pop(luaState_, 1);

        while(lua_next(luaState_, position_ - 1) != 0)
        {
            if(lua_isstring(luaState_, -1))
            {
                arg_.push_back(lua_tostring(luaState_, -1));
            }
            lua_pop(luaState_, 1);
        }
    }

    return true;
}

template<> inline
bool getArgument<std::tuple<QVector<double>, QStringList>>(
        lua_State *luaState_,
        int position_,
        std::tuple<QVector<double>, QStringList> &arg_
        )
{
    if(!lua_istable(luaState_, position_))
    {
        return false;
    }

    std::get<0>(arg_).clear();
    std::get<1>(arg_).clear();
    lua_pushnil(luaState_);
    if(0 == lua_next(luaState_, position_ - 1))
    {
        return true;
    }

    const int firstItemType = lua_type(luaState_, -1);
    if(LUA_TNUMBER == firstItemType)
    {
        std::get<0>(arg_).push_back(lua_tonumber(luaState_, -1));
        lua_pop(luaState_, 1);

        while(lua_next(luaState_, position_ - 1) != 0)
        {
            if(lua_isnumber(luaState_, -1))
            {
                std::get<0>(arg_).push_back(lua_tonumber(luaState_, -1));
            }
            lua_pop(luaState_, 1);
        }
    }
    else if(LUA_TSTRING == firstItemType)
    {
        std::get<1>(arg_).push_back(lua_tostring(luaState_, -1));
        lua_pop(luaState_, 1);

        while(lua_next(luaState_, position_ - 1) != 0)
        {
            if(lua_isstring(luaState_, -1))
            {
                std::get<1>(arg_).push_back(lua_tostring(luaState_, -1));
            }
            lua_pop(luaState_, 1);
        }
    }

    return true;
}

template<> inline
void traceArgument<QString>(lua_State *luaState_, int position_, QString &arg_)
{
    Q_UNUSED(arg_);
    qDebug() << LuaAPIUtils::type2String(lua_type(luaState_, position_)) << "(should be string)";
}

template<> inline
void traceArgument<bool>(lua_State *luaState_, int position_, bool &arg_)
{
    Q_UNUSED(arg_);
    qDebug() << LuaAPIUtils::type2String(lua_type(luaState_, position_)) << "(should be boolean)";
}

template<> inline
void traceArgument<double>(lua_State *luaState_, int position_, double &arg_)
{
    Q_UNUSED(arg_);
    qDebug() << LuaAPIUtils::type2String(lua_type(luaState_, position_)) << "(should be number)";
}

template<> inline
void traceArgument<QVector<double>>(lua_State *luaState_, int position_, QVector<double> &arg_)
{
    Q_UNUSED(arg_);
    qDebug() << LuaAPIUtils::type2String(lua_type(luaState_, position_)) << "(should be table)";
}

template<> inline
void traceArgument<QStringList>(lua_State *luaState_, int position_, QStringList &arg_)
{
    Q_UNUSED(arg_);
    qDebug() << LuaAPIUtils::type2String(lua_type(luaState_, position_)) << "(should be table)";
}

template<> inline
void traceArgument<std::tuple<QVector<double>, QStringList>>(
        lua_State *luaState_,
        int position_,
        std::tuple<QVector<double>, QStringList> &arg_
        )
{
    Q_UNUSED(arg_);
    qDebug() << LuaAPIUtils::type2String(lua_type(luaState_, position_)) << "(should be table)";
}

template<> inline
void traceArgument<std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4>>(
        lua_State *luaState_,
        int position_,
        std::tuple<int, QMatrix2x2, QMatrix3x3, QMatrix4x4> &arg_
        )
{
    Q_UNUSED(arg_);
    qDebug() << LuaAPIUtils::type2String(lua_type(luaState_, position_)) << "(should be table)";
}

template<> inline
void pushArgument<QString>(lua_State *luaState_, const QString &arg_)
{
    lua_pushstring(luaState_, arg_.toUtf8().constData());
}

template<> inline
void pushArgument<bool>(lua_State *luaState_, const bool &arg_)
{
    lua_pushboolean(luaState_, arg_ ? 1 : 0);
}

template<> inline
void pushArgument<int>(lua_State *luaState_, const int &arg_)
{
    lua_pushnumber(luaState_, arg_);
}

template<> inline
void pushArgument<float>(lua_State *luaState_, const float &arg_)
{
    lua_pushnumber(luaState_, arg_);
}

template<> inline
void pushArgument<double>(lua_State *luaState_, const double &arg_)
{
    lua_pushnumber(luaState_, arg_);
}

template<> inline
void pushArgument<QVector<double>>(lua_State *luaState_, const QVector<double> &arg_)
{
    lua_newtable(luaState_);
    int index = 1;
    for(double v_ : arg_)
    {
        lua_pushnumber(luaState_, index);
        lua_pushnumber(luaState_, v_);
        lua_settable(luaState_, -3);
        ++index;
    }
}

template<int size_, typename MatrixArg_> inline
void pushMatrix(lua_State *luaState_, const MatrixArg_ &arg_)
{
    QVector<double> data;
    data.reserve(size_);
    std::copy(arg_.constData(), arg_.constData() + size_,
              std::back_inserter(data));
    pushArgument(luaState_, data);
}

template<> inline
void pushArgument<QMatrix2x2>(lua_State *luaState_, const QMatrix2x2 &arg_)
{
    pushMatrix<2*2>(luaState_, arg_);
}

template<> inline
void pushArgument<QMatrix3x3>(lua_State *luaState_, const QMatrix3x3 &arg_)
{
    pushMatrix<3*3>(luaState_, arg_);
}

template<> inline
void pushArgument<QMatrix4x4>(lua_State *luaState_, const QMatrix4x4 &arg_)
{
    pushMatrix<4*4>(luaState_, arg_);
}

template<> inline
void pushArgument<QStringList>(lua_State *luaState_, const QStringList &arg_)
{
    lua_newtable(luaState_);
    int index = 1;
    for(const QString &v_ : arg_)
    {
        lua_pushnumber(luaState_, index);
        lua_pushstring(luaState_, v_.toUtf8().constData());
        lua_settable(luaState_, -3);
        ++index;
    }
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

template<typename Arg_> inline
void pushArguments(lua_State *luaState_, const Arg_ &arg_)
{
    pushArgument(luaState_, arg_);
}

template<typename Arg_, typename ... Args_> inline
bool getArguments(lua_State *luaState_, int count_, int position_,  Arg_ &arg_, Args_ &... args_)
{
    if(!getArgument(luaState_, position_ - count_, arg_))
    {
        return false;
    }
    getArguments(luaState_, count_, position_ + 1, args_ ...);
    return true;
}

template<typename Arg_, typename ... Args_> inline
void traceArguments(lua_State *luaState_, int count_, int position_, Arg_ &arg_, Args_ &... args_)
{
    traceArgument(luaState_, position_ - count_, arg_);
    traceArguments(luaState_, count_, position_, args_ ...);
}

template<typename Arg_, typename ... Args_> inline
void pushArguments(lua_State *luaState_, const Arg_ &arg_, const Args_ &... args_)
{
    pushArgument(luaState_, arg_);
    pushArguments(luaState_, args_ ...);
}

template<typename DataType_, typename LayoutArg_> inline
void getStructItemFromTable(
        lua_State *luaState_,
        int position_,
        DataType_ &data_,
        const LayoutArg_ &layoutArg_
        )
{
    const char *name = layoutArg_.getName();
    if(!lua_getfield(luaState_, position_, name))
    {
        return;
    }
    detail::getArgument(luaState_, -1, layoutArg_.getDataRef(data_));
    lua_pop(luaState_, 1);
}

template<typename DataType_> inline
void getStructItemFromTable(
        lua_State *luaState_,
        int position_,
        DataType_ &data_,
        void *
        )
{
    Q_UNUSED(luaState_);
    Q_UNUSED(position_);
    Q_UNUSED(data_);
}

template<typename DataType_, typename ... LayoutArgs_> inline
void getStructFromTable(
        lua_State *luaState_,
        int position_,
        DataType_ &data_,
        void *
        )
{
    Q_UNUSED(luaState_);
    Q_UNUSED(position_);
    Q_UNUSED(data_);
}

template<typename DataType_, typename ... LayoutArgs_> inline
void getStructFromTable(
        lua_State *luaState_,
        int position_,
        DataType_ &data_,
        const DataLayout<LayoutArgs_ ...> &layout_
        )
{
    detail::getStructItemFromTable(luaState_, position_, data_, layout_);
    detail::getStructFromTable(luaState_, position_, data_,
                               static_cast<const typename DataLayout<LayoutArgs_ ...>::NextLayout &>(layout_)
                               );
}

inline
bool isArray(const QJsonObject &obj_)
{
    const QStringList keys = obj_.keys();
    std::vector<int> numbers;
    static const int s_nan = g_luaStartIndex - 1;
    std::transform(
                std::begin(keys),
                std::end(keys),
                std::back_inserter(numbers),
                [](const QString &str_)->int
    {
        const int value = str_.toInt();
        return QString("%1").arg(value) == str_.trimmed() ? value : s_nan;
    });
    const auto fit = std::find(std::begin(numbers), std::end(numbers), s_nan);
    if(std::end(numbers) != fit)
    {
        return false;
    }
    std::sort(std::begin(numbers), std::end(numbers));
    int expected = g_luaStartIndex;
    for(int n_ : numbers)
    {
        if(expected != n_)
        {
            return false;
        }
        ++expected;
    }
    return true;
}

inline
QJsonArray convertToArray(const QJsonObject &obj_)
{
    if(!isArray(obj_))
    {
        return QJsonArray{};
    }
    const QStringList keys = obj_.keys();
    std::vector<int> numbers;
    std::transform(
                std::begin(keys),
                std::end(keys),
                std::back_inserter(numbers),
                [](const QString &str_)->int
    {
        return str_.toInt();
    });
    std::sort(std::begin(numbers), std::end(numbers));
    QJsonArray result{};
    for(int n_ : numbers)
    {
        const QJsonValue value = obj_.value(QString("%1").arg(n_));
        result.push_back(value);
    }
    return result;
}

inline
void getTable(
        lua_State *luaState_,
        int position_,
        QJsonObject &obj_
        )
{
    /* table is in the stack at index 't' */
    lua_pushnil(luaState_);  /* first key */
    while (0 != lua_next(luaState_, position_))
    {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        static const int s_keyIndex = -2;
        static const int s_valueIndex = -1;
        if(!lua_isstring(luaState_, s_keyIndex)
                && !lua_isnumber(luaState_, s_keyIndex)
                )
        {
            // invalid key, skip
            lua_pop(luaState_, 1);
            continue;
        }
        QString key = lua_isstring(luaState_, s_keyIndex)
                ? lua_tostring(luaState_, s_keyIndex)
                : QString("%1").arg(lua_tonumber(luaState_, s_keyIndex))
                ;
        switch(lua_type(luaState_, s_valueIndex))
        {
        case LUA_TNIL:
        {
            obj_.insert(key, QJsonValue::fromVariant(QVariant{}));
            break;
        }
        case LUA_TNUMBER:
        {
            double value = 0.0;
            getArgument(luaState_, s_valueIndex, value);
            obj_.insert(key, QJsonValue::fromVariant(QVariant::fromValue(value)));
            break;
        }
        case LUA_TBOOLEAN:
        {
            bool value = false;
            getArgument(luaState_, s_valueIndex, value);
            obj_.insert(key, QJsonValue::fromVariant(QVariant::fromValue(value)));
            break;
        }
        case LUA_TSTRING:
        {
            QString value;
            getArgument(luaState_, s_valueIndex, value);
            obj_.insert(key, QJsonValue::fromVariant(QVariant::fromValue(value)));
            break;
        }
        case LUA_TTABLE:
        {
            QJsonObject obj;
            getTable(luaState_, s_valueIndex, obj);
            if(isArray(obj))
            {
                QJsonArray array = convertToArray(obj);
                obj_.insert(key, QJsonValue{array});
            }
            else
            {
                obj_.insert(key, QJsonValue{obj});
            }
            break;
        }
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TTHREAD:
        case LUA_TLIGHTUSERDATA:
        default:
            // invalid value, skip
            break;
        }
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(luaState_, 1);
    }
    // remove key
    lua_pop(luaState_, 1);
}

inline
void getTable(
        lua_State *luaState_,
        int position_,
        QJsonDocument &doc_
        )
{
    QJsonObject obj;
    getTable(luaState_, position_, obj);
    doc_ = QJsonDocument(obj);
}

inline
void pushTable(
        lua_State *luaState_,
        const QJsonObject &obj_
        );
inline
void pushTable(
        lua_State *luaState_,
        const QJsonArray &array_
        );

template<typename KeyType_>
inline
void pushTableValue(
        lua_State *luaState_,
        const QJsonValue &array_,
        const KeyType_ &key_
        );

template<typename KeyType_>
inline
void pushTableValue(
        lua_State *luaState_,
        const QJsonValue &value_,
        const KeyType_ &key_
        )
{
    switch (value_.type())
    {
    case QJsonValue::Bool:
    {
        pushArgument(luaState_, key_ );
        const bool val = value_.toBool();
        pushArgument(luaState_, val );
        lua_settable(luaState_, -3);
        break;
    }
    case QJsonValue::Double:
    {
        pushArgument(luaState_, key_ );
        const double val = value_.toDouble();
        pushArgument(luaState_, val );
        lua_settable(luaState_, -3);
        break;
    }
    case QJsonValue::String:
    {
        pushArgument(luaState_, key_ );
        const QString val = value_.toString();
        pushArgument(luaState_, val );
        lua_settable(luaState_, -3);
        break;
    }
    case QJsonValue::Null:
    case QJsonValue::Undefined:
    {
        pushArgument(luaState_, key_ );
        lua_pushnil(luaState_);
        lua_settable(luaState_, -3);
        break;
    }
    case QJsonValue::Object:
    {
        const QJsonObject obj = value_.toObject();
        pushArgument(luaState_, key_ );
        pushTable(luaState_, obj);
        lua_settable(luaState_, -3);
        break;
    }
    case QJsonValue::Array:
    {
        const QJsonArray array = value_.toArray();
        pushArgument(luaState_, key_ );
        pushTable(luaState_, array);
        lua_settable(luaState_, -3);
        break;
    }
    default:
        break;
    }
}

inline
void pushTable(
        lua_State *luaState_,
        const QJsonArray &array_
        )
{
    lua_newtable(luaState_);
    for(int i = 0; i < array_.size(); i++)
    {
        const QJsonValue value = array_.at(i);
        const int index = i + 1;
        pushTableValue(luaState_, value, index);
    }
}

inline
void pushTable(
        lua_State *luaState_,
        const QJsonObject &obj_
        )
{
    lua_newtable(luaState_);
    QStringList keys = obj_.keys();
    for(const QString &key_ : keys)
    {
        const QJsonValue value = obj_.value(key_);
        pushTableValue(luaState_, value, key_);
    }
}

inline
void pushTable(
        lua_State *luaState_,
        const QJsonDocument &doc_
        )
{
    if(doc_.isObject())
    {
        const QJsonObject obj = doc_.object();
        pushTable(luaState_, obj);
    }
}

}

template<typename ... Args_> inline
bool getArguments(lua_State *luaState_, Args_ &... args_)
{
    constexpr int argumentsCount = sizeof(decltype(*detail::countOf<Args_ ...>::get()));
    const bool success = detail::getArguments(luaState_, argumentsCount, 0, args_ ...);
    if(!success)
    {
        qDebug() << "wrong argument type(s) : ";
        detail::traceArguments(luaState_, argumentsCount, 0, args_ ...);
    }
    return success;
}

template<typename ... Args_> inline
void pushArguments(lua_State *luaState_, const Args_ &... args_)
{
    detail::pushArguments(luaState_, args_ ...);
}

template<typename DataType_, typename DataLayout_> inline
void getStructFromTable(
        lua_State *luaState_,
        int position_,
        DataType_ &data_,
        const DataLayout_ &layout_
        )
{
    detail::getStructFromTable(luaState_, position_, data_, layout_ );
}

inline
void getTable(
        lua_State *luaState_,
        int position_,
        QJsonDocument &doc_
        )
{
    detail::getTable(luaState_, position_, doc_ );
}

inline
void pushTable(
        lua_State *luaState_,
        const QJsonDocument &doc_
        )
{
    detail::pushTable(luaState_, doc_ );
}


#endif // LUA_UTILS_H
