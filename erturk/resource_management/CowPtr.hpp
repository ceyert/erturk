#ifndef ERTURK_COW_PTR_H
#define ERTURK_COW_PTR_H

#include "../meta_types/TypeTrait.hpp"
#include <atomic>
#include <functional>
#include <utility>

namespace erturk::resource_management
{
template <class T, class Allocator, class Deleter>
class CowPtr final
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

public:
    explicit CowPtr(T* resource_ptr, const Allocator& alloc, const Deleter& deleter)
        : resource_control_ptr_{new ResourceControl_{resource_ptr, deleter}}, allocator_{alloc}
    {
    }

    // Apply pointer shallow copy and increase reference count
    CowPtr(const CowPtr& other) : resource_control_ptr_{other.resource_control_ptr_}, allocator_{other.allocator_}
    {
        resource_control_ptr_->reference_count_.fetch_add(1, std::memory_order_relaxed);
    }

    // Transfer control ownership
    CowPtr(CowPtr&& other) noexcept : resource_control_ptr_{other.resource_control_ptr_}, allocator_{other.allocator_}
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
            allocator_ = other.allocator_;
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
            allocator_ = other.allocator_;
            other.resource_control_ptr_ = nullptr;
        }
        return *this;
    }

    ~CowPtr()
    {
        release_resource_if();
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
    [[nodiscard]] const T& read() const noexcept
    {
        return *resource_control_ptr_->resource_;
    }

    [[nodiscard]] T& write() noexcept(false)
    {
        detach_resource_if();
        return *resource_control_ptr_->resource_;
    }

    void detach_resource_if() noexcept(false)
    {
        if (resource_control_ptr_->reference_count_.load(std::memory_order_acquire) > 1)
        {
            ResourceControl_* old_resource_control = resource_control_ptr_;

            // decrease old reference count
            old_resource_control->reference_count_.fetch_sub(1, std::memory_order_acq_rel);

            T* new_resource_ptr = allocator_.operator()();

            if (new_resource_ptr == nullptr)
            {
                release_resource_if();
                throw;
            }

            // Call assign operator to apply deep copy
            // T has responsibility to handle deep copy
            if constexpr (erturk::meta::is_copy_constructible<T>::value)
            {
                new_resource_ptr->operator=(*old_resource_control->resource_);
            }
            else
            {
                new_resource_ptr->operator=(std::move(*old_resource_control->resource_));
            }

            // Allocate new resource control
            resource_control_ptr_ = new ResourceControl_{new_resource_ptr, old_resource_control->deleter_};
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
        explicit ResourceControl_(T* resource, Deleter deleter)
            : resource_{resource}, reference_count_{1}, deleter_{deleter}
        {
        }

        ~ResourceControl_()
        {
            if (reference_count_.load(std::memory_order_acquire) == 0 && resource_ != nullptr)
            {
                deleter_.operator()(resource_);
            }
        }

        T* resource_{nullptr};
        std::atomic<size_t> reference_count_{0};
        Deleter deleter_{};
    };

    ResourceControl_* resource_control_ptr_{nullptr};
    Allocator allocator_{};
};

template <class T, class... Args>
inline CowPtr<T, std::function<T*()>, std::function<void(T*)>> make_cow_ptr(Args&&... args) noexcept(false)
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

    auto default_allocator = [... arguments = std::forward<Args>(args)]() -> T* {
        return new T{arguments...};
    };

    auto default_deleter = [](T* address) -> void {
        delete address;
    };
    return CowPtr<T, std::function<T*()>, std::function<void(T*)>>{new T{std::forward<Args>(args)...},
                                                                   default_allocator, default_deleter};
}

}  // namespace erturk::resource_management

#endif  // ERTURK_COW_PTR_H
