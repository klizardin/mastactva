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
#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>


class LuaAPIUtils
{
public:
    static QString type2String(int type_);
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
        return m_field.getName();
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
    lua_getfield(luaState_, position_, name);
    detail::getArgument(luaState_, 0, layoutArg_.getDataRef(data_));
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
        const DataLayout<LayoutArgs_ ...> &layout_
        )
{
    detail::getStructItemFromTable(luaState_, position_, data_, layout_.value);
    detail::getStructFromTable(luaState_, position_, data_,
                               static_cast<const typename DataLayout<LayoutArgs_ ...>::NextLayout &>(layout_)
                               );
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


#endif // LUA_UTILS_H
