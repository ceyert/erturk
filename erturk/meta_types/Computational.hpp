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
    typedef typename if_impl<(A{} > B{}), A, B>::type type;
};

template <typename A, typename B>
struct min_type_impl
{
    typedef typename if_impl<(A{} > B{}), B, A>::type type;
};

template <typename... Types>
struct types_sequence_impl
{
};

template <typename... Types>
struct length_impl
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

template <unsigned int N>
struct is_power_of_two_impl
{
    static const constexpr bool value = N && !(N & (N - 1));
};

template <int N, int... List>
struct contains_impl;

template <int N, int First, int... Rest>
struct contains_impl<N, First, Rest...>
{
    static const constexpr bool value = (N == First) || contains_impl<N, Rest...>::value;
};

template <int N>
struct contains_impl<N>
{
    static const constexpr bool value = false; // end of list reached, return false
};

template <int N>
struct is_even_impl
{
    static const constexpr bool value = N % 2 == 0;
};

template <int N>
struct is_odd_impl
{
    static const constexpr bool value = !is_even_impl<N>::value;
};

template <unsigned long N, unsigned long left, unsigned long right>
struct sqrt_impl
{
    static const constexpr unsigned long mid = (left + right + 1) / 2;

    static const constexpr unsigned long value =
        (mid * mid > N) ? sqrt_impl<N, left, mid - 1>::value : sqrt_impl<N, mid, right>::value;
};

template <unsigned long N, unsigned long M>
struct sqrt_impl<N, M, M>
{
    static const constexpr unsigned long value = M;
};

template <int... Ns>
struct sum_impl;

template <int First, int... Rest>
struct sum_impl<First, Rest...>
{
    static const constexpr long double value = First + sum_impl<Rest...>::value;
};

template <>
struct sum_impl<>
{
    static const constexpr int value = 0; // end of list reached, add 0
};

template <int N, template <int> class F>
struct static_for_impl
{
    static void execute()
    {
        F<N>::execute();
        static_for_impl<N - 1, F>::execute();
    }
};

template <template <int> class F>
struct static_for_impl<0, F>
{
    static void execute()
    {
        F<0>::execute();
    }
};

template <int... Ns>
struct seq_
{
};

template <typename Seq, int N>
struct prepend_;

template <int... Ns, int Last>
struct prepend_<seq_<Ns...>, Last>
{
    using type = seq_<Last, Ns...>;
};

template <int N, int... Ns>
struct reverse_seq_impl
{
    using type = typename prepend_<typename reverse_seq_impl<N - 1, Ns...>::type, N>::type;
};

template <int... Ns>
struct reverse_seq_impl<0, Ns...>
{
    using type = detail::seq_<0, Ns...>;
};

template <template <int> class Pred, int... Ns>
struct all_of_impl;

template <template <int> class Pred, int First, int... Rest>
struct all_of_impl<Pred, First, Rest...>
{
    static const constexpr bool value = Pred<First>::value && all_of_impl<Pred, Rest...>::value;
};

template <template <int> class Pred>
struct all_of_impl<Pred>
{
    static const constexpr bool value = true;
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

template <typename... Types>
struct length : detail::length_impl<Types...>
{
};

template <typename TList, typename T>
struct append : detail::append_impl<TList, T>
{
};

template <typename... Types, typename T>
struct append<types_sequence<Types...>, T>
{
    typedef types_sequence<Types..., T> type;
};

template <unsigned int base, unsigned int exponent>
struct power : detail::power_impl<base, exponent>
{
};

template <unsigned int N>
struct is_power_of_two : detail::is_power_of_two_impl<N>
{
};

template <int N, int... List>
struct contains : detail::contains_impl<N, List...>
{
};

template <int N>
struct is_even : detail::is_even_impl<N>
{
};

template <int N>
struct is_odd : detail::is_odd_impl<N>
{
};

template <unsigned long N>
struct sqrt : detail::sqrt_impl<N, 1, N>
{
};

template <int... Ns>
struct sum : detail::sum_impl<Ns...>
{
};

template <int N, template <int> class F>
struct static_for : detail::static_for_impl<N, F>
{
};

template <int N, int... Ns>
struct reverse_seq : detail::reverse_seq_impl<N, Ns...>
{
};

template <template <int> class Pred, int... Ns>
struct all_of : detail::all_of_impl<Pred, Ns...>
{
};

}  // namespace computational
}  // namespace erturk

#endif  // ERTURK_COMPUTATIONAL_H