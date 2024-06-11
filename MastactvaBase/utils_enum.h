#ifndef UTILS_ENUM_H
#define UTILS_ENUM_H


#include <type_traits>


/*
 * enum conversions
 * {
*/
template <typename EnumType_> inline
constexpr auto to_underlying(EnumType_ enumValue_) noexcept
{
    return static_cast<std::underlying_type_t<EnumType_>>(enumValue_);
}

template <typename EnumType_> inline
constexpr auto to_enum(const std::underlying_type_t<EnumType_> &val_) noexcept
{
    return static_cast<EnumType_>(val_);
}
// }


#endif // UTILS_ENUM_H
