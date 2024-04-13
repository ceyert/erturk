#ifndef ERTURK_CONTAINER_ARRAY_H
#define ERTURK_CONTAINER_ARRAY_H

#include <stdexcept>

namespace erturk
{
namespace container
{
template <typename T, const std::size_t SIZE>
class Array
{
public:
    constexpr Array()
    {
        for (std::size_t i = 0; i < SIZE; i++)
        {
            buffer_[i] = T{};
        }
    }

    template <typename... Args>
    constexpr Array(Args... args) : buffer_{T{args}...}
    {
        static_assert(sizeof...(Args) == SIZE, "Number of arguments does not match array size.");
    }

    constexpr Array(const T& val)
    {
        for (std::size_t i = 0; i < SIZE; i++)
        {
            buffer_[i] = val;
        }
    }

    constexpr Array(T&& val)
    {
        for (std::size_t i = 0; i < SIZE; i++)
        {
            buffer_[i] = std::move(val);
        }
    }

    constexpr Array(const Array& other)
    {
        for (std::size_t i = 0; i < SIZE; i++)
        {
            buffer_[i] = other.buffer_[i];
        }
    }

    constexpr Array(Array&& other) noexcept
    {
        for (std::size_t i = 0; i < SIZE; i++)
        {
            buffer_[i] = std::move(other.buffer_[i]);
        }
    }

    void fill(const T& value)
    {
        for (std::size_t i = 0; i < SIZE; i++)
        {
            buffer_[i] = value;
        }
    }

    void fill(const T& value, T* start, T* end)
    {
        if (start < buffer_ || end > buffer_ + SIZE)
        {
            throw std::out_of_range("Fill range out of buffer bounds");
        }

        while (start != end)
        {
            *start++ = value;
        }
    }

    void assign(const T& value)
    {
        fill(value);
    }

    [[nodiscard]] T& at(std::size_t pos) noexcept(false)
    {
        if (pos >= SIZE)
        {
            throw std::out_of_range("Index out of range");
        }
        return buffer_[pos];
    }

    [[nodiscard]] const T& at(std::size_t pos) const noexcept(false)
    {
        if (pos >= SIZE)
        {
            throw std::out_of_range("Index out of range");
        }
        return buffer_[pos];
    }

    [[nodiscard]] T& operator[](std::size_t pos)
    {
        return buffer_[pos];
    }

    [[nodiscard]] const T& operator[](std::size_t pos) const
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

    [[nodiscard]] std::size_t size() const
    {
        return SIZE;
    }

private:
    T buffer_[SIZE];

public:
    class Iterator
    {
    public:
        Iterator(T* ptr) : ptr_(ptr) {}

        Iterator& operator++()
        {
            ++ptr_;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            ++ptr_;
            return temp;
        }

        bool operator==(const Iterator& other) const
        {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const Iterator& other) const
        {
            return ptr_ != other.ptr_;
        }

        T& operator*()
        {
            return *ptr_;
        }

    private:
        T* ptr_;
    };

    class ConstIterator
    {
    public:
        ConstIterator(const T* ptr) : ptr_(ptr) {}

        ConstIterator& operator++()
        {
            ++ptr_;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator temp = *this;
            ++ptr_;
            return temp;
        }

        bool operator==(const ConstIterator& other) const
        {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const ConstIterator& other) const
        {
            return ptr_ != other.ptr_;
        }

        const T& operator*() const
        {
            return *ptr_;
        }

    private:
        const T* ptr_;
    };

    [[nodiscard]] Iterator begin()
    {
        return Iterator(buffer_);
    }

    [[nodiscard]] Iterator end()
    {
        return Iterator(buffer_ + SIZE);
    }

    [[nodiscard]] ConstIterator begin() const
    {
        return ConstIterator(buffer_);
    }

    [[nodiscard]] ConstIterator end() const
    {
        return ConstIterator(buffer_ + SIZE);
    }
};
}  // namespace container
}  // namespace erturk

#endif  // ERTURK_CONTAINER_ARRAY_H
