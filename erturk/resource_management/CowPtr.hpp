#ifndef ERTURK_COW_PTR_H
#define ERTURK_COW_PTR_H

#include "../meta_types/TypeTrait.hpp"
#include <atomic>
#include <functional>
#include <utility>

namespace erturk::resource_management
{
template <class T, const std::function<T*()> allocator, const std::function<void(T*)> deleter>
class CowPtr final
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

public:
    explicit CowPtr(const T* resource_ptr) : resource_control_ptr_{new ResourceControl_{resource_ptr}} {}

    // Apply pointer shallow copy and increase reference count
    CowPtr(const CowPtr& other) : resource_control_ptr_{other.resource_control_ptr_}
    {
        resource_control_ptr_->reference_count_.fetch_add(1, std::memory_order_relaxed);
    }

    // Transfer control ownership
    CowPtr(CowPtr&& other) noexcept : resource_control_ptr_{other.resource_control_ptr_}
    {
        other.resource_control_ptr_ = nullptr;
    }

    // Apply pointer shallow copy and increase reference count
    CowPtr& operator=(const CowPtr& other)
    {
        if (this != &other)
        {
            release_resource_if();
            resource_control_ptr_ = other.resource_control_ptr_;
            resource_control_ptr_->reference_count_.fetch_add(1, std::memory_order_relaxed);
        }
        return *this;
    }

    // Transfer control ownership
    CowPtr& operator=(CowPtr&& other) noexcept
    {
        if (this != &other)
        {
            release_resource_if();
            resource_control_ptr_ = other.resource_control_ptr_;
            other.resource_control_ptr_ = nullptr;
        }
        return *this;
    }

    ~CowPtr()
    {
        release_resource_if();
    }

    [[nodiscard]] const T& read() const noexcept
    {
        return *resource_control_ptr_->resource_ptr_;
    }

    [[nodiscard]] T& write() noexcept(false)
    {
        detach_resource_if();
        return *resource_control_ptr_->resource_ptr_;
    }

    [[nodiscard]] T* operator->()
    {
        return &write();
    }

    [[nodiscard]] const T* operator->() const noexcept
    {
        return &read();
    }

    [[nodiscard]] T& operator*()
    {
        return write();
    }

    [[nodiscard]] const T& operator*() const noexcept
    {
        return read();
    }

    [[nodiscard]] bool is_unique() const noexcept
    {
        return resource_control_ptr_->reference_count_.load(std::memory_order_acquire) == 1;
    }

    void detach() const
    {
        detach_resource_if();
    }

private:
    void detach_resource_if() noexcept(false)
    {
        if (resource_control_ptr_->reference_count_.load(std::memory_order_acquire) > 1)
        {
            ResourceControl_* old_resource_control = resource_control_ptr_;

            // decrease old reference count
            old_resource_control->reference_count_.fetch_sub(1, std::memory_order_acq_rel);

            T* new_resource_ptr = allocator.operator()();

            if (new_resource_ptr == nullptr)
            {
                release_resource_if();
                throw;
            }

            // Call assign operator to apply deep copy
            // T has responsibility to handle deep copy
            if constexpr (erturk::meta::is_copy_constructible<T>::value)
            {
                new_resource_ptr->operator=(*old_resource_control->resource_ptr_);
            }
            else
            {
                new_resource_ptr->operator=(std::move(*old_resource_control->resource_ptr_));
            }

            // Allocate new resource control
            resource_control_ptr_ = new ResourceControl_{new_resource_ptr};
        }
    }

    // Decrease and check reference count 0. If so, free resources.
    // Otherwise, let other CowPtr storage handle free resource.
    void release_resource_if()
    {
        resource_control_ptr_->reference_count_.fetch_sub(1, std::memory_order_release);
        if (resource_control_ptr_->reference_count_.load(std::memory_order_acquire) == 0)
        {
            delete resource_control_ptr_;
            resource_control_ptr_ = nullptr;
        }
    }

    struct ResourceControl_ final
    {
        explicit ResourceControl_(const T* resource) : resource_{resource}, reference_count_{1} {}

        ~ResourceControl_()
        {
            if (reference_count_.load(std::memory_order_acquire) == 0 && resource_ != nullptr)
            {
                deleter.operator()(resource_);
            }
        }

        const T* resource_{nullptr};
        std::atomic<size_t> reference_count_{0};
    };

    ResourceControl_* resource_control_ptr_{nullptr};
};

inline auto allocator = []<typename T, typename... Args>(Args&&... args) -> T* {
    return new T{std::forward<Args>(args)...};
};

inline auto deleter = []<typename T>(T* address) -> void {
    delete address;
};

template <class T, auto alloc = allocator, auto del = deleter, class... Args>
inline CowPtr<T, alloc, del>&& make_cow_ptr(Args&&... args) noexcept(false)
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

    return CowPtr<T, alloc, del>{new T{std::forward<Args>(args)...}};
}

}  // namespace erturk::resource_management

#endif  // ERTURK_COW_PTR_H
