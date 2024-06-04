#ifndef ERTURK_STRING_H
#define ERTURK_STRING_H

#include "../allocator/AlignedSystemAllocator.hpp"
#include "../memory/Memory.hpp"
#include "../memory/TypeBufferMemory.hpp"
#include "../meta_types/TypeTrait.hpp"
#include <stdexcept>

namespace erturk::container
{

// For simplicity:
// - No SSO
// - No COW
template <typename CharT, typename Allocator = erturk::allocator::AlignedSystemAllocator<CharT, alignof(CharT)>>
class BaseString final
{
    static_assert((erturk::meta::is_same<CharT, char>::value || erturk::meta::is_same<CharT, char8_t>::value
                   || erturk::meta::is_same<CharT, wchar_t>::value || erturk::meta::is_same<CharT, char16_t>::value
                   || erturk::meta::is_same<CharT, char32_t>::value),
                  "T must be a char type!");

private:
    static constexpr size_t TERMINATOR_ = 1;
    static constexpr size_t DEFAULT_CAPACITY_ = TERMINATOR_;
    static constexpr size_t DEFAULT_MULTIPLICATION = 2;

public:
    static constexpr size_t NPOS = -1;

public:
    class Iterator
    {
    public:
        explicit Iterator(CharT* ptr) : char_ptr_{ptr} {}

        Iterator& operator++()
        {
            char_ptr_++;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            char_ptr_++;
            return temp;
        }

        [[nodiscard]] bool operator==(const Iterator& other) const
        {
            return char_ptr_ == other.get();
        }

        [[nodiscard]] bool operator!=(const Iterator& other) const
        {
            return char_ptr_ != other.get();
        }

        [[nodiscard]] CharT& operator*()
        {
            return *char_ptr_;
        }

        [[nodiscard]] CharT* operator->()
        {
            return char_ptr_;
        }

        [[nodiscard]] const CharT* operator->() const
        {
            return char_ptr_;
        }

        [[nodiscard]] CharT* get() const
        {
            return char_ptr_;
        }

    private:
        CharT* char_ptr_;
    };

    explicit BaseString() noexcept(false)
        : capacity_{DEFAULT_CAPACITY_}, length_{0}, charBufferPtr_{Allocator::allocate(capacity_)}
    {
        if (charBufferPtr_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }

        charBufferPtr_[size()] = '\0';  // null-terminator at index length_
    }

    // Instantiate from string literal
    explicit BaseString(const char* c_string) noexcept(false) : capacity_{0}, length_{0}, charBufferPtr_{nullptr}
    {
        if (c_string != nullptr)
        {
            // Num of string literal chars, excluding the null-terminator.
            size_t literal_len = std::strlen(c_string);

            length_ = literal_len;

            capacity_ = size() + TERMINATOR_;

            charBufferPtr_ = Allocator::allocate(capacity_);

            if (charBufferPtr_ == nullptr)
            {
                throw std::runtime_error("Failed to allocate memory!");
            }

            erturk::memory::memcpy(c_string, data(), size());

            charBufferPtr_[size()] = '\0';  // null-terminator at index length
        }
    }

    BaseString(const BaseString& other) noexcept(false)
        : capacity_{other.capacity()}, length_{other.size()}, charBufferPtr_{Allocator::allocate(capacity_)}
    {
        if (charBufferPtr_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }

        // src_base, src_end, target_base
        erturk::memory::memcpy<CharT>(other.data(), other.data() + other.size(), data());

        charBufferPtr_[size()] = '\0';  // null-terminator at index length_
    }

    // Take over ownership
    BaseString(BaseString&& other) noexcept(false)
        : capacity_{other.capacity()}, length_{other.size()}, charBufferPtr_{Allocator::allocate(capacity_)}
    {
        if (charBufferPtr_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }

        // src_base, src_end, target_base
        erturk::memory::memcpy<CharT>(other.data(), other.data() + other.size(), data());

        charBufferPtr_[size()] = '\0';  // null-terminator at index length_

        other.clear();
    }

    ~BaseString()
    {
        clear();
        Allocator::deallocate(data());
        charBufferPtr_ = nullptr;
    }

    BaseString& operator=(const BaseString& other) noexcept(false)
    {
        if (this != &other)
        {
            clear();

            size_t required_size = other.size() + TERMINATOR_;

            if (required_size >= capacity())
            {
                expand_allocation(required_size);
            }

            length_ = other.size();

            // src_base, src_end, target_base
            erturk::memory::memcpy<CharT>(other.data(), other.data() + other.size(), data());

            charBufferPtr_[size()] = '\0';  // null-terminator at index length_
        }
        return *this;
    }

    // Take over ownership
    BaseString& operator=(BaseString&& other) noexcept
    {
        if (this != &other)
        {
            clear();

            size_t required_size = other.size() + TERMINATOR_;

            if (required_size >= capacity())
            {
                expand_allocation(required_size);
            }

            length_ = other.size();

            // src_base, src_end, target_base
            erturk::memory::memcpy<CharT>(other.data(), other.data() + other.size(), data());

            charBufferPtr_[size()] = '\0';  // null-terminator at index length_

            other.clear();
        }
        return *this;
    }

    BaseString& operator=(const char* c_string) noexcept(false)
    {
        if (c_string == nullptr)
        {
            throw std::runtime_error("Cannot assign null string literal!");
        }

        clear();

        size_t literal_len = std::strlen(c_string);

        size_t required_size = literal_len + TERMINATOR_;

        if (required_size >= capacity())
        {
            expand_allocation(required_size);
        }

        length_ = literal_len;

        erturk::memory::memcpy_n<CharT>(c_string, size(), data());

        charBufferPtr_[size()] = '\0';  // null-terminator at index length_

        return *this;
    }

    void push_back(const CharT& ch)
    {
        size_t required_capacity = size() + TERMINATOR_;

        if (required_capacity >= capacity())
        {
            expand_allocation(required_capacity);
        }

        charBufferPtr_[size()] = ch;  // override null-terminator with value

        length_++;

        charBufferPtr_[size()] = '\0';  // add null-terminator at index length_
    }

    [[nodiscard]] CharT pop_back()
    {
        CharT ch = charBufferPtr_[size() - TERMINATOR_];  // get char before null-terminator

        charBufferPtr_[size() - TERMINATOR_] = '\0';  // add null-terminator at index length_ - 1

        length_--;

        return ch;
    }

    void append(const CharT& ch)
    {
        size_t required_capacity = size() + TERMINATOR_;

        if (required_capacity >= capacity())
        {
            expand_allocation(required_capacity);
        }

        charBufferPtr_[size()] = ch;  // override null-terminator with ch

        length_++;

        charBufferPtr_[size()] = '\0';  // null-terminator at index length_
    }

    void append(const BaseString& other)
    {
        size_t required_capacity = size() + other.size() + TERMINATOR_;

        if (required_capacity >= capacity())
        {
            expand_allocation(required_capacity);
        }

        // src_base, src_end, target_base
        erturk::memory::memcpy<CharT>(other.data(), other.data() + other.size(), data() + size());

        length_ += other.size();

        charBufferPtr_[size()] = '\0';
    }

    void append(const char* c_string) noexcept(false)
    {
        if (c_string == nullptr)
        {
            throw std::runtime_error("Cannot append null string literal!");
        }

        size_t literal_len = std::strlen(c_string);

        size_t required_capacity = literal_len + size() + TERMINATOR_;

        if (required_capacity >= capacity())
        {
            expand_allocation(required_capacity);
        }

        // src_base, src_end, target_base
        erturk::memory::memcpy<CharT>(c_string, c_string + literal_len, data() + size());

        length_ += literal_len;

        charBufferPtr_[size()] = '\0';  // null-terminator at index size_
    }

    void reserve(const size_t new_capacity)
    {
        if (new_capacity > capacity())
        {
            expand_allocation(new_capacity);
        }
    }

    [[nodiscard]] const CharT& front() const
    {
        if (size() == 0)
        {
            throw std::out_of_range("String is empty");
        }
        return charBufferPtr_[0];
    }

    [[nodiscard]] const CharT& back() const
    {
        if (size() == 0)
        {
            throw std::out_of_range("String is empty");
        }
        return charBufferPtr_[size() - TERMINATOR_];
    }

    [[nodiscard]] CharT& operator[](const size_t index) noexcept(false)
    {
        if (index > size() - TERMINATOR_)
        {
            throw std::runtime_error("Out of bounds index!");
        }
        return charBufferPtr_[index];
    }

    [[nodiscard]] const CharT& operator[](const size_t index) const noexcept(false)
    {
        if (index > size() - TERMINATOR_)
        {
            throw std::runtime_error("Out of bounds index!");
        }
        return charBufferPtr_[index];
    }

    [[nodiscard]] CharT& at(const size_t index) noexcept(false)
    {
        if (index > size() - TERMINATOR_)
        {
            throw std::runtime_error("Out of bounds index!");
        }
        return charBufferPtr_[index];
    }

    [[nodiscard]] const CharT& at(const size_t index) const noexcept(false)
    {
        if (index > size() - TERMINATOR_)
        {
            throw std::runtime_error("Out of bounds index!");
        }
        return charBufferPtr_[index];
    }

    [[nodiscard]] const CharT* c_str() const
    {
        return charBufferPtr_ != nullptr ? charBufferPtr_ : "";
    }

    [[nodiscard]] CharT* data() const
    {
        return charBufferPtr_;
    }

    [[nodiscard]] CharT* data()
    {
        return charBufferPtr_;
    }

    [[nodiscard]] size_t size() const
    {
        return length_;
    }

    [[nodiscard]] size_t capacity() const
    {
        return capacity_;
    }

    void clear() noexcept
    {
        if (charBufferPtr_ != nullptr)
        {
            erturk::memory::memset_n(data(), size(), '\0');
        }

        length_ = 0;
    }

    [[nodiscard]] BaseString substr(size_t start_idx, size_t length) const noexcept(false)
    {
        if (start_idx >= size() - TERMINATOR_)
        {
            throw std::out_of_range("Starting position is out of bounds");
        }

        if (start_idx + length > size())
        {
            length = size() - start_idx;  // Shrink length
        }

        BaseString result{};

        size_t idx = 0;
        while (idx < length)
        {
            result.push_back(charBufferPtr_[start_idx + idx]);
            idx++;
        }

        return result;
    }

    [[nodiscard]] size_t find_first(const BaseString& from_str, const size_t index = 0) const
    {
        if (from_str.size() == 0 || index >= size())
        {
            return BaseString::NPOS;
        }

        if (from_str.size() == 0 && from_str.size() > size())
        {
            return BaseString::NPOS;
        }

        Window window{};

        window.baseIndex_ = index;
        window.endIndex_ = index;

        size_t patternSize = from_str.size();

        while (window.endIndex_ < size())
        {
            // Expand window to size of pattern
            if ((window.endIndex_ - window.baseIndex_ + 1) != patternSize)
            {
                window.endIndex_++;
                continue;
            }

            bool matchFound = true;

            // Match a current window with the pattern
            size_t idx = 0;
            while (idx < patternSize)
            {
                if (from_str.operator[](idx) == operator[](idx + window.baseIndex_))
                {
                    idx++;
                    continue;
                }
                matchFound = false;
                break;
            }

            if (!matchFound)
            {
                // Move window forward by one
                window.baseIndex_++;

                // Shrink window to base
                window.endIndex_ = window.baseIndex_;
                continue;
            }
            return window.baseIndex_;  // return base index of matched pattern
        }
        return BaseString::NPOS;
    }

    [[nodiscard]] size_t find_first(const char* from_c_string, const size_t index = 0) const
    {
        if (index >= size())
        {
            return BaseString::NPOS;
        }

        size_t literal_len = std::strlen(from_c_string);

        if (literal_len == 0 && literal_len > size())
        {
            return BaseString::NPOS;
        }

        Window window{};

        size_t patternSize = literal_len;

        window.baseIndex_ = index;
        window.endIndex_ = index;

        while (window.endIndex_ < size())
        {
            // Expand window to size of pattern
            if ((window.endIndex_ - window.baseIndex_ + 1) != patternSize)
            {
                window.endIndex_++;
                continue;
            }

            bool matchFound = true;

            // Match a current window with the pattern
            size_t idx = 0;
            while (idx < patternSize)
            {
                if (from_c_string[idx] == operator[](idx + window.baseIndex_))
                {
                    idx++;
                    continue;
                }
                matchFound = false;
                break;
            }

            if (!matchFound)
            {
                // Move window forward by one
                window.baseIndex_++;

                // Shrink window to base
                window.endIndex_ = window.baseIndex_;
                continue;
            }
            return window.baseIndex_;  // return base index of the matched pattern
        }
        return BaseString::NPOS;
    }

    [[nodiscard]] size_t find_first(const CharT ch, const size_t index = 0) const
    {
        if (index > size() - TERMINATOR_)
        {
            return BaseString::NPOS;
        }

        size_t idx = index;

        while (idx < size())
        {
            if (charBufferPtr_[idx] == ch)
            {
                return idx;
            }
            idx++;
        }

        return BaseString::NPOS;
    }

    [[nodiscard]] bool contains(const BaseString& str) const
    {
        return find_first(str) != BaseString::NPOS;
    }

    [[nodiscard]] bool contains(const char* c_string) const
    {
        return find_first(c_string) != BaseString::NPOS;
    }

    [[nodiscard]] bool contains(CharT ch) const
    {
        return find_first(ch) != BaseString::NPOS;
    }

    // base pointer
    [[nodiscard]] Iterator begin()
    {
        return Iterator{charBufferPtr_};
    }

    // end pointer
    [[nodiscard]] Iterator end()
    {
        return Iterator{charBufferPtr_ + size()};
    }

    // base pointer
    [[nodiscard]] Iterator begin() const
    {
        return Iterator{data()};
    }

    // end pointer
    [[nodiscard]] Iterator end() const
    {
        return Iterator{data() + size()};
    }

private:
    void expand_allocation(size_t new_capacity, size_t times = DEFAULT_MULTIPLICATION) noexcept(false)
    {
        CharT* new_buffer = Allocator::allocate(new_capacity * times);

        if (charBufferPtr_ == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory!");
        }

        // src_base, src_end, target_base
        erturk::memory::memcpy<CharT>(data(), data() + size(), new_buffer);

        Allocator::deallocate(data());

        charBufferPtr_ = new_buffer;
        capacity_ = new_capacity;
    }

private:
    size_t capacity_{0};  // length of buffer with terminator
    size_t length_{0};    // length of the char array without terminator
    CharT* charBufferPtr_{nullptr};

private:
    struct Window
    {
        size_t baseIndex_{0};
        size_t endIndex_{0};
    };
};

using String = BaseString<char>;

}  // namespace erturk::container

#endif  // ERTURK_STRING_H
