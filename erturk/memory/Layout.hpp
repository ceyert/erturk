#ifndef ERTURK_MEMLAYOUT_H
#define ERTURK_MEMLAYOUT_H

#include <cstdint>
#include <cstddef>
#include <stdexcept>

namespace erturk 
{
    namespace memory::layout 
    {
        template<typename T>
        inline bool isStorageAligned(const T* const storage) 
        {
            if (storage == nullptr) 
            {
                return false;
            }
            // check that, provided storage address is alined with type alignment
            return reinterpret_cast<std::uintptr_t>(storage) % alignof(T) == 0;
        }

        inline size_t alignSize(size_t size, size_t alignment) 
        {
            if (size % alignment != 0) 
            {
                return ((size / alignment) + 1) * alignment; // +1 used to round up
            }
            return size;
        }

        inline bool isAddressAligned(const void* const ptr, size_t alignment) 
        {
            if (!isSizePowerOfTwo(alignment)) 
            {
                return false;  // Alignment must be a power of two
            }
            uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
            return (address % alignment) == 0;
        }

        inline bool isAddressPowerOfTwo(const void* const ptr) 
        {
            uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
            return isSizePowerOfTwo(address);
        }

        inline bool isSizePowerOfTwo(uintptr_t size) 
        {
            return size != 0 && (size & (size - 1)) == 0;
        }

    }  // namespace memory::layout
}  // namespace erturk

#endif  // ERTURK_MEMLAYOUT_H