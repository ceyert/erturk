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

#ifdef __SIZEOF_INT128__
MAKE_ARITHMETIC_TYPE(__int128)
MAKE_ARITHMETIC_TYPE(unsigned __int128)
#endif
#undef MAKE_ARITHMETIC_TYPE

template <bool B, typename T, typename F>
struct conditional_impl
{
};

template <typename T, typename F>
struct conditional_impl<true, T, F>
{
    typedef T type;
};

template <typename T, typename F>
struct conditional_impl<false, T, F>
{
    typedef F type;
};

template <typename T, typename U>
struct is_same_impl : false_type
{
};

template <typename T>
struct is_same_impl<T, T> : true_type
{
};

template <bool Condition, typename T>
struct enableif_impl
{
};

template <typename T>
struct enableif_impl<true, T>
{
    typedef T type;
};

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

template <typename T>
struct is_reference_impl : false_type
{
};

template <typename T>
struct is_reference_impl<T&> : true_type
{
};

template <typename T>
struct is_reference_impl<T&&> : true_type
{
};

template <typename T>
struct add_rvalue_reference_impl
{
    using type = T&&;
};

template <typename T>
typename add_rvalue_reference_impl<T>::type declval() noexcept;

template <typename Void, typename Callable, typename... Args>
struct invoke_result_impl
{
};

template <typename Callable, typename... Args>
struct invoke_result_impl<decltype(void(declval<Callable>()(declval<Args>()...))), Callable, Args...>
{
    using type = decltype(declval<Callable>()(declval<Args>()...));
};

template <size_t Index, typename... Types>
struct get_type_impl
{
};

template <size_t Index, typename First, typename... Rest>
struct get_type_impl<Index, First, Rest...>
{
    using type = typename get_type_impl<Index - 1, Rest...>::type;
};

template <typename First, typename... Rest>
struct get_type_impl<0, First, Rest...>
{
    using type = First;
};

// ****************************************************************
// Use GCC built-in type traits : https://gcc.gnu.org/onlinedocs/gcc-12.3.0/libstdc++/api/a01587.html

template <typename T>
struct has_nothrow_assign_impl : integral_constant_impl<bool, __has_nothrow_assign(T)>
{
};

template <typename T>
struct has_nothrow_copy_impl : integral_constant_impl<bool, __has_nothrow_copy(T)>
{
};

template <typename T>
struct has_nothrow_constructor_impl : integral_constant_impl<bool, __has_nothrow_constructor(T)>
{
};

template <typename T>
struct has_trivial_assign_impl : integral_constant_impl<bool, __has_trivial_assign(T)>
{
};

template <typename T>
struct has_trivial_copy_impl : integral_constant_impl<bool, __has_trivial_copy(T)>
{
};

template <typename T>
struct has_trivial_constructor_impl : integral_constant_impl<bool, __has_trivial_constructor(T)>
{
};

template <typename T>
struct has_trivial_destructor_impl : integral_constant_impl<bool, __has_trivial_destructor(T)>
{
};

template <typename T>
struct is_trivial_impl : integral_constant_impl<bool, __is_trivial(T)>
{
};

template <typename T>
struct is_trivially_copyable_impl : integral_constant_impl<bool, __is_trivially_copyable(T)>
{
};

template <typename T>
struct is_abstract_impl : integral_constant_impl<bool, __is_abstract(T)>
{
};

template <typename T>
struct is_class_impl : integral_constant_impl<bool, __is_class(T)>
{
};

template <typename T>
struct is_enum_impl : integral_constant_impl<bool, __is_enum(T)>
{
};

template <typename T>
struct is_union_impl : integral_constant_impl<bool, __is_union(T)>
{
};

template <typename T>
struct is_literal_type_impl : integral_constant_impl<bool, __is_literal_type(T)>
{
};

template <typename T>
struct is_pod_impl : integral_constant_impl<bool, __is_pod(T)>
{
};

template <typename T>
struct is_polymorphic_impl : integral_constant_impl<bool, __is_polymorphic(T)>
{
};

template <typename T>
struct is_standard_layout_impl : integral_constant_impl<bool, __is_standard_layout(T)>
{
};

template <typename T>
struct is_trivially_constructible_impl : std::integral_constant<bool, __is_trivially_constructible(T)>
{
};
template <typename T>
struct is_copy_constructible_impl : integral_constant_impl<bool, __is_constructible(T, const T&)>
{
};

template <typename T>
struct is_move_constructible_impl : integral_constant_impl<bool, __is_constructible(T, T&&)>
{
};

template <typename T>
struct is_trivially_assignable_impl : std::integral_constant<bool, __is_trivially_assignable(T, T)>
{
};

template <typename T>
struct is_trivially_copy_assignable_impl : std::integral_constant<bool, __is_trivially_assignable(T, const T&)>
{
};

template <typename T>
struct is_trivially_copy_constructible_impl : std::integral_constant<bool, __is_trivially_constructible(T, const T&)>
{
};

template <typename T>
struct is_trivially_default_constructible_impl : std::integral_constant<bool, __is_trivially_constructible(T)>
{
};

}  // namespace detail

// ************************************************************************************************************************************

namespace meta
{

template <bool B, typename T, typename F>
struct conditional : detail::conditional_impl<B, T, F>
{
};

template <bool B, typename T>
struct enableif : detail::enableif_impl<B, T>
{
};

template <typename T>
struct add_rvalue_reference : detail::add_rvalue_reference_impl<T>
{
};

template <typename T>
typename add_rvalue_reference<T>::type declval() noexcept;

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

template <typename T>
struct is_reference : detail::is_reference_impl<T>
{
};

template <typename Callable, typename... Args>
struct invoke_result : detail::invoke_result_impl<void, Callable, Args...>
{
};

template <typename Callable, typename... Args>
using invoke_result_t = typename invoke_result<Callable, Args...>::type;

template <size_t Index, typename... Types>
struct get_type : detail::get_type_impl<Index, Types...>
{
};

template <size_t Index, typename... Types>
using get_type_t = typename get_type<Index, Types...>::type;

template <typename T>
struct is_abstract : detail::is_abstract_impl<T>
{
};

template <typename T>
struct is_class : detail::is_class_impl<T>
{
};

template <typename T>
struct is_enum : detail::is_enum_impl<T>
{
};

template <typename T>
struct is_union : detail::is_union_impl<T>
{
};

template <typename T>
struct is_literal_type : detail::is_literal_type_impl<T>
{
};

template <typename T>
struct is_pod : detail::is_pod_impl<T>
{
};

template <typename T>
struct is_polymorphic : detail::is_polymorphic_impl<T>
{
};

template <typename T>
struct is_standard_layout : detail::is_standard_layout_impl<T>
{
};

template <typename T>
struct is_copy_constructible : detail::is_copy_constructible_impl<T>
{
};

template <typename T>
struct is_move_constructible : detail::is_move_constructible_impl<T>
{
};

template <typename T>
struct has_nothrow_assign : detail::has_nothrow_assign_impl<T>
{
};

template <typename T>
struct has_nothrow_copy : detail::has_nothrow_copy_impl<T>
{
};

template <typename T>
struct has_nothrow_constructor : detail::has_nothrow_constructor_impl<T>
{
};

template <typename T>
struct has_trivial_assign : detail::has_trivial_assign_impl<T>
{
};

template <typename T>
struct has_trivial_copy : detail::has_trivial_copy_impl<T>
{
};

template <typename T>
struct has_trivial_constructor : detail::has_trivial_constructor_impl<T>
{
};

template <typename T>
struct has_trivial_destructor : detail::has_trivial_destructor_impl<T>
{
};

template <typename T>
struct is_trivial : detail::is_trivial_impl<T>
{
};

template <typename T>
struct is_trivially_copyable : detail::is_trivially_copyable_impl<T>
{
};

template <typename T>
struct is_trivially_constructible : detail::is_trivially_constructible_impl<T>
{
};

template <typename T>
struct is_trivially_assignable : detail::is_trivially_assignable_impl<T>
{
};

template <typename T>
struct is_trivially_copy_assignable : detail::is_trivially_copy_assignable_impl<T>
{
};

template <typename T>
struct is_trivially_copy_constructible : detail::is_trivially_copy_constructible_impl<T>
{
};

template <typename T>
struct is_trivially_default_constructible : detail::is_trivially_default_constructible_impl<T>
{
};

}  // namespace meta

}  // namespace erturk

#endif  // ERTURK_META_H
