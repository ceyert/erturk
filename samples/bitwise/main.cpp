
#include "../../erturk/bitwise/Bitmap.hpp"
#include <iostream>


int main()
{
    erturk::bitwise::Bitmap<32> bitmap{};

   // Set some bits
    bitmap.set(0);   // Set the 0th bit
    bitmap.set(5);   // Set the 5th bit
    bitmap.set(31);  // Set the last bit in bitmap (31st bit)

    // Test bits
    std::cout << "Bit 0 is " << (bitmap.test(0) ? "set" : "clear") << std::endl;
    std::cout << "Bit 1 is " << (bitmap.test(1) ? "set" : "clear") << std::endl;
    std::cout << "Bit 5 is " << (bitmap.test(5) ? "set" : "clear") << std::endl;

    // Number of set bits
    std::cout << "Number of set bits: " << bitmap.count() << std::endl;

    // Flip a bit
    bitmap.flip(5);
    std::cout << "Bit 5 is " << (bitmap.test(5) ? "set" : "clear") << " after flip" << std::endl;

    // Clear a bit
    bitmap.clear(0);
    std::cout << "Bit 0 is " << (bitmap.test(0) ? "set" : "clear") << " after clear" << std::endl;

    // Number of set bits
    std::cout << "Final number of set bits: " << bitmap.count() << std::endl;

    return 0;
}