#ifndef ERTURK_COW_PTR_H
#define ERTURK_COW_PTR_H

#include "../meta_types/TypeTrait.hpp"
#include <atomic>
#include <utility>

namespace erturk::resource_management
{
template <class T, T*(newCopy)(const T*)>
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

private:
    void detach_resource_if()
    {
        if (resource_control_ptr_->reference_count_.load(std::memory_order_acquire) > 1)
        {
            ResourceControl_* old_resource_control = resource_control_ptr_;

            // decrease old reference count
            old_resource_control->reference_count_.fetch_sub(1, std::memory_order_acq_rel);

            // New deep-copied resource
            T* new_resource_ptr = newCopy(old_resource_control->resource_ptr_);
            if (new_resource_ptr == nullptr)
            {
                release_resource_if();
                throw;
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
                delete resource_;
            }
        }

        const T* resource_{nullptr};
        std::atomic<size_t> reference_count_{0};
    };

    ResourceControl_* resource_control_ptr_{nullptr};
};

template <class T, T*(newCopy)(const T*, size_t)>
class CowPtr<T[], newCopy> final
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

public:
    explicit CowPtr(const T* ptrToBuffer) : resource_control_ptr_{new ResourceControl_{ptrToBuffer}} {}

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
        return *resource_control_ptr_->ptrToBuffer_;
    }

    [[nodiscard]] T& write() noexcept(false)
    {
        detach_resource_if();
        return *resource_control_ptr_->ptrToBuffer_;
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

private:
    void detach_resource_if()
    {
        if (resource_control_ptr_->reference_count_.load(std::memory_order_acquire) > 1)
        {
            ResourceControl_* old_resource_control = resource_control_ptr_;

            // decrease old reference count
            old_resource_control->reference_count_.fetch_sub(1, std::memory_order_acq_rel);

            // New deep-copied resource
            T* new_resource_ptr = newCopy(old_resource_control->ptrToBuffer_);
            if (new_resource_ptr == nullptr)
            {
                throw;
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
        explicit ResourceControl_(const T* ptrToBuffer) : ptrToBuffer_{ptrToBuffer}, reference_count_{1} {}

        ~ResourceControl_()
        {
            if (reference_count_.load(std::memory_order_acquire) == 0 && ptrToBuffer_ != nullptr)
            {
                delete[] ptrToBuffer_;
            }
        }

        const T* ptrToBuffer_{nullptr};
        std::atomic<size_t> reference_count_{0};
    };

    ResourceControl_* resource_control_ptr_{nullptr};
};

template <class T, T*(newCopy)(const T*)>
using CowArray = CowPtr<T[], newCopy>;

template <class T>
inline T* makeNewCopy(const T* oldAddr)
{
    if constexpr (erturk::meta::is_copy_constructible<T>::value)
    {
        return new T{*oldAddr};
    }
    return new T{std::move(*oldAddr)};
}

template <class T>
inline T* makeNewArrayCopy(const T* oldAddr, size_t count)
{
    if constexpr (erturk::meta::is_copy_constructible<T>::value)
    {
        return new T[count]{*oldAddr};
    }
    return new T[count]{std::move(*oldAddr)};
}

template <class T, T*(newCopy)(const T*) = makeNewCopy, class... Args>
inline CowPtr<T, newCopy>&& makeCowPtr(Args&&... args) noexcept(false)
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

    return CowPtr<T, newCopy>{new T{std::forward<Args>(args)...}};
}

template <class T, T*(newCopy)(const T*, size_t) = makeNewArrayCopy, class... Args>
inline CowArray<T, newCopy>&& makeCowArray(const size_t count, Args&&... args) noexcept(false)
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

    return CowArray<T, newCopy>{new T[count]{std::forward<Args>(args)...}};
}

}  // namespace erturk::resource_management

#endif  // ERTURK_COW_PTR_H
