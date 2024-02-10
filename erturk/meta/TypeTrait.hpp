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


// Signed and unsigned type traits
template <typename T, bool = is_arithmetic_impl<T>::value>
struct is_signed_helper_impl : false_type
{
};

template <typename T>
struct is_signed_helper_impl<T, true> : integral_constant_impl<bool, T(-1) < T(0)>
{
};

template <typename T>
struct is_signed_impl : is_signed_helper_impl<T>
{
};

template <typename T, bool = is_arithmetic_impl<T>::value>
struct is_unsigned_helper_impl : false_type
{
};

template <typename T>
struct is_unsigned_helper_impl<T, true> : integral_constant_impl<bool, T(0) < T(-1)>
{
};

template <typename T>
struct is_unsigned_impl : is_unsigned_helper_impl<T>
{
};


template <typename T, typename U>
struct is_same_impl : false_type
{
};

template <typename T>
struct is_same_impl<T, T> : true_type
{
};



template <typename T>
struct is_array_impl : false_type
{
};

template <typename T>
struct is_array_impl<T[]> : true_type
{
};

template <typename T, size_t N>
struct is_array_impl<T[N]> : true_type
{
};

template <typename T>
struct is_function_impl : false_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<Ret(Args...)> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<const Ret(Args...)> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<volatile Ret(Args...)> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<const volatile Ret(Args...)> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<Ret(Args...)&> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<const Ret(Args...)&> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<volatile Ret(Args...)&> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<const volatile Ret(Args...)&> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<Ret(Args...) &&> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<const Ret(Args...) &&> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<volatile Ret(Args...) &&> : true_type
{
};

template <typename Ret, typename... Args>
struct is_function_impl<const volatile Ret(Args...) &&> : true_type
{
};


template <typename T>
struct is_pointer_impl : false_type
{
};

template <typename T>
struct is_pointer_impl<T*> : true_type
{
};

template <typename T>
struct is_pointer_impl<const T*> : true_type
{
};


template <typename T>
struct is_lvalue_reference_impl : false_type
{
};

template <typename T>
struct is_lvalue_reference_impl<T&> : true_type
{
};

template <typename T>
struct is_rvalue_reference_impl : false_type
{
};

template <typename T>
struct is_rvalue_reference_impl<T&&> : true_type
{
};


template <typename T>
struct is_member_function_pointer_impl : false_type
{
};

template <typename T, typename U>
struct is_member_function_pointer_impl<T U::*> : integral_constant_impl<bool, is_function_impl<T>::value>
{
};


template <typename T>
struct is_const_impl : false_type
{
};

template <typename T>
struct is_const_impl<const T> : true_type
{
};

template <typename T>
struct is_volatile_impl : false_type
{
};

template <typename T>
struct is_volatile_impl<volatile T> : true_type
{
};


}  // namespace detail

// ************************************************************************************************************************************

namespace meta
{

template <typename T>
struct is_arithmetic : detail::is_arithmetic_impl<T>
{
};


template <typename T>
struct is_signed : detail::is_signed_impl<T>
{
};

template <typename T>
struct is_unsigned : detail::is_unsigned_impl<T>
{
};


template <typename T, typename U>
struct is_same : detail::is_same_impl<T, U>
{
};


template <typename T>
struct is_array : detail::is_array_impl<T>
{
};

template <typename T>
struct is_function : detail::is_function_impl<T>
{
};

template <typename T>
struct is_pointer : detail::is_pointer_impl<T>
{
};


template <typename T>
struct is_lvalue_reference : detail::is_lvalue_reference_impl<T>
{
};

template <typename T>
struct is_rvalue_reference : detail::is_rvalue_reference_impl<T>
{
};


template <typename T>
struct is_member_function_pointer : detail::is_member_function_pointer_impl<T>
{
};

template <typename T>
struct is_const : detail::is_const_impl<T>
{
};


template <typename T>
struct is_volatile : detail::is_volatile_impl<T>
{
};



}  // namespace meta

}  // namespace erturk

#endif  // ERTURK_META_H
