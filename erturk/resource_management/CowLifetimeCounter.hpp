#ifndef ERTURK_COW_PTR_H
#define ERTURK_COW_PTR_H

#include "../meta_types/TypeTrait.hpp"
#include <atomic>
#include <functional>
#include <thread>
#include <utility>
#if defined(__x86_64__)
#include <immintrin.h>
#define USE_X86_PAUSE
#endif

namespace erturk::resource_management
{
template <class T, class Allocator, class Deleter>
class CowLifetimeCounter final
{
    class ResourceControl_ final
    {
    public:
        explicit ResourceControl_(T* resource, const Allocator& allocator, const Deleter& deleter)
            : resource_{resource},
              resource_freed_{false},
              reference_count_{1},
              weak_count_{0},
              allocator_{allocator},
              deleter_{deleter},
              locked_{false}
        {
        }

        ResourceControl_(const ResourceControl_& other) = delete;

        ResourceControl_(ResourceControl_&& other) noexcept = delete;

        ResourceControl_& operator=(const ResourceControl_& other) = delete;

        ResourceControl_& operator=(ResourceControl_&& other) noexcept = delete;

        // SAFETY: resource managed by explicit free_resource_if() call
        ~ResourceControl_() = default;

        void increase_reference_count()
        {
            reference_count_.fetch_add(1, std::memory_order_acq_rel);
        }

        // Decrease reference count, free resource if reference count is 0.
        void decrease_reference_count()
        {
            reference_count_.fetch_sub(1, std::memory_order_acq_rel);
            free_resource_if();
        }

        void increase_weak_count()
        {
            weak_count_.fetch_add(1, std::memory_order_acq_rel);
        }

        void decrease_weak_count()
        {
            weak_count_.fetch_sub(1, std::memory_order_acq_rel);
        }

        [[nodiscard]] size_t reference_count() const noexcept
        {
            return reference_count_.load(std::memory_order_acquire);
        }

        [[nodiscard]] size_t weak_count() const noexcept
        {
            return weak_count_.load(std::memory_order_acquire);
        }

        [[nodiscard]] T* get_resource() noexcept(false)
        {
            if (is_resource_freed())
            {
                throw std::runtime_error("Resource already freed!");
            }
            return resource_;
        }

        [[nodiscard]] bool is_resource_freed() const
        {
            return resource_freed_;
        }

        [[nodiscard]] bool is_locked() const noexcept
        {
            return locked_.load(std::memory_order_acquire);
        }

        void set_lock(bool flag) noexcept
        {
            locked_.store(flag, std::memory_order_acq_rel);
        }

        [[nodiscard]] T* allocate()
        {
            return allocator_.operator()();
        }

        [[nodiscard]] Allocator& get_allocator()
        {
            return allocator_;
        }

        [[nodiscard]] Deleter& get_deleter()
        {
            return deleter_;
        }

    private:
        void free_resource_if()
        {
            if (!is_resource_freed())
            {
                if (reference_count_.load(std::memory_order_acquire) == 0 && resource_ != nullptr)
                {
                    deleter_.operator()(resource_);
                    resource_freed_ = true;
                    resource_ = nullptr;
                }
            }
        }

        T* resource_{nullptr};
        bool resource_freed_{false};
        std::atomic<size_t> reference_count_{0};
        std::atomic<size_t> weak_count_{0};
        Allocator allocator_{};
        Deleter deleter_{};
        std::atomic_bool locked_{false};
    };

    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

    static_assert(erturk::meta::is_same<decltype(std::declval<Allocator>()()), T*>::value,
                  "Allocator return type must be T*");

    static_assert(erturk::meta::is_same<decltype(std::declval<Deleter>()(std::declval<T*>())), void>::value,
                  "Deleter parameter must be T* and return void");

public:
    explicit CowLifetimeCounter(T* resource_ptr, const Allocator& allocator, const Deleter& deleter)
        : resource_control_ptr_{new ResourceControl_{resource_ptr, allocator, deleter}}
    {
    }

    // Apply pointer shallow copy and increase reference count
    CowLifetimeCounter(const CowLifetimeCounter& other) noexcept(false)
    {
        if (other.resource_control_ptr_ == nullptr)
        {
            throw std::runtime_error("Instantiation from invalid CowPtr!");
        }
        resource_control_ptr_ = other.resource_control_ptr_;
        resource_control_ptr_->increase_reference_count();
    }

    // Take over ownership
    CowLifetimeCounter(CowLifetimeCounter&& other) noexcept(false)
    {
        if (other.resource_control_ptr_ == nullptr)
        {
            throw std::runtime_error("Instantiation from invalid CowPtr!");
        }
        resource_control_ptr_ = other.resource_control_ptr_;
        other.resource_control_ptr_ = nullptr;
    }

    // Apply pointer shallow copy and increase reference count
    CowLifetimeCounter& operator=(const CowLifetimeCounter& other)
    {
        if (this != &other && other.resource_control_ptr_ != nullptr)
        {
            // both CowPtr manage the same resource?
            if (resource_control_ptr_ != other.resource_control_ptr_)
            {
                resource_control_ptr_->decrease_reference_count();  // leave previous resource
            }
            resource_control_ptr_ = other.resource_control_ptr_;  // set new address or current address
            resource_control_ptr_->increase_reference_count();
        }
        return *this;
    }

    // Take over ownership
    CowLifetimeCounter& operator=(CowLifetimeCounter&& other) noexcept
    {
        if (this != &other && other.resource_control_ptr_ != nullptr)
        {
            if (resource_control_ptr_ != other.resource_control_ptr_)
            {
                resource_control_ptr_->decrease_reference_count();  // leave previous resource
            }
            resource_control_ptr_ = other.resource_control_ptr_;  // set new address or current address

            other.resource_control_ptr_ = nullptr;
        }
        return *this;
    }

    // Check weak and reference count 0. If so, free resource control.
    // Otherwise, let other CowPtr storage handle free resource control.
    ~CowLifetimeCounter()
    {
        resource_control_ptr_->decrease_reference_count();

        if (resource_control_ptr_->reference_count() == 0 && resource_control_ptr_->weak_count() == 0)
        {
            delete resource_control_ptr_;
            resource_control_ptr_ = nullptr;
        }
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
        return resource_control_ptr_->reference_count() == 1;
    }

    [[nodiscard]] size_t reference_count() const noexcept
    {
        return resource_control_ptr_->reference_count();
    }

    [[nodiscard]] size_t weak_count() const noexcept
    {
        return resource_control_ptr_->weak_count();
    }

    void detach() const
    {
        detach_resource_if();
    }

private:
    [[nodiscard]] const T& read() const noexcept
    {
        return *resource_control_ptr_->get_resource();
    }

    [[nodiscard]] T& write() noexcept(false)
    {
        detach_resource_if();
        return *resource_control_ptr_->get_resource();
    }

    void detach_resource_if() noexcept(false)
    {
        if (resource_control_ptr_->reference_count() > 1)
        {
            if (resource_control_ptr_->is_locked())
            {
                while (resource_control_ptr_->is_locked())
                {
#ifdef USE_X86_PAUSE
                    _mm_pause();
#else
                    std::this_thread::yield();
#endif
                }

                // Check once again after a certain wait!
                if (resource_control_ptr_->reference_count() == 1)
                {
                    return;
                }
            }
            resource_control_ptr_->set_lock(true);

            // decrease reference count
            resource_control_ptr_->decrease_reference_count();

            T* new_resource_ptr = resource_control_ptr_->allocate();

            if (new_resource_ptr == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            // Call assign operator to apply deep copy
            // T has responsibility to handle deep copy
            if constexpr (erturk::meta::is_copy_constructible<T>::value)
            {
                new_resource_ptr->operator=(*resource_control_ptr_->get_resource());  // guarantee resource not freed!
            }
            else
            {
                new_resource_ptr->operator=(
                    std::move(*resource_control_ptr_->get_resource()));  // guarantee resource not freed!
            }

            ResourceControl_* old_resource_control = resource_control_ptr_;
            // Allocate new resource control
            resource_control_ptr_ = new ResourceControl_{new_resource_ptr, old_resource_control->get_allocator(),
                                                         old_resource_control->get_deleter()};

            resource_control_ptr_->set_lock(false);
        }
    }

    ResourceControl_* resource_control_ptr_{nullptr};
};

template <class T, class... Args>
inline CowLifetimeCounter<T, std::function<T*()>, std::function<void(T*)>> make_cow_ptr(Args&&... args) noexcept(false)
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

    // Capture arguments by value!
    std::function<T*(void)> defaultAllocator = [args...]() -> T* {
        return new T{args...};
    };

    std::function<void(T*)> defaultDeleter = [](T* address) -> void {
        if (address != nullptr)
        {
            delete address;
        }
    };
    return CowLifetimeCounter<T, std::function<T*()>, std::function<void(T*)>>{new T{std::forward<Args>(args)...},
                                                                          defaultAllocator, defaultDeleter};
}

}  // namespace erturk::resource_management

#endif  // ERTURK_COW_PTR_H
