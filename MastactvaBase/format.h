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
QString toString(Type_ val_, std::false_type, std::false_type)
{
    return QVariant::fromValue(val_).toString();
}

template<typename Type_> inline
QString toString(Type_ val_, std::true_type, std::false_type)
{
    return QString(val_);
}

template<typename Type_> inline
QString toString(Type_ val_, std::true_type, std::true_type)
{
    // prefer convertible to string
    return toString(val_, std::true_type(), std::false_type());
}

template<typename Type_> inline
QString toString(Type_ val_, std::false_type, std::true_type)
{
    return val_.toString();
}

}  // namespace Private


template<typename Type_> inline
QString toString(Type_ val_)
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

}  // namespace fmt


#endif // FORMAT_H
