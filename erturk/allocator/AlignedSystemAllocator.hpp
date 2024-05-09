#ifndef ERTURK_SYSTEM_ALLOC_H
#define ERTURK_SYSTEM_ALLOC_H

#include "../memory/Alignment.hpp"
#include <memory>
#include <new>

namespace erturk
{
namespace allocator
{

template <class T>
class AlignedSystemAllocator final
{
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

public:
    AlignedSystemAllocator() noexcept = default;

    AlignedSystemAllocator(const AlignedSystemAllocator&) noexcept = delete;

    AlignedSystemAllocator& operator=(const AlignedSystemAllocator&) noexcept = delete;

    AlignedSystemAllocator(AlignedSystemAllocator&&) noexcept = default;

    AlignedSystemAllocator& operator=(AlignedSystemAllocator&&) noexcept = default;

    ~AlignedSystemAllocator() noexcept = default;

    pointer allocate(const size_t requestInBytes, size_t alignment = alignof(T))
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
            // store un-aligned memory address just before aligned base address
            *((void**)((char*)raw_memory - sizeof(void*))) = raw_memory;

            return static_cast<T*>(raw_memory);
        }

        // put base addr after sizeof(void*) so we have space to put raw_memory
        void* offseted_base_addr = static_cast<char*>(raw_memory) + sizeof(void*);

        void* aligned_memory = memory::alignment::alignPointerFromBuffer(alignment_, total_required_size,
                                                                         offseted_base_addr, aligned_total_size);

        if (aligned_memory == nullptr)
        {
            ::operator delete(raw_memory);
            return nullptr;
        }

        // store un-aligned memory address just before aligned base address
        *((void**)((char*)aligned_memory - sizeof(void*))) = raw_memory;

        return static_cast<T*>(aligned_memory);
    }

    // n: number of storages earlier passed to allocate()
    void deallocate(const T* ptr, const size_t n) noexcept
    {
        if (ptr != nullptr)
        {
            // retrieve un-aligned memory address just before aligned base address
            T* raw_memory = static_cast<T*>(*((void**)((char*)ptr - sizeof(void*))));

            if (raw_memory == nullptr)
            {
                // somethings went wrong! (raw memory may corruped or overrided!)
                ::operator delete(ptr, n * sizeof(T), alignment_);
                return;
            }

            ::operator delete(raw_memory, n * sizeof(T), alignment_);
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
            new (ptr) U{std::forward<Args>(values)...};
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

}  // namespace allocator
}  // namespace erturk

#endif  // ERTURK_SYSTEM_ALLOC_H