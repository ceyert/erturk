#include "../../erturk/container/Array.hpp"
#include "../../erturk/container/DynamicArray.hpp"
#include "../../erturk/container/TypeBufferArray.hpp"
#include <iostream>

struct Point
{
    int x, y;

    Point() = delete;

    explicit Point(int px, int py) : x(px), y(py)
    {
        std::cout << "Point constructor" << "\n";
    }
};

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    return os << "(" << point.x << ", " << point.y << ")";
}

void type_buffer_array_examples()
{
    {
        erturk::container::TypeBufferArray<int, 5> array{1, 2, 3, 4, 5};

        for (int idx = 0; idx < array.size(); idx++)
        {
            std::cout << "Element at index " << idx << ": " << array.at(idx) << std::endl;
        }
    }

    {
        erturk::container::TypeBufferArray<Point, 3> array{Point{1, 2}, Point{3, 4}, Point{5, 6}};

        for (int idx = 0; idx < array.size(); idx++)
        {
            std::cout << "Element at index " << idx << ": " << array.at(idx) << std::endl;
        }
    }

    {
        erturk::container::TypeBufferArray<int, 5> array{};
        array.emplace(2, 100);
        std::cout << "Element at index 2: " << array.at(2) << std::endl;
    }

    {
        erturk::container::TypeBufferArray<Point, 3> pointArray{};
        pointArray.emplaceAll(Point{5, 5});
    }

    {
        erturk::container::TypeBufferArray<Point, 9> bufferArray{};

        for (int idx = 0; idx < bufferArray.size(); idx++)
        {
            bufferArray.emplace(idx, Point{idx - 1, idx + 1});
            std::cout << "Element at index " << idx << ": " << bufferArray.at(idx) << std::endl;
        }
    }
}

void array_examples()
{
    {
        erturk::container::Array<int, 5> array1{1, 2, 3, 4, 5};

        try
        {
            std::cout << "Element at index 2: " << array1.at(2) << std::endl;
            array1.at(2) = 30;
            std::cout << "Modified element at index 2: " << array1.at(2) << std::endl;
        }
        catch (const std::out_of_range& e)
        {
            std::cerr << e.what() << std::endl;
        }

        array1.fill(10);

        for (const int& element : array1)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;

        erturk::container::Array<int, 5> array2 = array1;
        std::cout << "Copied array: ";
        for (const int& element : array2)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;

        erturk::container::Array<int, 5> array3 = std::move(array2);
        std::cout << "Moved array: ";
        for (const auto& element : array3)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    {
        erturk::container::Array<Point, 3> pointArray{Point{1, 2}, Point{3, 4}, Point{5, 6}};
        pointArray.emplace(0, Point{5, 5});
    }

    {
        erturk::container::Array<Point, 3> pointArray{Point{1, 2}, Point{3, 4}, Point{5, 6}};

        for (int i = 0; i < pointArray.size(); ++i)
        {
            std::cout << "Point " << i + 1 << ": " << pointArray[i] << std::endl;
        }

        pointArray[1] = Point(10, 20);
        std::cout << "After modification, Point 2: " << pointArray[1] << std::endl;

        pointArray.fill(Point(0, 0));
        std::cout << "After filling all points with (0, 0):" << std::endl;
        for (const Point& point : pointArray)
        {
            std::cout << point << " ";
        }
        std::cout << std::endl;
    }
}

void dynamic_array_examples()
{
    {
        erturk::container::DynamicArray<int> vec{};

        for (int i = 0; i < 1000; i++)
        {
            vec.push_back(i);
        }

        for (int i : vec)
        {
            std::cout << i << " ";
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
        for (const std::string& s : copy)
        {
            std::cout << s << " ";
        }
        std::cout << std::endl;

        std::cout << "Moved array: ";
        for (const std::string& s : moved)
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

        for (char c : characters)
        {
            std::cout << c << " ";
        }
        std::cout << std::endl;

        characters.erase(characters.begin());  // Erase 'B'

        for (char c : characters)
        {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    type_buffer_array_examples();
    array_examples();
    dynamic_array_examples();
    return 0;
}
