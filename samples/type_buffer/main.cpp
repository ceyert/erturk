#include "../../erturk/memory/TypeBuffer.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>

class Person
{
public:
    std::string name;
    int age;
    Person(std::string name, int age) : name(std::move(name)), age(age) {}
    void display() const
    {
        std::cout << "Name: " << name << ", Age: " << age << std::endl;
    }
};

class LogManager
{
    std::ofstream logFile;

public:
    LogManager(const std::string& filename)
    {
        logFile.open(filename, std::ofstream::out | std::ofstream::app);
    }
    void log(const std::string& message)
    {
        if (logFile.is_open())
        {
            logFile << message << std::endl;
        }
    }
    ~LogManager()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }
};

int main()
{
    {
        erturk::memory::TypeBuffer<int> buffer{};
        buffer.emplace(42);
        std::cout << "Buffer contains: " << buffer.operator*() << std::endl;
    }

    {
        erturk::memory::TypeBuffer<Person> personBuffer{};
        personBuffer.emplace("Alice", 30);
        personBuffer->display();
    }

    {
        erturk::memory::TypeBuffer<LogManager> logger;
        logger.emplace("app.log");  // Initialize logger
        logger->log("Application started!");

        // Resetting the logger with a new file
        logger.reset();
        logger.emplace("new_app.log");
        logger->log("Logging to a new file");
    }

    {
        erturk::memory::TypeBuffer<std::vector<int>> originalBuffer{};
        originalBuffer.emplace();

        originalBuffer.operator*().push_back(1);
        originalBuffer.operator*().push_back(2);
        originalBuffer.operator*().push_back(3);
        originalBuffer.operator*().push_back(4);

        // Copy constructor
        erturk::memory::TypeBuffer<std::vector<int>> copyBuffer = originalBuffer;

        // Move constructor
        erturk::memory::TypeBuffer<std::vector<int>> movedBuffer = std::move(originalBuffer);
        for (int num : movedBuffer.operator*())
        {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }

    {
        erturk::memory::TypeBuffer<std::unique_ptr<int>> uniquePtrBuffer{};
        uniquePtrBuffer.emplace(std::make_unique<int>(10));
        std::cout << "Unique pointer value: " << uniquePtrBuffer.operator*().operator*() << std::endl;
    }

    {
        erturk::memory::TypeBuffer<std::map<std::string, int>> mapBuffer{};
        mapBuffer.emplace();

        mapBuffer.operator*().operator[]("one") = 1;
        mapBuffer.operator*().operator[]("two") = 1;

        for (const auto& pair : mapBuffer.operator*())
        {
            std::cout << pair.first << " = " << pair.second << std::endl;
        }
    }
}