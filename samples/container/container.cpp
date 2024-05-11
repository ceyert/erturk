#include "../../erturk/container/DynamicArray.hpp"
#include <iostream>

struct Point
{
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

int main()
{
    {
        erturk::container::DynamicArray<int> vec{};

        for (int i = 0; i < 1000; i++)
        {
            vec.push_back(i);
        }

        for (int i = 0; i < vec.size(); i++)
        {
            std::cout << vec[i] << " ";
        }
        std::cout << std::endl;
    }

    {
        erturk::container::DynamicArray<Point> points;
        points.emplace_back(Point{1, 2});
        points.push_back(Point{3, 4});

        for (auto& p : points)
        {
            std::cout << "(" << p.x << ", " << p.y << ") ";
        }
        std::cout << std::endl;
    }

    {
        erturk::container::DynamicArray<double> doubles{};
        doubles.reserve(10);

        for (double d = 0.0; d < 10.0; d += 1.0)
        {
            doubles.push_back(d);
        }

        std::cout << "Capacity: " << doubles.capacity() << std::endl;
        std::cout << "Size: " << doubles.size() << std::endl;
    }

    {
        erturk::container::DynamicArray<std::string> original{};
        original.push_back("Hello");
        original.push_back("World");

        erturk::container::DynamicArray<std::string> copy = original;

        erturk::container::DynamicArray<std::string> moved = std::move(original);

        std::cout << "Copied array: ";
        for (auto& s : copy)
        {
            std::cout << s << " ";
        }
        std::cout << std::endl;

        std::cout << "Moved array: ";
        for (auto& s : moved)
        {
            std::cout << s << " ";
        }
        std::cout << std::endl;
    }

    {
        erturk::container::DynamicArray<char> characters{};
        characters.push_back('A');
        characters.push_back('C');
        characters.insert(characters.begin(), 'B');

        for (auto c : characters)
        {
            std::cout << c << " ";
        }
        std::cout << std::endl;

        characters.erase(characters.begin());  // Erase 'B'

        for (auto c : characters)
        {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
