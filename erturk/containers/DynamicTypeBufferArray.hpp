#ifndef ERTURK_DYNAMIC_ARRAY_H
#define ERTURK_DYNAMIC_ARRAY_H

#include "../allocator/AlignedSystemAllocator.hpp"
#include "../memory/TypeBufferMemory.hpp"
#include "../meta_types/TypeTrait.hpp"
#include <stdexcept>

namespace erturk::container
{

// For simplicity:
// - No SSO
// - No COW

template <typename T, typename Allocator = erturk::allocator::AlignedSystemAllocator<T, alignof(T)>>
class DynamicTypeBufferArray final
{
    static_assert((erturk::meta::is_copy_constructible<T>::value || erturk::meta::is_move_constructible<T>::value),
                  "T must be copy constructible or move constructible!");

private:
    static constexpr size_t DEFAULT_CAPACITY_ = 2;
    static constexpr unsigned char DEFAULT_MUL_ = 2;

public:
    explicit DynamicTypeBufferArray() noexcept(false)
        : capacity_{DEFAULT_CAPACITY_}, size_{0}, typeBufferArrayPtr_{Allocator::allocate(capacity_)}
    {
        if (typeBufferArrayPtr_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }
    }

    explicit DynamicTypeBufferArray(const T& tVal, const size_t cap = DEFAULT_CAPACITY_) noexcept(false)
        : capacity_{cap}, size_{0}, typeBufferArrayPtr_{Allocator::allocate(capacity_)}
    {
        if (typeBufferArrayPtr_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }

        // instantiate type buffers with copy constructor
        erturk::type_buffer_memory::emplace_type_buffers_n(typeBufferArrayPtr_, capacity_, tVal,
                                                           erturk::type_buffer_memory::InstantiatePolicy::Copy);
    }

    DynamicTypeBufferArray(const DynamicTypeBufferArray& other) noexcept(false)
        : capacity_(other.capacity_), size_(other.size_), typeBufferArrayPtr_(Allocator::allocate(capacity_))
    {
        if (typeBufferArrayPtr_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }
        // Copy elements from other ptrToBuffer_ to other ptrToBuffer_ + size into ptrToBuffer_
        erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(
            other.typeBufferArrayPtr_, other.typeBufferArrayPtr_ + other.size_, typeBufferArrayPtr_,
            erturk::type_buffer_memory::InstantiatePolicy::Copy);
    }

    // Take over ownership
    DynamicTypeBufferArray(DynamicTypeBufferArray&& other) noexcept
        : capacity_(other.capacity_), size_(other.size_), typeBufferArrayPtr_(other.typeBufferArrayPtr_)
    {
        other.typeBufferArrayPtr_ = nullptr;
        other.size_ = 0;
        other.capacity_ = DEFAULT_CAPACITY_;
    }

    ~DynamicTypeBufferArray()
    {
        clear();
        Allocator::deallocate(typeBufferArrayPtr_);
        typeBufferArrayPtr_ = nullptr;
    }

    DynamicTypeBufferArray& operator=(const DynamicTypeBufferArray& other) noexcept(false)
    {
        static_assert(erturk::meta::is_copy_constructible<T>::value, "T must be copy constructible!");

        if (this != &other)
        {
            clear();
            Allocator::deallocate(typeBufferArrayPtr_);
            typeBufferArrayPtr_ = nullptr;

            size_ = other.size_;
            capacity_ = other.capacity_;

            // apply deep element copy
            typeBufferArrayPtr_ = Allocator::allocate(capacity_);

            if (typeBufferArrayPtr_ == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            // Copy elements from (other ptrToBuffer_ to other ptrToBuffer_ + size) into ptrToBuffer_
            erturk::type_buffer_memory::emplace_type_buffers_copy(
                other.typeBufferArrayPtr_, other.typeBufferArrayPtr_ + other.size_, typeBufferArrayPtr_,
                erturk::type_buffer_memory::InstantiatePolicy::Copy);
        }
        return *this;
    }

    // Take over ownership
    DynamicTypeBufferArray& operator=(DynamicTypeBufferArray&& other) noexcept
    {
        clear();
        Allocator::deallocate(typeBufferArrayPtr_);

        size_ = other.size_;
        capacity_ = other.capacity_;
        typeBufferArrayPtr_ = other.typeBufferArrayPtr_;

        other.size_ = 0;
        other.capacity_ = DEFAULT_CAPACITY_;
        other.typeBufferArrayPtr_ = nullptr;
        return *this;
    }

    void push_back(const T& tVal)
    {
        static_assert(erturk::meta::is_copy_constructible<T>::value, "T must be copy constructible!");

        if (size_ >= capacity_)
        {
            expand_allocation(capacity_ * DEFAULT_MUL_);
        }
        erturk::type_buffer_memory::construct_at(typeBufferArrayPtr_ + size_++, tVal);
    }

    void push_back(T&& tVal)
    {
        static_assert(erturk::meta::is_move_constructible<T>::value, "T must be move constructible!");

        if (size_ >= capacity_)
        {
            expand_allocation(capacity_ * DEFAULT_MUL_);
        }
        erturk::type_buffer_memory::construct_at(typeBufferArrayPtr_ + size_++, std::move(tVal));
    }

    template <typename... Args>
    void emplace_back(Args&&... args)
    {
        if (size_ >= capacity_)
        {
            expand_allocation(capacity_ * DEFAULT_MUL_);
        }
        erturk::type_buffer_memory::construct_at(typeBufferArrayPtr_ + size_++, std::forward<Args>(args)...);
    }

    void reserve(size_t new_capacity)
    {
        expand_allocation(new_capacity);
    }

    [[nodiscard]] T& operator[](const size_t index) noexcept(false)
    {
        if (index > size_)
        {
            throw std::runtime_error("Invalid Index!");
        }
        return typeBufferArrayPtr_[index];
    }

    [[nodiscard]] const T& operator[](const size_t index) const noexcept(false)
    {
        if (index > size_)
        {
            throw std::runtime_error("Invalid Index!");
        }
        return typeBufferArrayPtr_[index];
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
            erturk::type_buffer_memory::destruct_at(typeBufferArrayPtr_ + idx);
        }
        size_ = 0;
    }

private:
    void expand_allocation(size_t new_capacity) noexcept(false)
    {
        if (new_capacity > capacity_)
        {
            T* new_buffer = Allocator::allocate(new_capacity);

            if (typeBufferArrayPtr_ == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            // Emplace from (base address + insert_index) to (base address + size) into (new_buffer + insert_index + 1)
            erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(typeBufferArrayPtr_,
                                                                         typeBufferArrayPtr_ + size_, new_buffer);

            for (size_t idx = 0; idx < size_; idx++)
            {
                erturk::type_buffer_memory::destruct_at(typeBufferArrayPtr_ + idx);
            }
            Allocator::deallocate(typeBufferArrayPtr_);

            typeBufferArrayPtr_ = new_buffer;
            capacity_ = new_capacity;
        }
    }

private:
    size_t capacity_{0};
    size_t size_{0};
    T* typeBufferArrayPtr_{nullptr};

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

        [[nodiscard]] bool operator==(const Iterator& other) const
        {
            return t_ptr_ == other.t_ptr_;
        }

        [[nodiscard]] bool operator!=(const Iterator& other) const
        {
            return t_ptr_ != other.t_ptr_;
        }

        [[nodiscard]] T& operator*()
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
        return Iterator{typeBufferArrayPtr_};
    }

    [[nodiscard]] Iterator end()
    {
        return Iterator{typeBufferArrayPtr_ + size_};
    }

    [[nodiscard]] Iterator begin() const
    {
        return Iterator{typeBufferArrayPtr_};
    }

    [[nodiscard]] Iterator end() const
    {
        return Iterator{typeBufferArrayPtr_ + size_};
    }

    Iterator insert(const Iterator iterator, const T& value) noexcept(false)
    {
        if (iterator.get() < typeBufferArrayPtr_ || iterator.get() > typeBufferArrayPtr_ + size_)
        {
            return Iterator{nullptr};
        }

        size_t insert_index = iterator.get() - typeBufferArrayPtr_;

        if (size_ >= capacity_)
        {
            size_t new_capacity = capacity_ * DEFAULT_MUL_;
            T* new_buffer = Allocator::allocate(new_capacity);

            if (new_buffer == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            // Emplace from base address to base address + insert_index into new buffer
            erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(
                typeBufferArrayPtr_, typeBufferArrayPtr_ + insert_index, new_buffer);

            // Emplace insert value
            erturk::type_buffer_memory::construct_at(new_buffer + insert_index, value);

            // Emplace from (base address + insert_index) to (base address + size) into (new_buffer + insert_index + 1)
            erturk::type_buffer_memory::emplace_type_buffers_copy<T, T*>(
                typeBufferArrayPtr_ + insert_index, typeBufferArrayPtr_ + size_, new_buffer + insert_index + 1);

            for (size_t idx = 0; idx < size_; idx++)
            {
                erturk::type_buffer_memory::destruct_at(typeBufferArrayPtr_ + idx);
            }
            Allocator::deallocate(typeBufferArrayPtr_);

            typeBufferArrayPtr_ = new_buffer;
            capacity_ = new_capacity;
        }
        else
        {
            // Shift elements from right to left that make space for the new element
            size_t idx = size_;
            while (idx > insert_index)
            {
                // Emplace T from (ptrToBuffer_ + idx) - 1 at address (ptrToBuffer_ + idx)
                erturk::type_buffer_memory::construct_at(typeBufferArrayPtr_ + idx,
                                                         std::move(typeBufferArrayPtr_[idx - 1]));
                erturk::type_buffer_memory::destruct_at(typeBufferArrayPtr_ + idx - 1);
                idx--;
            }

            erturk::type_buffer_memory::construct_at(typeBufferArrayPtr_ + insert_index, value);
        }
        size_++;
        return Iterator{typeBufferArrayPtr_ + insert_index};  // return inserted position iterator
    }

    Iterator erase(const Iterator iterator)
    {
        if (iterator.get() < typeBufferArrayPtr_ || iterator.get() >= typeBufferArrayPtr_ + size_)
        {
            return Iterator{nullptr};
        }

        size_t erase_index = iterator.get() - typeBufferArrayPtr_;

        erturk::type_buffer_memory::destruct_at(typeBufferArrayPtr_ + erase_index);

        for (size_t idx = erase_index; idx < size_ - 1; idx++)
        {
            erturk::type_buffer_memory::construct_at(typeBufferArrayPtr_ + idx,
                                                     std::move(typeBufferArrayPtr_[idx + 1]));
            erturk::type_buffer_memory::destruct_at(typeBufferArrayPtr_ + idx + 1);
        }

        size_--;
        return Iterator{typeBufferArrayPtr_ + erase_index};
    }
};
}  // namespace erturk::container
#endif  // ERTURK_DYNAMIC_ARRAY_H
