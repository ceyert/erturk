#ifndef ERTURK_MEMORY_H
#define ERTURK_MEMORY_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace erturk::memory
{

inline void* memset(void* destination, const int value, const size_t size)
{
    if (destination == nullptr || size == 0)
    {
        return nullptr;
    }

    unsigned char* addr = static_cast<unsigned char*>(destination);
    const unsigned char val = static_cast<const unsigned char>(value);

    size_t idx = 0;
    while (size > idx)
    {
        *addr++ = val;
        idx++;
    }

    return destination;
}

template <class T, class T_Iterator>
inline constexpr T_Iterator memset(const T_Iterator dest_begin, const T_Iterator dest_end, const T& value)
{
    if (dest_begin == nullptr || dest_end == nullptr)
    {
        return nullptr;
    }

    T_Iterator curr_dest = const_cast<T_Iterator>(dest_begin);

    while (curr_dest != dest_end)
    {
        *curr_dest = value;
        curr_dest++;  // increase to next T memory layout
    }
    return curr_dest;
}

template <class T>
inline constexpr T* memset(const T* dest_begin, const T* dest_end, const T& value)
{
    if (dest_begin == nullptr || dest_end == nullptr)
    {
        return nullptr;
    }

    T* curr_dest = const_cast<T*>(dest_begin);

    while (curr_dest != dest_end)
    {
        *curr_dest = value;

        curr_dest++;  // increase to next T memory layout
    }
    return curr_dest;
}

template <class T, class T_Iterator, class Size>
inline constexpr T_Iterator memset_n(const T_Iterator dest_begin, const Size size, const T& value)
{
    if (dest_begin == nullptr || size <= 0)
    {
        return nullptr;
    }

    T_Iterator curr_dest = const_cast<T_Iterator>(dest_begin);

    Size idx = 0;

    while (size > idx)
    {
        *curr_dest = value;

        curr_dest++;  // increase to next T memory layout
        idx++;
    }
    return curr_dest;
}

template <class T, class Size>
inline constexpr T* memset_n(const T* dest_begin, const Size size, const T& value)
{
    if (dest_begin == nullptr || size <= 0)
    {
        return nullptr;
    }

    T* curr_dest = const_cast<T*>(dest_begin);

    size_t idx = 0;

    while (size > idx)
    {
        *curr_dest = value;
        curr_dest++;  // increase to next T memory layout
        idx++;
    }
    return curr_dest;
}

inline void* memcpy(const void* source, void* destination, const size_t size)
{
    if (destination == nullptr || source == nullptr || size <= 0)
    {
        return nullptr;
    }

    char* dest = static_cast<char*>(destination);
    const char* src = static_cast<const char*>(source);

    size_t idx = 0;
    while (size > idx)
    {
        *dest++ = *src++;
        idx++;
    }

    return destination;
}

template <class T, class T_Iterator>
inline constexpr T_Iterator memcpy(const T_Iterator src_begin, const T_Iterator src_end, const T_Iterator dest_begin)
{
    if (src_begin == nullptr || src_end == nullptr || dest_begin == nullptr)
    {
        return nullptr;
    }

    T_Iterator src_begin_ = const_cast<T_Iterator>(src_begin);
    T_Iterator curr_dest = const_cast<T_Iterator>(dest_begin);

    while (src_begin_ != src_end)
    {
        *curr_dest = *src_begin_;

        src_begin_++;  // increase to next T memory layout
        curr_dest++;   // increase to next T memory layout
    }
    return curr_dest;
}

template <class T>
inline constexpr T* memcpy(const T* src_begin, const T* src_end, const T* dest_begin)
{
    if (src_begin == nullptr || src_end == nullptr || dest_begin == nullptr)
    {
        return nullptr;
    }

    T* curr_dest = const_cast<T*>(dest_begin);

    while (src_begin != src_end)
    {
        *curr_dest = *src_begin;

        src_begin++;  // increase to next T memory layout
        curr_dest++;  // increase to next T memory layout
    }
    return curr_dest;
}

template <class T, class T_Iterator, class Size>
inline constexpr T_Iterator memcpy_n(const T_Iterator src_begin, const Size size, const T_Iterator dest_begin)
{
    if (src_begin == nullptr || size <= 0 || dest_begin == nullptr)
    {
        return nullptr;
    }

    T_Iterator src_begin_ = const_cast<T_Iterator>(src_begin);
    T_Iterator curr_dest = const_cast<T_Iterator>(dest_begin);

    Size idx = 0;
    while (size > idx)
    {
        *curr_dest = *src_begin_;

        src_begin_++;  // increase to next T memory layout
        curr_dest++;   // increase to next T memory layout
        idx--;
    }
    return curr_dest;
}

template <class T>
inline constexpr T* memcpy_n(const T* src_begin, const size_t size, const T* dest_begin)
{
    if (src_begin == nullptr || size <= 0 || dest_begin == nullptr)
    {
        return nullptr;
    }

    T* curr_dest = const_cast<T*>(dest_begin);

    size_t idx = 0;
    while (size > idx)
    {
        *curr_dest = *src_begin;

        src_begin++;  // increase to next T memory layout
        curr_dest++;  // increase to next T memory layout
        idx++;
    }
    return curr_dest;
}

inline int memcmp(const void* str1, const void* str2, const size_t size)
{
    if (str1 == nullptr || str2 == nullptr || size <= 0)
    {
        return 0;
    }

    const unsigned char* str1_ = static_cast<const unsigned char*>(str1);
    const unsigned char* str2_ = static_cast<const unsigned char*>(str2);
    size_t size_ = size;

    while (size_ > 0)
    {
        if (*str1_ != *str2_)
        {
            return (*str1_ < *str2_) ? -1 : 1;
        }
        str1_++;
        str2_++;
        size_--;
    }
    return 0;
}

/*
Memory overlapping is source and destination buffers have the same memory addresses.

During the copy, data corruption occurs because a source is on the left side of the destination, the data of source is
already changed.

memmove is used to copy a block of memory from source to destination memory address with preserving overlapping memory
regions.

ref: https://opensource.apple.com/source/network_cmds/network_cmds-481.20.1/unbound/compat/memmove.c.auto.html
*/
inline void* memmove(void* destination, const void* source, size_t size)
{
    if (destination == nullptr || source == nullptr || size <= 0)
    {
        return nullptr;
    }

    uint8_t* from = (uint8_t*)source;
    uint8_t* to = (uint8_t*)destination;

    if (from == to || size == 0)
    {
        return destination;
    }

    if (to > from && to - from < (int)size)
    {
        /* to overlaps with from */
        /*  <from......>         */
        /*         <to........>  */
        /* copy in reverse, to avoid overwriting from */
        for (int i = size - 1; i >= 0; i--)
        {
            to[i] = from[i];
        }
        return destination;
    }
    if (from > to && from - to < (int)size)
    {
        /* to overlaps with from */
        /*        <from......>   */
        /*  <to........>         */
        /* copy forwards, to avoid overwriting from */
        for (size_t i = 0; i < size; i++)
        {
            to[i] = from[i];
        }
        return destination;
    }

    memcpy(source, destination, size);

    return destination;
}

}  // namespace erturk::memory

#endif  // ERTURK_MEMORY_H