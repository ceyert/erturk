#ifndef ERTURK_SYSTEM_ALLOC_H
#define ERTURK_SYSTEM_ALLOC_H

#include "../memory/Alignment.hpp"
#include <memory>
#include <new>

namespace erturk::allocator
{

template <class T, const size_t Alignment = alignof(T)>
class AlignedSystemAllocator
{
    static_assert((memory::alignment::isSizePowerOfTwo(Alignment)), "Alignment must be power of 2!");

public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer_type;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

public:
    constexpr AlignedSystemAllocator() noexcept = default;

    constexpr AlignedSystemAllocator(const AlignedSystemAllocator&) noexcept = default;

    constexpr AlignedSystemAllocator& operator=(const AlignedSystemAllocator&) noexcept = default;

    constexpr AlignedSystemAllocator(AlignedSystemAllocator&&) noexcept = default;

    constexpr AlignedSystemAllocator& operator=(AlignedSystemAllocator&&) noexcept = default;

    constexpr ~AlignedSystemAllocator() noexcept = default;

    pointer_type allocate(const size_t count) noexcept
    {
        if (count == 0)
        {
            return nullptr;
        }

        const size_t total_required_size = ((count * sizeof(T)) + sizeof(void*));  // add additional space for storing un-aligned address

        const size_t rounded_total_size = memory::alignment::advanceSizeByAlignment(total_required_size, Alignment);

        void* raw_memory = std::malloc(rounded_total_size);

        if (raw_memory == nullptr)
        {
            return nullptr;
        }

        // check raw_memory is already aligned if we're lucky
        if (memory::alignment::isAddressAligned(raw_memory, Alignment))
        {
            return storeAndAlign(raw_memory);
        }

        void* advanced_addr = memory::alignment::advanceAddressByAlignment(raw_memory, Alignment);
        if (advanced_addr == nullptr)
        {
            std::free(static_cast<void*>(raw_memory));
            return nullptr;
        }

        return storeAndAlign(advanced_addr);
    }

    /**
     *  @brief  Deallocate memory.
     *  @param  ptr  Pointer to the memory to deallocate.
     *  @param  n  The number of objects space was allocated for.
     */
    void deallocate(T* ptr, const size_t) noexcept
    {
        if (ptr != nullptr)
        {
            // retrieve un-aligned memory address just before provided aligned address
            void* un_aligned_addr = memory::alignment::rewindAddressByAlignment(ptr, Alignment);
            if (un_aligned_addr == nullptr)
            {
                // something's went wrong! (raw memory may corrupt or override!)
                std::free(static_cast<T*>(ptr));
                return;
            }
            std::free(static_cast<T*>(un_aligned_addr));
        }
    }

    void deallocate(T* ptr) noexcept
    {
        if (ptr != nullptr)
        {
            // retrieve un-aligned memory address just before provided aligned address
            void* un_aligned_addr = memory::alignment::rewindAddressByAlignment(ptr, Alignment);
            if (un_aligned_addr == nullptr)
            {
                // something's went wrong! (raw memory may corrupt or override!)
                std::free(static_cast<T*>(ptr));
                return;
            }
            std::free(static_cast<T*>(un_aligned_addr));
        }
    }

    template <typename U>
    struct rebind
    {
        using other = AlignedSystemAllocator<U>;
    };

    template <typename U, typename... Args>
    void construct(U* addr, Args&&... args)
    {
        if (addr != nullptr)
        {
            new (addr) U{std::forward<Args>(args)...};  // construct at
        }
    }

    template <typename U>
    void destroy(U* addr)
    {
        if (addr != nullptr)
        {
            addr->~U();
        }
    }

    T* storeAndAlign(void* addr)
    {
        // store aligned memory address into aligned base address
        *((void**)static_cast<unsigned char*>(addr)) = addr;

        void* advanced_addr = memory::alignment::advanceAddressByAlignment(addr, Alignment);
        if (advanced_addr == nullptr)
        {
            std::free(static_cast<void*>(addr));
            return nullptr;
        }

        if (!memory::alignment::isAddressAligned(advanced_addr, Alignment))
        {
            // still?!
            std::free(static_cast<void*>(addr));
            return nullptr;
        }

        // return next aligned memory address just after base address
        return static_cast<T*>(advanced_addr);
    }
};

}  // namespace erturk::allocator

#endif  // ERTURK_SYSTEM_ALLOC_H