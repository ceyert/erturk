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

}  // namespace computational
}  // namespace erturk

#endif  // ERTURK_COMPUTATIONAL_H