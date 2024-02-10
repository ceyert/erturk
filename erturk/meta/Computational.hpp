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

}  // namespace computational
}  // namespace erturk

#endif  // ERTURK_COMPUTATIONAL_H