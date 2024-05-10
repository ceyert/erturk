#include "../../erturk/allocator/AlignedSystemAllocator.hpp"
#include <iostream>
#include <list>
#include <vector>

int main()
{
    {
        erturk::allocator::AlignedSystemAllocator<int, 16> allocator{};

        int* buffer1 = allocator.allocate(200);
        std::cout << "********* buffer1 ********* \n";
        for (int i = 0; i < 200; i++)
        {
            allocator.construct(&buffer1[i], i);
            std::cout << "Addr: " << &buffer1[i] << " - value: " << buffer1[i] << "\n";
        }

        int* buffer2 = allocator.allocate(400);
        std::cout << "********* buffer2 ********* \n";
        for (int i = 0; i < 400; i++)
        {
            allocator.construct(&buffer2[i], i);
            std::cout << "Addr: " << &buffer2[i] << " - value: " << buffer2[i] << "\n";
        }

        int* buffer3 = allocator.allocate(1000);
        std::cout << "********* buffer3 ********* \n";
        for (int i = 0; i < 1000; i++)
        {
            allocator.construct(&buffer3[i], i);
            std::cout << "Addr: " << &buffer3[i] << " - value: " << buffer3[i] << "\n";
        }

        allocator.deallocate(buffer1, 200);
        allocator.deallocate(buffer2, 400);
        allocator.deallocate(buffer3, 1000);
    }

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