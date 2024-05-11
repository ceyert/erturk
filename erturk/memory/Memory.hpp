#ifndef ERTURK_MEMORY_H
#define ERTURK_MEMORY_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace erturk::memory
{

inline void* memmove(void* destination, const void* source, const size_t size)
{
    if (destination == nullptr || source == nullptr || size <= 0)
    {
        return nullptr;
    }

    char* dest_base = static_cast<char*>(destination);
    const char* src_base = static_cast<const char*>(source);

    // check source and destination memory regions overlap
    if ((src_base < dest_base) && (dest_base < src_base + size)) // overlap
    {
        // copy from end to start to avoid overwriting
        size_t idx = size;
        while (idx > 0)
        {
            dest_base[idx] = src_base[idx];
            idx--;
        }
    }
    else // no overlap
    {
        // copy from start to end
        size_t idx = 0;
        while (idx < size)
        {
            dest_base[idx] = src_base[idx];
            ++idx;
        }
    }
    return destination;
}

inline void* memset(void* destination, int value, size_t size)
{
    if (destination == nullptr || size <= 0)
    {
        return nullptr;
    }

    unsigned char* addr = static_cast<unsigned char*>(destination);
    unsigned char val = static_cast<unsigned char>(value);
    while (size > 0)
    {
        *addr++ = val;
        size--;
    }

    return destination;
}

inline void* memcpy(void* destination, const void* source, size_t size)
{
    if (destination == nullptr || source == nullptr || size <= 0)
    {
        return nullptr;
    }

    char* dest = static_cast<char*>(destination);
    const char* src = static_cast<const char*>(source);

    while (size > 0)
    {
        *dest++ = *src++;
        size--;
    }
    return destination;
}

inline int memcmp(const void* str1, const void* str2, size_t size)
{
    if (str1 == nullptr || str2 == nullptr || size <= 0)
    {
        return 0;
    }

    const unsigned char* str1_ = static_cast<const unsigned char*>(str1);
    const unsigned char* str2_ = static_cast<const unsigned char*>(str2);

    while (size > 0)
    {
        if (*str1_ != *str2_)
        {
            return (*str1_ < *str2_) ? -1 : 1;
        } 
        str1_++;
        str2_++;
        size--;
    }
    return 0;
}

}  // namespace erturk::memory

#endif  // ERTURK_MEMORY_H