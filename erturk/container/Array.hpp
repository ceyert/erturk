#ifndef ERTURK_CONTAINER_ARRAY_H
#define ERTURK_CONTAINER_ARRAY_H

#include "../meta_types/TypeTrait.hpp"
#include <stdexcept>
#include <utility>

namespace erturk::container
{
template <typename T, const size_t SIZE>
class Array final
{
public:
    // Instantiate buffer_ in-place with default constructor
    constexpr Array()
    {
        static_assert(
            (erturk::meta::is_default_constructible<T>::value || erturk::meta::is_trivially_constructible<T>::value
             || erturk::meta::is_trivially_default_constructible<T>::value),
            "T must be default constructible!");

        for (size_t idx = 0; idx < SIZE; idx++)
        {
            buffer_[idx] = T{};
        }
    }

    // Instantiate buffer_ in-place with copy constructor
    template <typename... TypeSeq>
    constexpr Array(const TypeSeq&... t_refs) : buffer_{T{t_refs}...}
    {
        static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                      "T must be copy constructible or move constructible!");

        static_assert(sizeof...(TypeSeq) == SIZE, "Arguments does not match array size!");
    }

    constexpr explicit Array(const T& val)
    {
        static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                      "T must be copy constructible or move constructible!");

        for (size_t idx = 0; idx < SIZE; idx++)
        {
            buffer_[idx] = val;
        }
    }

    constexpr Array(const Array& other)
    {
        static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                      "T must be copy constructible or move constructible!");

        for (size_t idx = 0; idx < SIZE; idx++)
        {
            buffer_[idx] = other.buffer_[idx];
        }
    }

    constexpr Array(Array&& other) noexcept
    {
        static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                      "T must be copy constructible or move constructible!");

        for (size_t idx = 0; idx < SIZE; idx++)
        {
            buffer_[idx] = std::move(other.buffer_[idx]);
        }
    }

    constexpr Array& operator=(const Array& other)
    {
        if (this != &other)
        {
            for (size_t idx = 0; idx < SIZE; idx++)
            {
                buffer_[idx] = other.buffer_[idx];
            }
        }
        return *this;
    }

    constexpr Array& operator=(Array&& other) noexcept
    {
        if (this != &other)
        {
            for (size_t idx = 0; idx < SIZE; idx++)
            {
                buffer_[idx] = std::move(other.buffer_[idx]);
            }
        }
        return *this;
    }

    ~Array()
    {
        for (size_t idx = 0; idx < SIZE; idx++)
        {
            buffer_[idx].~T();
        }
    }

    template <typename... Args>
    constexpr void emplace(size_t pos, Args&&... args)
    {
        static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                      "T must be copy constructible or move constructible!");

        buffer_[pos] = T{std::forward<Args>(args)...};
    }

    template <typename... Args>
    constexpr void emplaceAll(Args&&... args)
    {
        static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                      "T must be copy constructible or move constructible!");

        for (size_t idx = 0; idx < SIZE; idx++)
        {
            buffer_[idx] = T{std::forward<Args>(args)...};
        }
    }

    void fill(const T& value)
    {
        for (size_t idx = 0; idx < SIZE; idx++)
        {
            buffer_[idx] = value;
        }
    }

    void fill(const T& value, T* start, const T* end)
    {
        if (start < buffer_ || end > buffer_ + SIZE)
        {
            throw std::out_of_range("Fill range out of buffer bounds");
        }

        while (start != end)
        {
            *start = value;
            start++;
        }
    }

    void assign(const T& value)
    {
        fill(value);
    }

    [[nodiscard]] T& at(const size_t pos) noexcept(false)
    {
        if (pos >= SIZE)
        {
            throw std::out_of_range("Index out of range");
        }
        return buffer_[pos];
    }

    [[nodiscard]] const T& at(const size_t pos) const noexcept(false)
    {
        if (pos >= SIZE)
        {
            throw std::out_of_range("Index out of range");
        }
        return buffer_[pos];
    }

    [[nodiscard]] T& operator[](const size_t pos)
    {
        return buffer_[pos];
    }

    [[nodiscard]] const T& operator[](const size_t pos) const
    {
        return buffer_[pos];
    }

    [[nodiscard]] T& front()
    {
        return buffer_[0];
    }

    [[nodiscard]] const T& front() const
    {
        return buffer_[0];
    }

    [[nodiscard]] T& back()
    {
        return buffer_[SIZE - 1];
    }

    [[nodiscard]] const T& back() const
    {
        return buffer_[SIZE - 1];
    }

    [[nodiscard]] bool empty() const
    {
        return SIZE == 0;
    }

    [[nodiscard]] size_t size() const
    {
        return SIZE;
    }

private:
    T buffer_[SIZE];

public:
    class Iterator
    {
    public:
        explicit Iterator(T* ptr) : t_ptr_{ptr} {}

        Iterator& operator++()
        {
            t_ptr_++;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            t_ptr_++;
            return temp;
        }

        bool operator==(const Iterator& other) const
        {
            return t_ptr_ == other.t_ptr_;
        }

        bool operator!=(const Iterator& other) const
        {
            return t_ptr_ != other.t_ptr_;
        }

        T& operator*()
        {
            return *t_ptr_;
        }

    private:
        T* t_ptr_{nullptr};
    };

    [[nodiscard]] Iterator begin()
    {
        return Iterator{buffer_};
    }

    [[nodiscard]] Iterator end()
    {
        return Iterator{buffer_ + SIZE};
    }

    [[nodiscard]] Iterator begin() const
    {
        return Iterator{buffer_};
    }

    [[nodiscard]] Iterator end() const
    {
        return Iterator{buffer_ + SIZE};
    }
};
}  // namespace erturk::container

#endif  // ERTURK_CONTAINER_ARRAY_H
