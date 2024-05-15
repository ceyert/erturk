#include "../../erturk/resource_management/CowPtr.hpp"
#include <iostream>
#include <string>
#include <utility>

class Person
{
public:
    std::string name;
    int age;

    Person(std::string name, int age) : name(std::move(name)), age(age)
    {
        std::cout << name << " " << age << "\n";
    }

    Person(const Person& other) : name(other.name), age(other.age) {}
};

int main()
{
    {
        auto cow1 = erturk::resource_management::make_cow_ptr<Person>("Hi", 100);
        auto cow2 = erturk::resource_management::make_cow_ptr<Person>("Hey", 200);

        cow1 = cow2;

        cow1.operator->()->age = 600;
    }

    return 0;
}
