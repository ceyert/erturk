
#include "../../erturk/allocator/AlignedSystemAllocator.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <map>

int main() {

    {
        std::vector<int, erturk::allocator::AlignedSystemAllocator<int>> my_vector(50, 42);

        for (int i = 0; i < 1000; i++) {
            my_vector.push_back(i);
        }

        for (const int& value: my_vector) {
            std::cout << "Value: " << value << " at Address: " << &value << std::endl;
        }
    }

    {
        std::list<int, erturk::allocator::AlignedSystemAllocator<int>> my_list{};

        for (int i = 0; i < 500; i++) {
            my_list.push_back(i);
        }

        for (int i = 0; i < 500; i++) {
            my_list.push_front(i);
        }

        for (const int& value: my_list) {
            std::cout << "Value: " << value << " at Address: " << &value << std::endl;
        }
    }


    {
        std::map<const int, std::string, std::less<int>, erturk::allocator::AlignedSystemAllocator<std::pair<const int, std::string>>> my_map{};

        for (int i = 0; i < 1000; i++) {
            my_map.emplace(i, std::to_string(i));
        }

        for (const auto &pair: my_map) {
            std::cout << "Key: " << pair.first << " Value: " << pair.second << " at Address: " << &pair << std::endl;
        }

    }

    return 0;
}
