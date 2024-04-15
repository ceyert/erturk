
#include "../../erturk/bitwise/BitArray.hpp"
#include <iostream>


int main()
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
    std::cout << "Bits after reset: " << (bitArray.none() ? "No bits are set" : "Some bits are still set") << std::endl;

    // Number of set bits
    std::cout << "Final number of set bits: " << bitArray.count() << std::endl;

    return 0;
}