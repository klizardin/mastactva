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

#ifndef FORMAT_H
#define FORMAT_H


#include <type_traits>
#include <QString>
#include <QVariant>
#include "../MastactvaBase/format_type.h"


namespace fmt
{

template<typename DestType_, template<typename> class ContainerType_, typename SrcType_> inline
ContainerType_<DestType_> toType(const ContainerType_<SrcType_> &vals_, const DestType_ &defaultValue_)
{
    ContainerType_<DestType_> result;
    result.reserve(vals_.size());
    for(const SrcType_ &valItem_ : vals_)
    {
        result.push_back(toType(valItem_, defaultValue_));
    }
    return result;
}

template<typename DestType_> inline
QList<DestType_> toType(const QStringList &vals_, const DestType_ &defaultValue_)
{
    QList<DestType_> result;
    result.reserve(vals_.size());
    for(const QString &valItem_ : vals_)
    {
        result.push_back(toType(valItem_, defaultValue_));
    }
    return result;
}

template<typename DestType_, template<typename> class ContainerType_, typename SrcType_> inline
ContainerType_<DestType_> toTypeList(const DestType_ &defaultValue_, const ContainerType_<SrcType_> &vals_)
{
    return toType(vals_, defaultValue_);
}

template<typename DestType_> inline
QList<DestType_> toTypeList(const DestType_ &defaultValue_, const QStringList &vals_)
{
    return toType(vals_, defaultValue_);
}

template<typename DestType_, typename SrcType_> inline
DestType_ toTypeValue(const DestType_ &defaultValue_, const SrcType_ &val_)
{
    return toType(val_, defaultValue_);
}



namespace details
{

template<typename Type_> inline
QString intergralTypes(const Type_ &val_)
{
    return QVariant::fromValue(val_).toString();
}

template<typename Type_> inline
QString convertableToStringTypes(const Type_ &val_)
{
    return QString(val_);
}

template<class Type_> inline
QString compoundTypes(const Type_ &val_)
{
    return val_.toString();
}

template<typename Type_> inline
QString toString(const Type_ &val_, std::false_type, std::false_type)
{
    return intergralTypes(val_);
}

template<typename Type_> inline
QString toString(const Type_ &val_, std::true_type, std::false_type)
{
    return convertableToStringTypes(val_);
}

template<typename Type_> inline
QString toString(const Type_ &val_, std::true_type, std::true_type)
{
    // prefer convertable to QString from this two alternatives
    // to not have to define both
    //          operator QString() const
    // and
    //          toString() const
    // method, just one is enough
    return convertableToStringTypes(val_);
}

template<typename Type_> inline
QString toString(const Type_ &val_, std::false_type, std::true_type)
{
    return compoundTypes(val_);
}

template<typename Type_>
class Constant
{
public:
    Type_ value;
};


template<typename...>
class ArgsList
{
};

template<>
class ArgsList<>
{
public:
    ArgsList() = default;

    template<typename ... StrArgs_>
    QString toString(const QString &fmt_, StrArgs_ ...) const
    {
        return fmt_;
    }

    template<typename SetType_>
    void set(const SetType_ &)
    {
    }

    QString sum() const
    {
        return QString();
    }
};

template<typename Arg_>
class ArgsList<Arg_>
{
public:
    ArgsList(Arg_ &&arg_) : m_arg(std::forward<Arg_>(arg_)) {}

    template<typename ... StrArgs_>
    QString toString(const QString &fmt_, StrArgs_ ... strArgs_) const
    {
        return fmt_.arg(strArgs_ ..., fmt::toString(m_arg));
    }

    template<typename SetType_>
    void set(const SetType_ &val_)
    {
        m_arg = fmt::toType(val_, m_arg);
    }

    QString sum() const
    {
        return fmt::toString(m_arg);
    }

private:
    Arg_ m_arg;
};

template<typename Arg_>
class ArgsList<Constant<Arg_>>
{
public:
    ArgsList(Constant<Arg_>  &&arg_) : m_arg(std::forward<Constant<Arg_>>(arg_)) {}

    template<typename ... StrArgs_>
    QString toString(const QString &fmt_, StrArgs_ ... strArgs_) const
    {
        return fmt_.arg(strArgs_ ..., fmt::toString(m_arg.value));
    }

    template<typename SetType_>
    void set(const SetType_ &)
    {
    }

    QString sum() const
    {
        return fmt::toString(m_arg.value);
    }

private:
    Constant<Arg_> m_arg;
};

template<typename Arg_, typename ... Args_>
class ArgsList<Arg_, Args_ ...> : public ArgsList<Args_ ...>
{
public:
    ArgsList(Arg_ &&arg_, Args_ &&... args_)
        : ArgsList<Args_ ...>(std::forward<Args_>(args_) ...), m_arg(std::forward<Arg_>(arg_))
    {
    }

    template<typename ... StrArgs_>
    QString toString(const QString &fmt_, StrArgs_ ... strArgs_) const
    {
        return ArgsList<Args_ ...>::toString(fmt_, strArgs_ ..., fmt::toString(m_arg));
    }

    template<typename SetType_>
    void set(const SetType_ &val_)
    {
        m_arg = fmt::toType(val_, m_arg);
        ArgsList<Args_ ...>::set(val_);
    }

    QString sum() const
    {
        return fmt::toString(m_arg) + ArgsList<Args_ ...>::sum();
    }

private:
    Arg_ m_arg;
};

template<typename Arg_, typename ... Args_>
class ArgsList<Constant<Arg_>, Args_ ...> : public ArgsList<Args_ ...>
{
public:
    ArgsList(Constant<Arg_> &&arg_, Args_ &&... args_)
        : ArgsList<Args_ ...>(std::forward<Args_>(args_) ...), m_arg(std::forward<Constant<Arg_>>(arg_))
    {
    }

    template<typename ... StrArgs_>
    QString toString(const QString &fmt_, StrArgs_ ... strArgs_) const
    {
        return ArgsList<Args_ ...>::toString(fmt_, strArgs_ ..., fmt::toString(m_arg.value));
    }

    template<typename SetType_>
    void set(const SetType_ &val_)
    {
        ArgsList<Args_ ...>::set(val_);
    }

    QString sum() const
    {
        return fmt::toString(m_arg.value) + ArgsList<Args_ ...>::sum();
    }

private:
    Constant<Arg_> m_arg;
};


template<typename ... Args_>
class Format
{
public:
    Format(const QString &format_, Args_ &&... args_)
        : m_format(format_),
          m_args(std::forward<Args_>(args_) ...)
    {}

    QString toString() const
    {
        return m_args.toString(m_format);
    }

    template<typename SetType_>
    void set(const SetType_ &val_)
    {
        m_args.set(val_);
    }

    operator QString () const
    {
        return toString();
    }

private:
    QString m_format;
    ArgsList<Args_ ...> m_args;
};


template<typename DataType_>
class RefOfValue
{
private:
    using DataType = typename std::remove_cv<typename std::remove_reference<DataType_>::type>::type;

public:
    RefOfValue(const DataType &refValue_)
        : contains(false)
        , dataRef(std::cref(refValue_))
    {
    }

    RefOfValue(DataType &&value_)
        : contains(true)
        , dataValue(std::move(value_))
        , dataRef(std::cref(dataValue))
    {
    }

    RefOfValue(RefOfValue &&value_)
        : dataRef(std::cref(dataValue))
    {
        operator=(std::move(value_));
    }

    RefOfValue & operator = (RefOfValue &&value_)
    {
        if(value_.contains)
        {
            dataValue = std::move(value_.dataValue);
            dataRef = std::cref(dataValue);
            contains = true;
        }
        else
        {
            dataValue = DataType{};
            dataRef = std::move(value_.dataRef);
            contains = false;
        }
        return *this;
    }

    const DataType &get() const
    {
        return dataRef.get();
    }

    RefOfValue(const RefOfValue &) = delete;
    RefOfValue & operator = (const RefOfValue &) = delete;

private:
    bool contains = false;
    DataType dataValue;
    std::reference_wrapper<const DataType> dataRef;
};


template<typename DataType_>
class List
{
private:
    using DataType = typename std::remove_cv<typename std::remove_reference<DataType_>::type>::type;

public:
    List(DataType_ &&data_, const QString &separator_)
        : m_data(std::forward<DataType_>(data_)), m_seporator(separator_)
    {
    }

    QStringList toStringList() const
    {
        using Type = decltype(*std::begin(DataType{}));

        QStringList strs;
        strs.reserve(m_data.get().size());
        for(const Type &val_ : m_data.get())
        {
            strs.push_back(fmt::toString(val_));
        }
        return strs;
    }

    QString toString() const
    {
        return toStringList().join(m_seporator);
    }

    operator QString () const
    {
        return toString();
    }

    operator QStringList () const
    {
        return toStringList();
    }

private:
    RefOfValue<DataType_> m_data;
    QString m_seporator;
};

template<typename DataType_, typename FormatType_>
class FormatList
{
private:
    using DataType = typename std::remove_cv<typename std::remove_reference<DataType_>::type>::type;

public:
    FormatList(
            FormatType_ &&format_,
            DataType_ &&data_,
            const QString &separator_
            )
        : m_format(std::forward<FormatType_>(format_)),
          m_data(std::forward<DataType_>(data_)),
          m_seporator(separator_)
    {
    }

    QStringList toStringList() const
    {
        using Type = decltype(*std::begin(DataType{}));

        QStringList strs;
        strs.reserve(m_data.get().size());
        for(const Type &val_ : m_data.get())
        {
            FormatType_ format = m_format;
            format.set(val_);
            strs.push_back(fmt::toString(format));
        }
        return strs;
    }

    QString toString() const
    {
        return toStringList().join(m_seporator);
    }

    operator QString () const
    {
        return toString();
    }

    operator QStringList () const
    {
        return toStringList();
    }

private:
    FormatType_ m_format;
    RefOfValue<DataType_> m_data;
    QString m_seporator;
};

template<typename ... Args_>
class Sum
{
public:
    Sum(Args_ &&... args_)
        : m_args(std::forward<Args_>(args_) ...)
    {
    }

    QString toString() const
    {
        return m_args.sum();
    }

    operator QString () const
    {
        return toString();
    }

private:
    details::ArgsList<Args_...> m_args;
};

template<typename OkType_, typename FailType_>
class Choose
{
public:
    Choose(bool condition_, OkType_ &&ok_, FailType_ &&fail_)
        : m_condition(condition_),
          m_ok(std::forward<OkType_>(ok_)),
          m_fail(std::forward<FailType_>(fail_))
    {}

    QString toString() const
    {
        if(m_condition)
        {
            return fmt::toString(m_ok);
        }
        else
        {
            return fmt::toString(m_fail);
        }
    }

    operator QString () const
    {
        return toString();
    }

private:
    bool m_condition;
    OkType_ m_ok;
    FailType_ m_fail;
};

using Null = Sum<>;

}  // namespace Private


template<typename Type_> inline
QString toString(const Type_ &val_)
{
    return details::toString(
                val_,
                typename std::integral_constant<
                    bool,
                    std::is_convertible<Type_, QString>::value
                >::type(),
                typename std::integral_constant<
                    bool,
                    std::is_compound<Type_>::value
                >::type()
                );
}

template<typename DestType_, typename SrcType_> inline
DestType_ toType(const SrcType_ &val_, const DestType_ &)
{
    return DestType_(val_);
}

template<typename Type_> inline
Type_ toType(const Type_ &val_, const Type_ &)
{
    return val_;
}

template<typename ... Args_>
inline
details::Format<Args_...> format(const QString &format_, Args_ &&... args_)
{
    return details::Format<Args_ ...>{format_, std::forward<Args_>(args_) ...};
}

template<typename ListType_>
inline
details::List<ListType_> list(
        ListType_ &&data_,
        const QString &separator_
        )
{
    return details::List<ListType_>{
        std::forward<ListType_>(data_),
        separator_
    };
}

template<typename ListType_, typename FormatType_>
inline
details::FormatList<ListType_, FormatType_> list(
        FormatType_ &&format_,
        ListType_ &&data_,
        const QString &separator_
        )
{
    return details::FormatList<ListType_, FormatType_>{
        std::forward<FormatType_>(format_),
        std::forward<ListType_>(data_),
        separator_
    };
}

template<typename ItemType_, template<typename> class Container_> inline
Container_<ItemType_> merge(const Container_<ItemType_> &c_)
{
    return c_;
}

template<typename ItemType_, template<typename> class Container_> inline
Container_<ItemType_> merge(Container_<ItemType_> &&c_)
{
    return std::move(c_);
}

template<typename ItemType_, template<typename> class Container_, typename ... ItemTypes_> inline
Container_<ItemType_> merge(const Container_<ItemType_> &c_, ItemTypes_ &&... nexts_)
{
    Container_<ItemType_> result{c_};
    result.append(merge(std::forward<ItemTypes_>(nexts_) ...));
    return result;
}

template<typename ItemType_, template<typename> class Container_, typename ... ItemTypes_> inline
Container_<ItemType_> merge(Container_<ItemType_> &&c_, ItemTypes_ &&... nexts_)
{
    Container_<ItemType_> result{std::move(c_)};
    result.append(merge(std::forward<ItemTypes_>(nexts_) ...));
    return result;
}

inline
QStringList merge(const QStringList &c_)
{
    return c_;
}

inline
QStringList merge(QStringList &&c_)
{
    return std::move(c_);
}

template<typename ... ItemTypes_> inline
QStringList merge(const QStringList &c_, ItemTypes_ &&... nexts_)
{
    QStringList result{c_};
    result.append(merge(std::forward<ItemTypes_>(nexts_) ...));
    return result;
}

template<typename ... ItemTypes_> inline
QStringList merge(QStringList &&c_, ItemTypes_ &&... nexts_)
{
    QStringList result{std::move(c_)};
    result.append(merge(std::forward<ItemTypes_>(nexts_) ...));
    return result;
}

template<typename Type_>
details::Constant<Type_> constant(Type_ &&value_)
{
    return details::Constant<Type_>{std::forward<Type_>(value_)};
}

template<typename ... Args_> inline
details::Sum<Args_ ...> sum(Args_ &&... args_)
{
    return details::Sum<Args_ ...>{std::forward<Args_>(args_) ...};
}

template<typename OkType_, typename FailType_> inline
details::Choose<OkType_, FailType_> choose(bool condition_, OkType_ &&ok_, FailType_ &&fail_)
{
    return details::Choose<OkType_, FailType_>(
                condition_,
                std::forward<OkType_>(ok_),
                std::forward<FailType_>(fail_)
                );
}

inline
details::Null null()
{
    return details::Null{};
}

}  // namespace fmt


#endif // FORMAT_H
