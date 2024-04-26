#ifndef ERTURK_SYSTEM_ALLOC_H
#define ERTURK_SYSTEM_ALLOC_H

#include <new>
#include <memory>

namespace erturk
{
namespace allocator
{

template<class T>
class SystemAllocator final {
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef T value_type;

public:
    SystemAllocator() noexcept = default;

    SystemAllocator(const SystemAllocator &) noexcept = default;

    SystemAllocator &operator=(const SystemAllocator &) noexcept = default;

    SystemAllocator(SystemAllocator &&) noexcept = default;

    SystemAllocator &operator=(SystemAllocator &&) noexcept = default;

    ~SystemAllocator() noexcept = default;

    T *allocate(std::size_t requestInBytes, std::size_t alignment = alignof(T)) {
        if (requestInBytes == 0) {
            return nullptr;
        }
        std::size_t required_size = requestInBytes * sizeof(T);

        std::size_t total_size = required_size + alignment - 1 + sizeof(void *);

        void *raw_memory = ::operator new(total_size);

        void *start = static_cast<char *>(raw_memory) + sizeof(void *);

        void *aligned_memory = std::align(alignment, required_size, start, total_size);

        if (aligned_memory == nullptr) {
            ::operator delete(raw_memory);
            throw std::bad_alloc();
        }

        // Store the original pointer before the aligned address for proper deallocation
        *((void **) ((char *) aligned_memory - sizeof(void *))) = raw_memory;
        return static_cast<T *>(aligned_memory);
    }

    void deallocate(T *ptr, std::size_t n) noexcept {
        if (ptr != nullptr) {
            // Retrieve the original pointer stored just before the aligned address
            void *raw_memory = *((void **) ((char *) ptr - sizeof(void *)));
            ::operator delete(raw_memory);
        }
    }

    template<typename U>
    struct rebind {
        using other = SystemAllocator<U>;
    };

    template<typename U, typename... Args>
    void construct(U *ptr, Args&&... values) {
        if (ptr != nullptr) {
            new(ptr) U{std::forward<Args>(values)...};
        }
    }

    template<typename U>
    void destroy(U *ptr) {
        if (ptr != nullptr) {
            ptr->~U();
        }
    }
};

}  // namespace allocator
}  // namespace erturk

#endif  // ERTURK_SYSTEM_ALLOC_H
