#ifndef FORMAT_H
#define FORMAT_H


#include <type_traits>
#include <QString>
#include <QVariant>


namespace fmt
{

template<typename Type_> inline
QString toString(const Type_ &val_);

template<typename DestType_, typename SrcType_> inline
DestType_ toType(const SrcType_ &val_, const DestType_ &defaultValue_);

template<typename DestType_, typename SrcType_>
QList<DestType_> toType(const QList<SrcType_> &vals_, const DestType_ &defaultValue_)
{
    QList<DestType_> result;
    result.reserve(vals_.size());
    for(const SrcType_ &valItem_ : vals_)
    {
        result.push_back(toType(valItem_, defaultValue_));
    }
    return result;
}

template<typename DestType_, typename SrcType_>
QVector<DestType_> toType(const QVector<SrcType_> &vals_, const DestType_ &defaultValue_)
{
    QVector<DestType_> result;
    result.reserve(vals_.size());
    for(const SrcType_ &valItem_ : vals_)
    {
        result.push_back(toType(valItem_, defaultValue_));
    }
    return result;
}


namespace Private
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

template<typename...>
class FormatArgs
{
};

template<>
class FormatArgs<>
{
public:
    FormatArgs() = default;

    template<typename ... StrArgs_>
    QString toString(const QString &fmt_, StrArgs_ ...) const
    {
        return fmt_;
    }

    template<typename SetType_>
    void set(const SetType_ &)
    {
    }
};

template<typename Arg_>
class FormatArgs<Arg_>
{
public:
    FormatArgs(Arg_ arg_) : m_arg(arg_) {}

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

private:
    Arg_ m_arg;
};

template<typename Arg_, typename ... Args_>
class FormatArgs<Arg_, Args_ ...> : public FormatArgs<Args_ ...>
{
public:
    FormatArgs(Arg_ arg_, Args_ ... args_)
        : FormatArgs<Args_ ...>(args_ ...), m_arg(arg_)
    {
    }

    template<typename ... StrArgs_>
    QString toString(const QString &fmt_, StrArgs_ ... strArgs_) const
    {
        return FormatArgs<Args_ ...>::toString(fmt_, strArgs_ ..., fmt::toString(m_arg));
    }

    template<typename SetType_>
    void set(const SetType_ &val_)
    {
        m_arg = fmt::toType(val_, m_arg);
        FormatArgs<Args_ ...>::set(val_);
    }

private:
    Arg_ m_arg;
};


template<typename ... Args_>
class Format
{
public:
    Format(const QString &format_, Args_ ... args_)
        : m_format(format_),
          m_args(args_ ...)
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

private:
    QString m_format;
    FormatArgs<Args_ ...> m_args;
};

template<typename Type_>
class List
{
public:
    List(const QVector<Type_> &data_, const QString &separator_)
        : m_data(data_), m_seporator(separator_)
    {
    }

    QString toString() const
    {
        QStringList strs;
        strs.reserve(m_data.size());
        for(const Type_ &val_ : m_data)
        {
            strs.push_back(fmt::toString(val_));
        }
        return strs.join(m_seporator);
    }

private:
    QVector<Type_> m_data;
    QString m_seporator;
};

template<typename Type_, typename ... Args_>
class FormatList
{
public:
    FormatList(
            Format<Args_...> format_,
            const QVector<Type_> &data_,
            const QString &separator_
            )
        : m_format(format_),
          m_data(data_),
          m_seporator(separator_)
    {
    }

    QString toString() const
    {
        QStringList strs;
        strs.reserve(m_data.size());
        for(const Type_ &val_ : m_data)
        {
            Private::Format<Args_...> format = m_format;
            format.set(val_);
            strs.push_back(fmt::toString(format));
        }
        return strs.join(m_seporator);
    }

private:
    Private::Format<Args_...> m_format;
    QVector<Type_> m_data;
    QString m_seporator;
};

}  // namespace Private


template<typename Type_> inline
QString toString(const Type_ &val_)
{
    return Private::toString(
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

template<typename ... Args_>
inline
Private::Format<Args_...> format(const QString &format_, Args_ ... args_)
{
    return Private::Format<Args_ ...>(format_, args_ ...);
}

template<typename ListType_>
inline
Private::List<ListType_> list(
        const QVector<ListType_> &data_,
        const QString &separator_
        )
{
    return Private::List<ListType_>(data_, separator_);
}

template<typename ListType_, typename ... Args_>
inline
Private::FormatList<ListType_, Args_ ...> list(
        Private::Format<Args_...> format_,
        const QVector<ListType_> &data_,
        const QString &separator_
        )
{
    return Private::FormatList<ListType_, Args_ ...>(format_, data_, separator_);
}

}  // namespace fmt


#endif // FORMAT_H
