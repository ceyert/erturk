#ifndef ERTURK_SYSTEM_ALLOC_H
#define ERTURK_SYSTEM_ALLOC_H

#include "../memory/Alignment.hpp"
#include <memory>
#include <new>

namespace erturk::allocator
{

template <class T>
class AlignedSystemAllocator
{
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer_type;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

public:
    AlignedSystemAllocator() noexcept = default;

    AlignedSystemAllocator(const AlignedSystemAllocator&) noexcept = default;

    AlignedSystemAllocator& operator=(const AlignedSystemAllocator&) noexcept = default;

    AlignedSystemAllocator(AlignedSystemAllocator&&) noexcept = default;

    AlignedSystemAllocator& operator=(AlignedSystemAllocator&&) noexcept = default;

    ~AlignedSystemAllocator() noexcept = default;

    pointer_type allocate(const size_t requestInBytes, size_t alignment = alignof(T))
    {
        if (requestInBytes == 0)
        {
            return nullptr;
        }

        // check that alignment is aligned
        if (!memory::alignment::isSizePowerOfTwo(alignment))
        {
            alignment = memory::alignment::alignSize(alignment, alignof(T));
        }

        alignment_ = alignment;

        size_t total_required_size = (requestInBytes * sizeof(T));

        size_t aligned_total_size = memory::alignment::alignSize(total_required_size, alignment_);

        void* raw_memory = ::operator new(aligned_total_size);

        if (raw_memory == nullptr)
        {
            return nullptr;
        }

        // check raw_memory is already aligned if we lucky
        if (memory::alignment::isAddressAligned(raw_memory, alignment_))
        {
            // store aligned memory address into aligned base address
            *((void**)static_cast<char*>(raw_memory)) = raw_memory;

            void* next_addr = static_cast<char*>(raw_memory) + sizeof(void*);

            if (!memory::alignment::isAddressAligned(next_addr, alignment))
            {
                void* next_aligned_addr = memory::alignment::alignAddressUnsafe(raw_memory, alignment);

                // return next aligned memory address just after base address
                return static_cast<T*>(next_aligned_addr);
            }

            // return next aligned memory address just after base address
            return static_cast<T*>(next_addr);
        }

        void* aligned_addr = memory::alignment::alignAddressUnsafe(raw_memory, alignment);

        // store un-aligned memory address into aligned base address
        *((void**)static_cast<char*>(aligned_addr)) = aligned_addr;

        // put base addr after sizeof(void*) so we have space to put raw_memory
        void* next_addr = static_cast<char*>(aligned_addr) + sizeof(void*);

        // check next_addr is aligned
        if (!memory::alignment::isAddressAligned(next_addr, alignment))
        {
            void* next_aligned_addr = memory::alignment::alignAddressUnsafe(raw_memory, alignment);

            // return next aligned memory address just after base address
            return static_cast<T*>(next_aligned_addr);
        }
        // return next aligned memory address just after base address
        return static_cast<T*>(next_addr);
    }

    /**
     *  @brief  Deallocate memory.
     *  @param  ptr  Pointer to the memory to deallocate.
     *  @param  n  The number of objects space was allocated for.
     *
     */
    void deallocate(T* ptr, const size_t n) noexcept
    {
        if (ptr != nullptr)
        {
            // retrieve un-aligned memory address just before aligned base address
            T* raw_memory = static_cast<T*>(*((void**)(reinterpret_cast<char*>(ptr) - sizeof(void*))));

            if (raw_memory == nullptr)
            {
                // somethings went wrong! (raw memory may corruped or overrided!)
                ::operator delete(ptr, n * sizeof(T));
                return;
            }

            ::operator delete(raw_memory, n * sizeof(T));
        }
    }

    void deallocate(T* ptr) noexcept
    {
        if (ptr != nullptr)
        {
            // retrieve un-aligned memory address just before aligned base address
            T* raw_memory = static_cast<T*>(*((void**)(reinterpret_cast<char*>(ptr) - sizeof(void*))));

            if (raw_memory == nullptr)
            {
                // somethings went wrong! (raw memory may corruped or overrided!)
                ::operator delete(ptr);
                return;
            }

            ::operator delete(raw_memory);
        }
    }

    template <typename U>
    struct rebind
    {
        using other = AlignedSystemAllocator<U>;
    };

    template <typename U, typename... Args>
    void construct(U* ptr, Args&&... values)
    {
        if (ptr != nullptr)
        {
            new (ptr) U{std::forward<Args>(values)...};  // construct at
        }
    }

    template <typename U>
    void destroy(U* ptr)
    {
        if (ptr != nullptr)
        {
            ptr->~U();
        }
    }

private:
    size_t alignment_{0};
};

}  // namespace erturk::allocator

#endif  // ERTURK_SYSTEM_ALLOC_H