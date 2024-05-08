#ifndef ERTURK_MEMALIGN_H
#define ERTURK_MEMALIGN_H

#include <cstdint>
#include <cstddef>
#include <stdexcept>

namespace erturk 
{
    namespace memory::alignment 
    {

        inline bool isSizePowerOfTwo(size_t size) 
        {
            return size != 0 && (size & (size - 1)) == 0;
        }

        inline bool isAddressPowerOfTwo(const void* const ptr) 
        {
            if (ptr == nullptr)
            {
                return false;
            }
            
            return isSizePowerOfTwo(reinterpret_cast<size_t>(ptr));
        }

        inline size_t alignSize(size_t size, size_t alignment) 
        {
            if (size % alignment != 0) 
            {
                return ((size / alignment) + 1) * alignment; // +1 used to round up
            }
            return size;
        }

        // To advance a pointer to the next address with the desired alignment within buffer
        inline void *alignPointerFromBuffer(const size_t alignment, const size_t size, void* ptr, size_t &space) 
        {
            if (!isSizePowerOfTwo(alignment) || ptr == nullptr) 
            {
                return nullptr;
            }

            uintptr_t address = reinterpret_cast<uintptr_t>(ptr);

            size_t aligned = alignSize(address, alignment);

            size_t usedSpace = aligned - address;

            if (space < usedSpace + size) 
            {
                return nullptr;
            }

            space -= usedSpace; 
            return ptr = reinterpret_cast<void *>(aligned);
        }

        template<typename T>
        inline bool isStorageAligned(const T* const storage, const size_t alignment) 
        {
            if (storage == nullptr) 
            {
                return false;
            }
            // check that, provided storage address is alined with provided alignment
            return reinterpret_cast<uintptr_t>(storage) % alignment == 0;
        }

        template<typename T>
        inline bool isStorageAligned(const T* const storage) 
        {
            if (storage == nullptr) 
            {
                return false;
            }
            // check that, provided storage address is alined with type alignment
            return reinterpret_cast<uintptr_t>(storage) % alignof(T) == 0;
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

    }  // namespace memory::alignment
}  // namespace erturk

#endif  // ERTURK_MEMALIGN_H