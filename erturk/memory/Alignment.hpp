#ifndef ERTURK_MEMALIGN_H
#define ERTURK_MEMALIGN_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace erturk::memory::alignment
{

inline constexpr bool isSizePowerOfTwo(const size_t size)
{
    return size != 0 && (size & (size - 1)) == 0;
}

inline constexpr size_t alignSizePowerOfTwo(const size_t size)
{
    size_t count = size;
    size_t result = 1;
    while (result < size)
    {
        result *= 2;
        if (--count == 0)
        {
            return 0;
        }
    }
    return result;
}

inline constexpr size_t alignSizeWithAlignment(const size_t size, const size_t alignment)
{
    size_t count = size;
    size_t result = 1;
    while (result < size)
    {
        result *= alignment;
        if (--count == 0)
        {
            return 0;
        }
    }
    return result;
}

inline constexpr bool isAddressPowerOfTwo(const void* const ptr)
{
    if (ptr == nullptr)
    {
        return false;
    }

    return isSizePowerOfTwo(reinterpret_cast<size_t>(ptr));
}

inline constexpr size_t advanceSizeByAlignment(size_t size, const size_t alignment, bool force = false)
{
    if (force)
    {
        return ((size / alignment) + 1) * alignment;  // +1 used to round up
    }
    if (size % alignment != 0)
    {
        return ((size / alignment) + 1) * alignment;  // +1 used to round up
    }
    return size;
}

inline constexpr size_t rewindSizeByAlignment(size_t size, const size_t alignment, bool force = false)
{
    if (force)
    {
        return ((size / alignment) - 1) * alignment;  // +1 used to round up
    }
    if (size % alignment != 0)
    {
        return ((size / alignment) - 1) * alignment;  // +1 used to round up
    }
    return size;
}

// To advance a pointer to the next address with the desired alignment within buffer
inline constexpr void* alignAddressFromBuffer(const size_t alignment, const size_t size, void* ptr, size_t& space)
{
    if (!isSizePowerOfTwo(alignment) || ptr == nullptr)
    {
        return nullptr;
    }

    uintptr_t address = reinterpret_cast<uintptr_t>(ptr);

    size_t aligned = advanceSizeByAlignment(address, alignment);

    size_t usedSpace = aligned - address;

    if (space < usedSpace + size)
    {
        return nullptr;
    }

    space -= usedSpace;
    return ptr = reinterpret_cast<void*>(aligned);
}

template <typename T>
inline constexpr bool isStorageAligned(const T* const storage, const size_t alignment)
{
    if (storage == nullptr)
    {
        return false;
    }
    // check that, provided storage address is alined with provided alignment
    return reinterpret_cast<uintptr_t>(storage) % alignment == 0;
}

template <typename T>
inline constexpr bool isStorageAligned(const T* const storage)
{
    if (storage == nullptr)
    {
        return false;
    }
    // check that, provided storage address is alined with type alignment
    return reinterpret_cast<uintptr_t>(storage) % alignof(T) == 0;
}

inline constexpr bool isAddressAligned(const void* const ptr, const size_t alignment)
{
    if (!isSizePowerOfTwo(alignment))
    {
        return false;  // Alignment must be a power of two
    }
    uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
    return (address % alignment) == 0;
}

// Unsafe : Ensure that address is sequential that is larger than alignment size
inline constexpr void* advanceAddressByAlignment(void* ptr, const size_t alignment)
{
    if (!isSizePowerOfTwo(alignment) || ptr == nullptr)
    {
        return nullptr;
    }

    uintptr_t address = reinterpret_cast<uintptr_t>(ptr);

    size_t aligned = advanceSizeByAlignment(address, alignment, true);

    return ptr = reinterpret_cast<void*>(aligned);
}

// Unsafe : Ensure that address is sequential that is larger than alignment size
inline constexpr void* rewindAddressByAlignment(void* ptr, const size_t alignment)
{
    if (!isSizePowerOfTwo(alignment) || ptr == nullptr)
    {
        return nullptr;
    }

    uintptr_t address = reinterpret_cast<uintptr_t>(ptr);

    size_t aligned = rewindSizeByAlignment(address, alignment, true);

    return ptr = reinterpret_cast<void*>(aligned);
}

}  // namespace erturk::memory::alignment

#endif  // ERTURK_MEMALIGN_H