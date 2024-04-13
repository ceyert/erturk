
#include "../../erturk/container/Array.hpp"
#include <iostream>

struct Point
{
    int x, y;

    explicit Point() : x(0), y(0) {}

    explicit Point(int px, int py) : x(px), y(py) {}
};

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    return os << "(" << point.x << ", " << point.y << ")";
}

int main()
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

    {
        erturk::container::Array<Point, 3> points = {Point(1, 2), Point(3, 4), Point(5, 6)};

        for (int i = 0; i < points.size(); ++i)
        {
            std::cout << "Point " << i + 1 << ": " << points[i] << std::endl;
        }

        points[1] = Point(10, 20);
        std::cout << "After modification, Point 2: " << points[1] << std::endl;

        points.fill(Point(0, 0));
        std::cout << "After filling all points with (0, 0):" << std::endl;
        for (const Point& point : points)
        {
            std::cout << point << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}