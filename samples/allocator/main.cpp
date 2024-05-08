#include "../../erturk/allocator/AlignedSystemAllocator.hpp"
#include <iostream>
#include <list>
#include <vector>

int main()
{
    {
        std::vector<int, erturk::allocator::AlignedSystemAllocator<int>> my_vector(50, 42);

        for (int i = 0; i < 1000; i++)
        {
            my_vector.push_back(i);
        }

        for (const int& value : my_vector)
        {
            std::cout << "Value: " << value << " at Address: " << &value << std::endl;
        }
    }

    {
        std::list<int, erturk::allocator::AlignedSystemAllocator<int>> my_list{};

        for (int i = 0; i < 500; i++)
        {
            my_list.push_back(i);
        }

        for (int i = 0; i < 500; i++)
        {
            my_list.push_front(i);
        }

        for (const int& value : my_list)
        {
            std::cout << "Value: " << value << " at Address: " << &value << std::endl;
        }
    }


    return 0;
}