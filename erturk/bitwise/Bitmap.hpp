#ifndef ERTURK_BITMAP_H
#define ERTURK_BITMAP_H

#include "../../erturk/container/Array.hpp"

namespace erturk
{
namespace bitwise
{

template <size_t BIT_IDX>
class Bitmap
{
private:
    static constexpr size_t CHAR_SIZE_ = 8;

    static constexpr size_t BUFFER_SIZE_ = (BIT_IDX + CHAR_SIZE_ - 1) / CHAR_SIZE_;

    erturk::container::Array<unsigned char, BUFFER_SIZE_> buffer_{};

public:
    Bitmap() = default;

    ~Bitmap() = default;

    // Set a bit to 1
    void set(size_t index)
    {
        if (index >= BIT_IDX)
        {
            return;
        }

        buffer_[index / CHAR_SIZE_] |= (1 << (index % CHAR_SIZE_));
    }

    // Clear a bit to 0
    void clear(size_t index)
    {
        if (index >= BIT_IDX)
        {
            return;
        }

        buffer_[index / CHAR_SIZE_] &= ~(1 << (index % CHAR_SIZE_));
    }

    // Flip a bit (0->1, 1->0)
    void flip(size_t index)
    {
        if (index >= BIT_IDX)
        {
            return;
        }

        buffer_[index / CHAR_SIZE_] ^= (1 << (index % CHAR_SIZE_));
    }

    // Check if a bit is set (1)
    bool test(size_t index) const
    {
        if (index >= BIT_IDX)
        {
            return false;
        }

        return (buffer_[index / CHAR_SIZE_] & (1 << (index % CHAR_SIZE_))) != 0;
    }

    // Count the number of set bits
    size_t count() const
    {
        size_t count = 0;

        for (size_t idx = 0; idx < BIT_IDX; idx++)
        {
            if (test(idx))
            {
                count++;
            }
        }
        return count;
    }
};

}  // namespace bitwise
}  // namespace erturk

#endif  // ERTURK_BITMAP_H
