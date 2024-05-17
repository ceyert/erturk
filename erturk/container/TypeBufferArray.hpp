#ifndef ERTURK_CONTAINER_TYPE_BUFFER_ARRAY_H
#define ERTURK_CONTAINER_TYPE_BUFFER_ARRAY_H

#include "../memory/TypeBuffer.hpp"
#include "../meta_types/TypeTrait.hpp"
#include <stdexcept>
#include <utility>

namespace erturk::container
{
template <typename T, const size_t SIZE>
class TypeBufferArray final
{
public:
    constexpr TypeBufferArray() = default;

    template <typename... TypeSeq>
    constexpr explicit TypeBufferArray(TypeSeq&&... t_refs)
        : type_buffer_array_{erturk::memory::TypeBuffer<T>{std::forward<T>(t_refs)}...}
    {
        static_assert(sizeof...(TypeSeq) == SIZE, "Arguments does not match array size!");
    }

    constexpr TypeBufferArray(const TypeBufferArray& other)
    {
        for (size_t idx = 0; idx < SIZE; idx++)
        {
            type_buffer_array_[idx] = other.type_buffer_array_[idx];
        }
    }

    constexpr TypeBufferArray(TypeBufferArray&& other) noexcept
    {
        for (size_t idx = 0; idx < SIZE; idx++)
        {
            type_buffer_array_[idx] = std::move(other.type_buffer_array_[idx]);
        }
    }

    constexpr TypeBufferArray& operator=(const TypeBufferArray& other)
    {
        if (this != &other)
        {
            for (size_t idx = 0; idx < SIZE; idx++)
            {
                type_buffer_array_[idx] = other.type_buffer_array_[idx];
            }
        }
        return *this;
    }

    constexpr TypeBufferArray& operator=(TypeBufferArray&& other) noexcept
    {
        if (this != &other)
        {
            for (size_t idx = 0; idx < SIZE; idx++)
            {
                type_buffer_array_[idx] = std::move(other.type_buffer_array_[idx]);
            }
        }
        return *this;
    }

    ~TypeBufferArray()
    {
        for (size_t idx = 0; idx < SIZE; idx++)
        {
            type_buffer_array_[idx].reset();
        }
    }

    template <typename... Args>
    constexpr void emplace(size_t pos, Args&&... args)
    {
        type_buffer_array_[pos].emplace(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void emplaceAll(Args&&... args)
    {
        for (size_t idx = 0; idx < SIZE; idx++)
        {
            type_buffer_array_[idx].emplace(std::forward<Args>(args)...);
        }
    }

    void fill(const T& value)
    {
        for (size_t idx = 0; idx < SIZE; idx++)
        {
            type_buffer_array_[idx].emplace(value);
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
        return type_buffer_array_[pos].operator*();
    }

    [[nodiscard]] const T& at(const size_t pos) const noexcept(false)
    {
        if (pos >= SIZE)
        {
            throw std::out_of_range("Index out of range");
        }
        return type_buffer_array_[pos].operator*();
    }

    [[nodiscard]] erturk::memory::TypeBuffer<T>& operator[](const size_t pos)
    {
        return type_buffer_array_[pos];
    }

    [[nodiscard]] const erturk::memory::TypeBuffer<T>& operator[](const size_t pos) const
    {
        return type_buffer_array_[pos];
    }

    [[nodiscard]] bool is_initialized(const size_t pos)
    {
        return type_buffer_array_[pos].is_initialized();
    }

    [[nodiscard]] T& front()
    {
        return type_buffer_array_[0].operator*();
    }

    [[nodiscard]] const T& front() const
    {
        return type_buffer_array_[0].operator*();
    }

    [[nodiscard]] T& back()
    {
        return type_buffer_array_[SIZE - 1].operator*();
    }

    [[nodiscard]] const T& back() const
    {
        return type_buffer_array_[SIZE - 1].operator*();
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
    erturk::memory::TypeBuffer<T> type_buffer_array_[SIZE];

public:
    class Iterator
    {
    public:
        explicit Iterator(T* ptr) : type_buffer_ptr_{ptr} {}

        Iterator& operator++()
        {
            type_buffer_ptr_->operator->()++;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            type_buffer_ptr_->operator->()++;
            return temp;
        }

        bool operator==(const Iterator& other) const
        {
            return type_buffer_ptr_->operator->() == other.type_buffer_ptr_->operator->();
        }

        bool operator!=(const Iterator& other) const
        {
            return type_buffer_ptr_->operator->() != other.type_buffer_ptr_->operator->();
        }

        T& operator*()
        {
            return *type_buffer_ptr_->operator*();
        }

    private:
        erturk::memory::TypeBuffer<T>* type_buffer_ptr_{nullptr};
    };

    [[nodiscard]] Iterator begin()
    {
        return Iterator{type_buffer_array_};
    }

    [[nodiscard]] Iterator end()
    {
        return Iterator{type_buffer_array_ + SIZE};
    }

    [[nodiscard]] Iterator begin() const
    {
        return Iterator{type_buffer_array_};
    }

    [[nodiscard]] Iterator end() const
    {
        return Iterator{type_buffer_array_ + SIZE};
    }
};
}  // namespace erturk::container

#endif  // ERTURK_CONTAINER_TYPE_BUFFER_ARRAY_H
