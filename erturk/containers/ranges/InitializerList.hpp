#ifndef ERTURK_INITIALIZER_LIST_H
#define ERTURK_INITIALIZER_LIST_H

#include <cstddef>  

namespace erturk
{
namespace range
{
template <class T>
class InitializerList
{
public:
    InitializerList(const T* begin, const T* end) : beginPtr_(begin), endPtr_(end) {}

    size_t size() const
    {
        return (endPtr_ - beginPtr_) / sizeof(T);
    }

    const T* begin() const
    {
        return beginPtr_;
    }

    const T* end() const
    {
        return endPtr_;
    }

    const T& operator[](const size_t index) const
    {
        return *(beginPtr_ + index);
    }

private:
    const T* beginPtr_;
    const T* endPtr_;
};
}  // namespace range
}  // namespace erturk

#endif  // ERTURK_INITIALIZER_LIST_H