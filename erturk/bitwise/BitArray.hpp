#ifndef ERTURK_BITARRAY_H
#define ERTURK_BITARRAY_H

#include <utility>
#include "../../erturk/container/Array.hpp"

namespace erturk
{
namespace bitwise
{

template <const size_t BIT_SIZE>
class BitArray
{
private:
    static constexpr size_t CHAR_SIZE_ = 8;

    static constexpr size_t BUFFER_SIZE_ = (BIT_SIZE + CHAR_SIZE_ - 1) / CHAR_SIZE_;

    erturk::container::Array<unsigned char, BUFFER_SIZE_> buffer_{};

public:
    BitArray() = default;
    
    // Copy constructor
    BitArray(const BitArray& other) : buffer_(other.buffer_) {}

    // Move constructor
    BitArray(BitArray&& other) noexcept : buffer_(std::move(other.buffer_)) {}

    // Destructor
    ~BitArray() = default;

    // Copy assignment operator
    BitArray& operator=(const BitArray& other) {
        if (this != &other) {
            buffer_ = other.buffer_;
        }
        return *this;
    }

    // Move assignment operator
    BitArray& operator=(BitArray&& other) noexcept {
        if (this != &other) {
            buffer_ = std::move(other.buffer_);
        }
        return *this;
    }

    // Set a bit to 1
    void set(size_t index)
    {
        if (index >= BIT_SIZE)
        {
            return;
        }

        buffer_[index / CHAR_SIZE_] |= (1 << (index % CHAR_SIZE_));
    }

    // Clear a bit to 0
    void clear(size_t index)
    {
        if (index >= BIT_SIZE)
        {
            return;
        }

        buffer_[index / CHAR_SIZE_] &= ~(1 << (index % CHAR_SIZE_));
    }

    // Flip a bit (0->1, 1->0)
    void flip(size_t index)
    {
        if (index >= BIT_SIZE)
        {
            return;
        }

        buffer_[index / CHAR_SIZE_] ^= (1 << (index % CHAR_SIZE_));
    }

    // Check if a bit is set (1)
    bool test(size_t index) const
    {
        if (index >= BIT_SIZE)
        {
            return false;
        }

        return (buffer_[index / CHAR_SIZE_] & (1 << (index % CHAR_SIZE_))) != 0;
    }

    // Count the number of set bits
    size_t count() const
    {
        size_t count = 0;

        for (unsigned char byte : buffer_)
        {
            while (byte)
            {
                count += byte & 1;
                byte >>= 1;
            }
        }
        return count;
    }

    // Resets all bits 
    void reset()
    {
        for (unsigned char& byte : buffer_)
        {
            byte = 0;
        } 
    }

    // Checks all bits are set to 1
    bool all() const
    {
        for (size_t i = 0; i < BUFFER_SIZE_; i++)
        {
            if (buffer_[i] != 0xFF)
            {
                return false;
            } 
        }

        return true;
    }

    // Checks all bits are set to 0
    bool none() const
    {
        for (const unsigned char& byte : buffer_)
        {
            if (byte != 0)
            {
                return false;
            } 
        }
        return true;
    }
};

}  // namespace bitwise
}  // namespace erturk

#endif  // ERTURK_BITARRAY_H
