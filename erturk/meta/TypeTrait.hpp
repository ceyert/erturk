#ifndef ERTURK_META_H
#define ERTURK_META_H

#include <cstddef>

namespace erturk
{
namespace detail
{
template <typename T, T rvalue>
struct integral_constant_impl
{
    static constexpr const T value = rvalue;
    using value_type = T;
    using type = integral_constant_impl;
    constexpr explicit operator value_type() const noexcept
    {
        return value;
    }
    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

using true_type = integral_constant_impl<bool, true>;
using false_type = integral_constant_impl<bool, false>;


// Arithmetic type traits definitions
template <typename T>
struct is_arithmetic_impl : false_type
{
};

#define MAKE_ARITHMETIC_TYPE(T)              \
    template <>                              \
    struct is_arithmetic_impl<T> : true_type \
    {                                        \
    };

MAKE_ARITHMETIC_TYPE(bool)
MAKE_ARITHMETIC_TYPE(char)
MAKE_ARITHMETIC_TYPE(signed char)
MAKE_ARITHMETIC_TYPE(unsigned char)
MAKE_ARITHMETIC_TYPE(wchar_t)
MAKE_ARITHMETIC_TYPE(char16_t)
MAKE_ARITHMETIC_TYPE(char32_t)
MAKE_ARITHMETIC_TYPE(short)
MAKE_ARITHMETIC_TYPE(unsigned short)
MAKE_ARITHMETIC_TYPE(int)
MAKE_ARITHMETIC_TYPE(unsigned int)
MAKE_ARITHMETIC_TYPE(long)
MAKE_ARITHMETIC_TYPE(unsigned long)
MAKE_ARITHMETIC_TYPE(long long)
MAKE_ARITHMETIC_TYPE(unsigned long long)
MAKE_ARITHMETIC_TYPE(float)
MAKE_ARITHMETIC_TYPE(double)
MAKE_ARITHMETIC_TYPE(long double)
#undef MAKE_ARITHMETIC_TYPE

#ifdef __SIZEOF_INT128__
template <>
struct is_arithmetic_impl<__int128> : true_type
{
};
template <>
struct is_arithmetic_impl<unsigned __int128> : true_type
{
};
#endif

}  // namespace detail

// ************************************************************************************************************************************

namespace meta
{

template <typename T>
struct is_arithmetic : detail::is_arithmetic_impl<T>
{
};

}  // namespace meta

}  // namespace erturk

#endif  // ERTURK_META_H
