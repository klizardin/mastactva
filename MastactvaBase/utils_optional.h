#ifndef UTILS_OPTIONAL_H
#define UTILS_OPTIONAL_H


#include <memory>


/*
 * primitives of imitation of the intialized/uninitialized value type
 * with std::pair or with std::tuple
 * {
*/
template <typename Type_> inline
constexpr bool has_value(const std::pair<Type_, bool> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr bool has_value(const std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_>
constexpr bool & has_value(std::pair<Type_, bool> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr bool & has_value(std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_> inline
constexpr const Type_ & value(const std::pair<Type_, bool> &data_) noexcept
{
    return data_.first;
}

template <typename Type_>
constexpr const Type_ & value(const std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_> inline
constexpr Type_ & value(std::pair<Type_, bool> &data_) noexcept
{
    return data_.first;
}

template <typename Type_> inline
constexpr Type_ & value(std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_>
constexpr bool has_value(const std::pair<bool, Type_> &data_) noexcept
{
    return data_.first;
}

template <typename Type_> inline
constexpr bool has_value(const std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_> inline
constexpr bool & has_value(std::pair<bool, Type_> &data_) noexcept
{
    return data_.first;
}

template <typename Type_> inline
constexpr bool & has_value(std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_> inline
constexpr const Type_ & value(const std::pair<bool, Type_> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr const Type_ & value(const std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_> inline
constexpr Type_ & value(std::pair<bool, Type_> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr Type_ & value(std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<1>(data_);
}
// }

#endif // UTILS_OPTIONAL_H
