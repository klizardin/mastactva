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
QString toString(Type_ val_, std::false_type)
{
    return QVariant::fromValue(val_).toString();
}

template<typename Type_> inline
QString toString(Type_ val_, std::true_type)
{
    return QString(val_);
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
                >::type()
                );
}

}  // namespace fmt


#endif // FORMAT_H
