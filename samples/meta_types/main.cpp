
#include "../../erturk/meta_types/Computational.hpp"
#include "../../erturk/meta_types/TypeTrait.hpp"
#include <iostream>

int main()
{
    {
        std::cout << std::boolalpha;

        std::cout << "int is arithmetic: " << erturk::meta::is_arithmetic<int>::value << std::endl;

        std::cout << "double is arithmetic: " << erturk::meta::is_arithmetic<double>::value << std::endl;

        std::cout << "std::string is arithmetic: " << erturk::meta::is_arithmetic<std::string>::value << std::endl;

        std::cout << "int and unsigned int are the same: " << erturk::meta::is_same<int, unsigned int>::value
                  << std::endl;

        std::cout << "int[] is an array: " << erturk::meta::is_array<int[]>::value << std::endl;

        std::cout << "Function pointer is a function: " << erturk::meta::is_function<int (*)(int)>::value << std::endl;

        std::cout << "int* is a pointer: " << erturk::meta::is_pointer<int*>::value << std::endl;

        if (erturk::meta::enableif<true, int>::type(0) == 0)
        {
            std::cout << "Enable if example: true branch" << std::endl;
        }
    }

    // ***************************************************************************************************

    {
        std::cout << "Factorial of 5: " << erturk::computational::factorial<5>::value << std::endl;

        std::cout << "Fibonacci of 5: " << erturk::computational::fibonacci<5>::value << std::endl;

        std::cout << "GCD of 48 and 18: " << erturk::computational::gcd<48, 18>::value << std::endl;

        std::cout << "LCM of 6 and 8: " << erturk::computational::lcm<6, 8>::value << std::endl;

        std::cout << "5 * 7 = " << erturk::computational::multiplies<5, 7>::type::value << std::endl;

        std::cout << "Is 11 prime? " << erturk::computational::is_prime<11>::value << std::endl;

        std::cout << "Max type size of int and double: "
                  << erturk::meta::is_same<double, erturk::computational::max_type<int, double>::type>::value
                  << std::endl;

        std::cout << "Min type size of int and double: "
                  << erturk::meta::is_same<int, erturk::computational::min_type<int, double>::type>::value << std::endl;

        std::cout << "Length of types_sequence<int, float, double>: "
                  << erturk::computational::length<int, float, double>::value << std::endl;

        std::cout << "Power of 2^5: " << erturk::computational::power<2, 5>::value << std::endl;

        std::cout << "Is 16 a power of two? " << erturk::computational::is_power_of_two<16>::value << std::endl;

        std::cout << "Does the sequence 1, 2, 3, 4 contain 3? " << erturk::computational::contains<3, 1, 2, 3, 4>::value
                  << std::endl;

        std::cout << "Is 4 even? " << erturk::computational::is_even<4>::value << std::endl;

        std::cout << "Is 5 odd? " << erturk::computational::is_odd<5>::value << std::endl;

        std::cout << "Square root of 16: " << erturk::computational::sqrt<16>::value << std::endl;

        std::cout << "Sum of 1, 2, 3, 4: " << erturk::computational::sum<1, 2, 3, 4>::value << std::endl;
    }

    return 0;
}
