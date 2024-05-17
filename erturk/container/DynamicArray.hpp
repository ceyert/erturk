#ifndef ERTURK_DYNAMIC_ARRAY_H
#define ERTURK_DYNAMIC_ARRAY_H

#include "../allocator/AlignedSystemAllocator.hpp"
#include "../memory/TypeBufferMemory.hpp"
#include "../meta_types/TypeTrait.hpp"
#include "TypeBufferArray.hpp"
#include <stdexcept>

namespace erturk::container
{

template <typename T, typename Allocator = erturk::allocator::AlignedSystemAllocator<T, alignof(T)>>
class DynamicArray final
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

private:
    static constexpr size_t DEFAULT_CAP_ = 2;
    static constexpr unsigned char DEFAULT_MUL_ = 2;

public:
    explicit DynamicArray() noexcept(false)
        : capacity_{DEFAULT_CAP_}, size_{0}, ptrToBuffer_{Allocator::allocate(capacity_)}
    {
        if (ptrToBuffer_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }
    }

    explicit DynamicArray(const T& value, size_t cap = DEFAULT_CAP_) noexcept(false)
        : capacity_{cap}, size_{0}, ptrToBuffer_{Allocator::allocate(capacity_)}
    {
        if (ptrToBuffer_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }

        erturk::type_buffer_memory::emplace_type_buffers(ptrToBuffer_, capacity_, value);
    }

    DynamicArray(const DynamicArray& other) noexcept(false)
        : capacity_(other.capacity_), size_(other.size_), ptrToBuffer_(Allocator::allocate(capacity_))
    {
        if (ptrToBuffer_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }
        erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(other.ptrToBuffer_,
                                                                     other.ptrToBuffer_ + other.size_, ptrToBuffer_);
    }

    DynamicArray(DynamicArray&& other) noexcept
        : capacity_(other.capacity_), size_(other.size_), ptrToBuffer_(other.ptrToBuffer_)
    {
        other.ptrToBuffer_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~DynamicArray()
    {
        clear();
        Allocator::deallocate(ptrToBuffer_);
    }

    DynamicArray& operator=(const DynamicArray& other) noexcept(false)
    {
        if (this != &other)
        {
            size_ = other.size_;
            capacity_ = other.capacity_;

            // apply deep element copy
            ptrToBuffer_ = Allocator::allocate(capacity_);

            if (ptrToBuffer_ == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            erturk::type_buffer_memory::emplace_type_buffers_copy(other.ptrToBuffer_, other.ptrToBuffer_ + other.size_,
                                                                  ptrToBuffer_);
        }
        return *this;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept
    {
        size_ = other.size_;
        capacity_ = other.capacity_;
        ptrToBuffer_ = other.ptrToBuffer_;

        other.size_ = 0;
        other.capacity_ = 0;
        other.ptrToBuffer_ = nullptr;  // resource ownership moved
        return *this;
    }

    void push_back(const T& value)
    {
        if (size_ >= capacity_)
        {
            reserve(capacity_ * DEFAULT_MUL_);
        }
        Allocator::construct(ptrToBuffer_ + size_++, value);
    }

    void push_back(T&& value)
    {
        if (size_ >= capacity_)
        {
            reserve(capacity_ * DEFAULT_MUL_);
        }
        Allocator::construct(ptrToBuffer_ + size_++, std::move(value));
    }

    template <typename... Args>
    void emplace_back(Args&&... args)
    {
        if (size_ >= capacity_)
        {
            reserve(capacity_ * DEFAULT_MUL_);
        }
        Allocator::construct(ptrToBuffer_ + size_++, std::forward<Args>(args)...);
    }

    void reserve(size_t new_capacity) noexcept(false)
    {
        if (new_capacity > capacity_)
        {
            T* new_buffer = Allocator::allocate(new_capacity);

            if (ptrToBuffer_ == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(ptrToBuffer_, ptrToBuffer_ + size_,
                                                                         new_buffer);

            for (size_t idx = 0; idx < size_; idx++)
            {
                Allocator::destroy(ptrToBuffer_ + idx);
            }
            Allocator::deallocate(ptrToBuffer_);

            ptrToBuffer_ = new_buffer;
            capacity_ = new_capacity;
        }
    }

    [[nodiscard]] T& operator[](const size_t index)
    {
        return ptrToBuffer_[index];
    }

    [[nodiscard]] const T& operator[](const size_t index) const
    {
        return ptrToBuffer_[index];
    }

    [[nodiscard]] size_t size() const
    {
        return size_;
    }

    [[nodiscard]] size_t capacity() const
    {
        return capacity_;
    }

    void clear() noexcept
    {
        for (size_t idx = 0; idx < size_; idx++)
        {
            Allocator::destroy(ptrToBuffer_ + idx);
        }
        size_ = 0;
    }

private:
    size_t capacity_{0};
    size_t size_{0};
    T* ptrToBuffer_{nullptr};

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

        [[nodiscard]] T* get() const
        {
            return t_ptr_;
        }

    private:
        T* t_ptr_;
    };

    [[nodiscard]] Iterator begin()
    {
        return Iterator{ptrToBuffer_};
    }

    [[nodiscard]] Iterator end()
    {
        return Iterator{ptrToBuffer_ + size_};
    }

    [[nodiscard]] Iterator begin() const
    {
        return Iterator{ptrToBuffer_};
    }

    [[nodiscard]] Iterator end() const
    {
        return Iterator{ptrToBuffer_ + size_};
    }

    Iterator insert(const Iterator iterator, const T& value) noexcept(false)
    {
        if (iterator.get() < ptrToBuffer_ || iterator.get() > ptrToBuffer_ + size_)
        {
            return Iterator{nullptr};
        }

        size_t insert_index = iterator.get() - ptrToBuffer_;

        if (size_ >= capacity_)
        {
            size_t new_capacity = capacity_ * DEFAULT_MUL_;
            T* new_buffer = Allocator::allocate(new_capacity);

            if (new_buffer == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            // Emplace from base address to base address + insert_index into new buffer
            erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(ptrToBuffer_, ptrToBuffer_ + insert_index,
                                                                         new_buffer);

            // Emplace insert value
            Allocator::construct(new_buffer + insert_index, value);

            // Emplace from (base address + insert_index) to (base address + size) into (new_buffer + insert_index + 1)
            erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(
                ptrToBuffer_ + insert_index, ptrToBuffer_ + size_, new_buffer + insert_index + 1);

            for (size_t idx = 0; idx < size_; idx++)
            {
                Allocator::destroy(ptrToBuffer_ + idx);
            }
            Allocator::deallocate(ptrToBuffer_);

            ptrToBuffer_ = new_buffer;
            capacity_ = new_capacity;
        }
        else
        {
            // Shift elements from right to left that make space for the new element
            size_t idx = size_;
            while (idx > insert_index)
            {
                // Emplace T from (ptrToBuffer_ + idx) - 1 at address (ptrToBuffer_ + idx)
                Allocator::construct(ptrToBuffer_ + idx, std::move(ptrToBuffer_[idx - 1]));
                Allocator::destroy(ptrToBuffer_ + idx - 1);
                idx--;
            }

            Allocator::construct(ptrToBuffer_ + insert_index, value);
        }
        size_++;
        return Iterator{ptrToBuffer_ + insert_index};  // return inserted position iterator
    }

    Iterator erase(const Iterator iterator)
    {
        if (iterator.get() < ptrToBuffer_ || iterator.get() >= ptrToBuffer_ + size_)
        {
            return Iterator{nullptr};
        }

        size_t erase_index = iterator.get() - ptrToBuffer_;

        Allocator::destroy(ptrToBuffer_ + erase_index);

        for (size_t idx = erase_index; idx < size_ - 1; idx++)
        {
            Allocator::construct(ptrToBuffer_ + idx, std::move(ptrToBuffer_[idx + 1]));
            Allocator::destroy(ptrToBuffer_ + idx + 1);
        }

        size_--;
        return Iterator{ptrToBuffer_ + erase_index};
    }
};
}  // namespace erturk::container
#endif  // ERTURK_DYNAMIC_ARRAY_H
