#include "../../erturk/resource_management/CowLifetimeCounter.hpp"
#include <iostream>
#include <string>
#include <utility>

class Person
{
public:
    std::string name_;
    int age_;

    Person(std::string name, int age) : name_(std::move(name)), age_(age)
    {
        std::cout << name_ << " - " << age_ << "\n";
    }

    Person(const Person& other) = default;
};

int main()
{
    auto cow1 = erturk::resource_management::make_cow_ptr<Person>(std::string{"Hi"}, 100);
    {
        auto cow2 = cow1;

        cow2.operator->()->age_ = 600;
    }

    cow1.operator->()->age_ = 300;

    return 0;
}
