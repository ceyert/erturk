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



}  // namespace detail

// ************************************************************************************************************************************

namespace meta
{


}  // namespace meta

}  // namespace erturk

#endif  // ERTURK_META_H
