#ifndef ERTURK_SEQUENTIAL_H
#define ERTURK_SEQUENTIAL_H

#include <cstddef>

namespace erturk
{
namespace sequential
{

namespace detail
{

template <typename T, T... Seqs>
struct integer_sequence
{
    using value_type = T;
    static const constexpr std::size_t size() noexcept
    {
        return sizeof...(Seqs);
    }
};


template <typename T, T Size, T... Seqs>
struct make_integer_sequence_helper;

template <typename T, T Size, T... Seqs>
struct make_integer_sequence_helper : make_integer_sequence_helper<T, Size - 1, Size - 1, Seqs...>
{
};

template <signed int... Seqs>
struct make_integer_sequence_helper<signed int, 0, Seqs...>
{
    using type = integer_sequence<signed int, Seqs...>;
};

template <unsigned int... Seqs>
struct make_integer_sequence_helper<unsigned int, 0, Seqs...>
{
    using type = integer_sequence<unsigned int, Seqs...>;
};

template <signed short... Seqs>
struct make_integer_sequence_helper<signed short, 0, Seqs...>
{
    using type = integer_sequence<signed short, Seqs...>;
};

template <unsigned short... Seqs>
struct make_integer_sequence_helper<unsigned short, 0, Seqs...>
{
    using type = integer_sequence<unsigned short, Seqs...>;
};

template <signed char... Seqs>
struct make_integer_sequence_helper<signed char, 0, Seqs...>
{
    using type = integer_sequence<signed char, Seqs...>;
};

template <unsigned char... Seqs>
struct make_integer_sequence_helper<unsigned char, 0, Seqs...>
{
    using type = integer_sequence<unsigned char, Seqs...>;
};

template <std::size_t... Seqs>
struct make_integer_sequence_helper<std::size_t, 0, Seqs...>
{
    using type = integer_sequence<std::size_t, Seqs...>;
};

template <long... Seqs>
struct make_integer_sequence_helper<long, 0, Seqs...>
{
    using type = integer_sequence<long, Seqs...>;
};

}  // namespace detail

// ************************************************************************************************************

template <typename T, T... Ts>
using integer_sequence_t = detail::integer_sequence<T, Ts...>;

template <std::size_t... Seqs>
using index_sequence_t = detail::integer_sequence<std::size_t, Seqs...>;

template <typename T, T Size>
using make_integer_sequence = typename detail::make_integer_sequence_helper<T, Size>::type;

template <std::size_t Size>
using make_index_sequence = make_integer_sequence<std::size_t, Size>;

}  // namespace sequential
}  // namespace erturk

#endif  // ERTURK_SEQUENTIAL_H
