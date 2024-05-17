#ifndef ERTURK_TYPE_BUFFER_H
#define ERTURK_TYPE_BUFFER_H

#include "../meta_types/TypeTrait.hpp"
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace erturk::memory
{

template <typename T>
class TypeBuffer final
{
    static_assert(std::is_destructible<T>::value, "Type T must be destructible");
    static_assert(!std::is_abstract<T>::value, "Type T cannot be abstract");

public:
    explicit constexpr TypeBuffer() noexcept(false)
        : type_buffer_{},
          is_initialized_{false},
          construct_functor_{[]() {
              throw std::logic_error("Default construction attempted without initialization!");
          }},
          destruct_functor_{default_destructor}
    {
    }

    template <typename... Args>
    explicit constexpr TypeBuffer(Args&&... args)
        : type_buffer_{},
          is_initialized_{false},
          construct_functor_{[this, &args...]() {  // capture by reference, arguments may not copy-move constructible
              this->construct(std::forward<Args>(args)...);
          }},
          destruct_functor_{default_destructor}
    {
        static_assert(std::is_default_constructible<T>::value, "No matching default constructor for type T");
    }

    TypeBuffer(const TypeBuffer<T>& other)
        : type_buffer_{},
          is_initialized_{false},
          construct_functor_{other.construct_functor_},
          destruct_functor_{other.destruct_functor_}
    {
        static_assert(erturk::meta::is_copy_constructible<T>::value, "No matching copy constructor for type T");

        if (other.is_initialized_)
        {
            construct(other.operator*());
        }
    }

    TypeBuffer(TypeBuffer<T>&& other) noexcept
        : type_buffer_{},
          is_initialized_{false},
          construct_functor_{std::move(other.construct_functor_)},
          destruct_functor_{std::move(other.destruct_functor_)}
    {
        static_assert(erturk::meta::is_move_constructible<T>::value, "No matching move constructor for type T");

        if (other.is_initialized_)
        {
            construct(std::move(other.operator*()));
        }

        other.construct_functor_ = nullptr;
        other.destruct_functor_ = nullptr;
    }

    explicit TypeBuffer(const T& other)
        : type_buffer_{},
          is_initialized_{false},
          construct_functor_{[this, &other]() {  // capture by reference, arguments may not copy-move constructible
              this->construct(std::move(other));
          }},
          destruct_functor_(default_destructor)
    {
        static_assert(erturk::meta::is_copy_constructible<T>::value, "No matching copy constructor for type T");
    }

    explicit TypeBuffer(T&& other)
        : type_buffer_{},
          is_initialized_{false},
          construct_functor_{[this, &other]() {  // capture by reference, arguments may not copy-move constructible
              this->construct(std::move(other));
          }},
          destruct_functor_{default_destructor}
    {
        static_assert(erturk::meta::is_move_constructible<T>::value, "No matching move constructor for type T");
    }

    ~TypeBuffer()
    {
        destruct();
    }

    TypeBuffer<T>& operator=(const TypeBuffer<T>& other)
    {
        static_assert(std::is_copy_assignable<T>::value || erturk::meta::is_copy_constructible<T>::value,
                      "No matching copy/move assignment operator for type T");

        if (other.is_initialized_)
        {
            instantiate();
            assign(other.operator*());
        }
        else
        {
            construct_functor_ = other.construct_functor_;
        }
        destruct_functor_ = other.destruct_functor_;

        return (*this);
    }

    TypeBuffer<T>& operator=(TypeBuffer<T>&& other) noexcept
    {
        static_assert(std::is_copy_assignable<T>::value || erturk::meta::is_copy_constructible<T>::value,
                      "No matching copy/move assignment operator for type T");

        if (other.is_initialized_)
        {
            instantiate();
            assign(std::move(other.operator*()));
        }
        else
        {
            construct_functor_ = std::move(other.construct_functor_);
            other.construct_functor_ = nullptr;
        }
        destruct_functor_ = std::move(other.destruct_functor_);
        other.destruct_functor_ = nullptr;

        return (*this);
    }

    template <typename... Args>
    void emplace(Args&&... args)
    {
        static_assert(std::is_constructible<T, Args...>::value || erturk::meta::is_copy_constructible<T>::value
                          || erturk::meta::is_move_constructible<T>::value,
                      "No matching constructor for type T with given arguments");

        // capture by reference, arguments may not copy-move constructible
        construct_functor_ = [this, &args...]() {
            this->construct(std::forward<Args>(args)...);
        };

        instantiate();
    }

    template <typename... Args>
    void emplace_lazy(Args&&... args)
    {
        static_assert(std::is_constructible<T, Args...>::value || erturk::meta::is_copy_constructible<T>::value
                          || erturk::meta::is_move_constructible<T>::value,
                      "No matching constructor for type T with given arguments");

        // capture by reference, arguments may not copy-move constructible
        construct_functor_ = [this, &args...]() {
            this->construct(std::forward<Args>(args)...);
        };
    }

    T& operator=(const T& tVal)
    {
        static_assert(std::is_copy_assignable<T>::value, "No matching copy assignment operator for type T");

        instantiate();
        assign(tVal);

        return *type_buffer_address();
    }

    T& operator=(T&& tVal)
    {
        static_assert(std::is_move_assignable<T>::value, "No matching move assignment operator for type T");

        instantiate();
        assign(std::forward<T>(tVal));

        return *type_buffer_address();
    }

    explicit operator T&() const
    {
        instantiate();
        return *type_buffer_address();
    }

    explicit operator bool() const noexcept
    {
        return is_initialized();
    }

    bool is_initialized() const noexcept
    {
        return is_initialized_;
    }

    T* get() const
    {
        instantiate();
        return type_buffer_address();
    }

    T& operator*() const
    {
        instantiate();
        return *type_buffer_address();
    }

    T* operator->() const
    {
        instantiate();
        return type_buffer_address();
    }

    void reset()
    {
        destruct();
        construct_functor_ = []() {
            throw std::logic_error("Uninitialized construction after reset!");
        };
    }

    size_t size_of_buffer() const noexcept
    {
        return sizeof(type_buffer_);
    }

    size_t size_of_alignment() const noexcept
    {
        return alignof(T);
    }

private:
    using unqualified_pointer = typename std::remove_cv<T>::type*;

    // "val" call destructor on end of scope by default
    static constexpr void default_destructor(T& val) noexcept {}

    // address-of idiom (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Address_Of)
    unqualified_pointer type_buffer_address() const noexcept
    {
        return reinterpret_cast<unqualified_pointer>(
            &const_cast<char&>(reinterpret_cast<const volatile char&>(type_buffer_)));
    }

    void instantiate() const
    {
        if (!is_initialized_)
        {
            construct_functor_.operator()();
            is_initialized_ = true;
        }
    }

    // Default constructor
    // User-defined constructor
    // Copy Constructor
    // Move constructor
    template <typename... Args>
    void construct(Args&&... args) const
    {
        static_assert(std::is_constructible<T, Args...>::value || erturk::meta::is_copy_constructible<T>::value
                          || erturk::meta::is_move_constructible<T>::value,
                      "No matching constructor for type T with given arguments");

        destruct();

        new (type_buffer_address()) T{std::forward<Args>(args)...};  // in-place instantiation
        is_initialized_ = true;
    }

    void destruct() const
    {
        if (is_initialized_)
        {
            if (destruct_functor_)
            {
                destruct_functor_(*type_buffer_address());
            }
            type_buffer_address()->~T();
            is_initialized_ = false;
        }
    }

    void assign(const T& rhs) const noexcept(std::is_nothrow_copy_assignable<T>::value)
    {
        (*type_buffer_address()) = rhs;
    }

    void assign(T&& rhs) const noexcept(std::is_nothrow_move_assignable<T>::value)
    {
        (*type_buffer_address()) = std::forward<T>(rhs);
    }

private:
    mutable unsigned char type_buffer_[((sizeof(T) + alignof(T) - 1) / alignof(T)) * alignof(T)];
    mutable bool is_initialized_;
    std::function<void()> construct_functor_;
    std::function<void(T&)> destruct_functor_;
};

}  // namespace erturk::memory

#endif  // ERTURK_TYPE_BUFFER_H
