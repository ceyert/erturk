#ifndef ERTURK_MEMLAYOUT_H
#define ERTURK_MEMLAYOUT_H

#include <cstdint>
#include <stdexcept>

namespace erturk 
{
    namespace memory::layout 
    {
        inline size_t alignment(size_t value, size_t alignment) 
        {
            if (value % alignment != 0) 
            {
                return ((value / alignment) + 1) * alignment; // +1 used to round up
            }
            return value;
        }

        inline bool isPowerOfTwo(uintptr_t ptr) 
        {
            return ptr != 0 && (ptr & (ptr - 1)) == 0;
        }

        inline bool isAddressPowerOfTwo(void *ptr) 
        {
            uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
            return isPowerOfTwo(address);
        }

        inline bool isAddressPowerOfAlignment(void *ptr, size_t alignment) 
        {
            if (!isPowerOfTwo(alignment)) 
            {
                return false;  // Alignment must be a power of two
            }
            uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
            return (address % alignment) == 0;
        }

        inline void *alignAddress(void *ptr, size_t alignment) 
        {
            if (ptr == nullptr || alignment == 0 || !isAddressPowerOfAlignment(ptr, alignment)) 
            {
                return nullptr;
            }

            uintptr_t mask = alignment - 1;
            uintptr_t address = reinterpret_cast<uintptr_t>(ptr);

            // Rounds up the address to the nearest multiple of the alignment
            uintptr_t alignedAddress = (address + mask) & ~mask;
            return reinterpret_cast<void *>(alignedAddress);
        }
    }  // namespace memory::layout
}  // namespace erturk

#endif  // ERTURK_MEMLAYOUT_H