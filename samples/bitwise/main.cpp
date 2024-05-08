#include "../../erturk/bitwise/BitArray.hpp"
#include "../../erturk/bitwise/MultiDimensionalBitArray.hpp"
#include <iostream>

int main()
{
    {
        erturk::bitwise::BitArray<32> bitArray{};

        // Set some bits
        bitArray.set(0);   // Set the 0th bit
        bitArray.set(5);   // Set the 5th bit
        bitArray.set(31);  // Set the last bit in bitmap (31st bit)

        // Test bits
        std::cout << "Bit 0 is " << (bitArray.test(0) ? "set" : "clear") << std::endl;
        std::cout << "Bit 1 is " << (bitArray.test(1) ? "set" : "clear") << std::endl;
        std::cout << "Bit 5 is " << (bitArray.test(5) ? "set" : "clear") << std::endl;

        // Number of set bits
        std::cout << "Number of set bits: " << bitArray.count() << std::endl;

        // Flip a bit
        bitArray.flip(5);
        std::cout << "Bit 5 is " << (bitArray.test(5) ? "set" : "clear") << " after flip" << std::endl;

        // Clear a bit
        bitArray.clear(0);
        std::cout << "Bit 0 is " << (bitArray.test(0) ? "set" : "clear") << " after clear" << std::endl;

        // Check if all bits are set
        std::cout << "All bits set: " << (bitArray.all() ? "Yes" : "No") << std::endl;

        // Check if no bits are set
        std::cout << "No bits set: " << (bitArray.none() ? "Yes" : "No") << std::endl;

        // Reset the bit array
        bitArray.reset();
        std::cout << "Bits after reset: " << (bitArray.none() ? "No bits are set" : "Some bits are still set")
                  << std::endl;

        // Number of set bits
        std::cout << "Final number of set bits: " << bitArray.count() << std::endl;
    }

    {
        // 5 arrays, each array holds 32 bits
        erturk::bitwise::MultiDimensionalBitArray<32, 5> multiBitArray{};

        multiBitArray.set(0, 0);   // Set 0th bit in 1. BitArray
        multiBitArray.set(1, 5);   // Set 5th bit in 2. BitArray
        multiBitArray.set(4, 31);  // Set 31st bit in 5. BitArray

        std::cout << "BitArray 0, Bit 0 is " << (multiBitArray.test(0, 0) ? "set" : "clear") << std::endl;
        std::cout << "BitArray 1, Bit 1 is " << (multiBitArray.test(1, 1) ? "set" : "clear") << std::endl;
        std::cout << "BitArray 1, Bit 5 is " << (multiBitArray.test(1, 5) ? "set" : "clear") << std::endl;

        std::cout << "Total number of set bits: " << multiBitArray.count() << std::endl;
        std::cout << "Bits after reset: " << (multiBitArray.none() ? "No bits are set" : "Some bits are still set")
                  << std::endl;

        // Flip a bit
        multiBitArray.flip(1, 5);
        std::cout << "BitArray 1, Bit 5 is " << (multiBitArray.test(1, 5) ? "set" : "clear") << " after flip"
                  << std::endl;

        // Clear a bit
        multiBitArray.clear(0, 0);
        std::cout << "BitArray 0, Bit 0 is " << (multiBitArray.test(0, 0) ? "set" : "clear") << " after clear"
                  << std::endl;

        // Check if all bits are set
        std::cout << "All bits set: " << (multiBitArray.all() ? "Yes" : "No") << std::endl;

        // Check if no bits are set
        std::cout << "No bits set: " << (multiBitArray.none() ? "Yes" : "No") << std::endl;

        // Reset the entire bit arrays
        multiBitArray.reset();
        std::cout << "Bits after reset: " << (multiBitArray.none() ? "No bits are set" : "Some bits are still set")
                  << std::endl;
    }

    return 0;
}