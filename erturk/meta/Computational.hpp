#ifndef ERTURK_COMPUTATIONAL_H
#define ERTURK_COMPUTATIONAL_H

#include <cstddef>

namespace erturk
{
namespace computational
{

namespace detail
{

template <unsigned int N>
struct factorial_impl
{
    static const constexpr unsigned int value = N * factorial_impl<N - 1>::value;
};

template <>
struct factorial_impl<0>
{  // Base case specialization
    static const constexpr unsigned int value = 1;
};

template <unsigned int N>
struct fibonacci_impl
{
    static const constexpr unsigned int value = fibonacci_impl<N - 1>::value + fibonacci_impl<N - 2>::value;
};


template <unsigned int N>
struct fibonacci_impl
{
    static const constexpr unsigned int value = fibonacci_impl<N - 1>::value + fibonacci_impl<N - 2>::value;
};

template <>
struct fibonacci_impl<0>
{  // Specialization for N = 0
    static const constexpr unsigned int value = 0;
};

template <>
struct fibonacci_impl<1>
{  // Specialization for N = 1
    static const constexpr unsigned int value = 1;
};


template <unsigned int A, unsigned int B>
struct gcd_impl
{
    static const constexpr unsigned int value = gcd_impl<B, A % B>::value;
};

template <unsigned int A>
struct gcd_impl<A, 0>
{
    static const constexpr unsigned int value = A;
};

template <unsigned int A, unsigned int B>
struct lcm_impl
{
    static const constexpr unsigned int value = (A / gcd_impl<A, B>::value) * B;
};


template <int N>
struct int_
{
    static const constexpr int value = N;
};

template <int A, int B>
struct multiplies_impl
{
    typedef int_<A * B> type;
};


template <int N, int I = 2>
struct is_prime_impl
{
    static const constexpr bool value = (I * I > N) ? true : ((N % I) && is_prime_impl<N, I + 1>::value);
};

template <int N>
struct is_prime_impl<N, N>
{
    static const constexpr bool value = true;
};

template <>
struct is_prime_impl<0, 2>
{
    static const constexpr bool value = false;
};

template <>
struct is_prime_impl<1, 2>
{
    static const constexpr bool value = false;
};


template <bool Condition, typename TrueType, typename FalseType>
struct if_impl
{
    typedef TrueType type;
};

template <typename TrueType, typename FalseType>
struct if_impl<false, TrueType, FalseType>
{
    typedef FalseType type;
};


template <typename A, typename B>
struct max_type_impl
{
    typedef typename if_impl<(A::value > B::value), A, B>::type type;
};

template <typename A, typename B>
struct min_type_impl
{
    typedef typename if_impl<(A::value > B::value), B, A>::type type;
};


template <typename... Types>
struct types_sequence_impl
{
};


template <typename TList>
struct length_impl;

template <typename... Types>
struct length_impl<types_sequence_impl<Types...>>
{
    static const constexpr size_t value = sizeof...(Types);
};


template <typename TList, typename T>
struct append_impl;

template <typename... Types, typename T>
struct append_impl<types_sequence_impl<Types...>, T>
{
    typedef types_sequence_impl<Types..., T> type;
};


template <unsigned int base, unsigned int exponent>
struct power_impl
{
    static const constexpr unsigned int value = base * power_impl<base, exponent - 1>::value;
};

template <unsigned int base>
struct power_impl<base, 0>
{
    static const constexpr unsigned int value = 1;
};

}  // namespace detail

// ************************************************************************************************************

template <unsigned int N>
struct factorial : detail::factorial_impl<N>
{
};


template <unsigned int N>
struct fibonacci : detail::fibonacci_impl<N>
{
};


template <unsigned int A, unsigned int B>
struct gcd : detail::gcd_impl<A, B>
{
};

template <unsigned int A, unsigned int B>
struct lcm : detail::lcm_impl<A, B>
{
};

template <int A, int B>
struct multiplies : detail::multiplies_impl<A, B>
{
};


// Might hit the template instantiation depth limit for larger numbers.
template <int N, int I = 2>
struct is_prime : detail::is_prime_impl<N, I>
{
};


template <bool Condition, typename TrueType, typename FalseType>
struct if_ : detail::if_impl<Condition, TrueType, FalseType>
{
};


template <typename A, typename B>
struct max_type : detail::max_type_impl<A, B>
{
};

template <typename A, typename B>
struct min_type : detail::min_type_impl<A, B>
{
};


template <typename... Types>
struct types_sequence : detail::types_sequence_impl<Types...>
{
};


template <typename TList>
struct length : detail::length_impl<TList>
{
};


template <typename TList, typename T>
struct append : detail::append_impl<TList, T>
{
};


template <unsigned int base, unsigned int exponent>
struct power : detail::power_impl<base, exponent>
{
};

}  // namespace computational
}  // namespace erturk

#endif  // ERTURK_COMPUTATIONAL_H