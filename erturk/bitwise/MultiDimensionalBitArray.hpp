#ifndef ERTURK_MD_BITARRAY_H
#define ERTURK_MD_BITARRAY_H

#include "../../erturk/bitwise/BitArray.hpp"

namespace erturk
{
namespace bitwise
{

template <const size_t BIT_SIZE, const size_t SIZE>
class MultiDimensionalBitArray {
private:
    erturk::container::Array<erturk::bitwise::BitArray<BIT_SIZE>, SIZE> arrays_{};

public:
    MultiDimensionalBitArray() = default;
    ~MultiDimensionalBitArray() = default;

    // Set a bit to 1
    void set(size_t arrays_idx, size_t bit_idx) 
    {
        if (arrays_idx < SIZE && bit_idx < BIT_SIZE) 
        {
            arrays_[arrays_idx].set(bit_idx);
        }
    }

    // Clear a bit to 0
    void clear(size_t arrays_idx, size_t bit_idx) 
    {
        if (arrays_idx < SIZE && bit_idx < BIT_SIZE) 
        {
            arrays_[arrays_idx].clear(bit_idx);
        }
    }

    // Flip a bit (0->1, 1->0)
    void flip(size_t arrays_idx, size_t bit_idx) 
    {
        if (arrays_idx < SIZE && bit_idx < BIT_SIZE) 
        {
            arrays_[arrays_idx].flip(bit_idx);
        }
    }

    // Check if a bit is set (1)
    bool test(size_t arrays_idx, size_t bit_idx) const 
    {
        if (arrays_idx < SIZE && bit_idx < BIT_SIZE) 
        {
            return arrays_[arrays_idx].test(bit_idx);
        }
        return false;
    }

    // Resets all bits in all BitArrays
    void reset() 
    {
        for (erturk::bitwise::BitArray<BIT_SIZE>& bitArray : arrays_) 
        {
            bitArray.reset();
        }
    }

    // Checks if all bits are set to 1
    bool all() const 
    {
        for (const erturk::bitwise::BitArray<BIT_SIZE>& bitArray : arrays_) 
        {
            if (!bitArray.all()) 
            {
                return false;
            }
        }
        return true;
    }

    // Checks if all bits are set to 0
    bool none() const 
    {
        for (const erturk::bitwise::BitArray<BIT_SIZE>& bitArray : arrays_) 
        {
            if (!bitArray.none()) 
            {
                return false;
            }
        }
        return true;
    }

    // Count the total number of set bits in all BitArrays
    size_t count() const 
    {
        size_t total_count = 0;
        for (const erturk::bitwise::BitArray<BIT_SIZE>& bitArray : arrays_) 
        {
            total_count += bitArray.count();
        }
        return total_count;
    }
};


}  // namespace bitwise
}  // namespace erturk

#endif  // ERTURK_MD_BITARRAY_H
