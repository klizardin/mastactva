#ifndef FORMAT_H
#define FORMAT_H


#include <type_traits>
#include <QString>
#include <QVariant>


namespace fmt
{
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

class Format
{
public:
    Format(const QString &format_) : m_format(format_){}

    QString toString() const
    {
        return m_format;
    }

private:
    QString m_format;
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

inline
Private::Format format(const QString &format_)
{
    return Private::Format(format_);
}

}  // namespace fmt


#endif // FORMAT_H
