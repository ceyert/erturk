#include <iostream>
#include "../../erturk/concurrency/atomic/Atomic.hpp"

int main(int argc, char const *argv[])
{
    erturk::atomic::Atomic<int> atomicInt(0);
    std::cout << "Initial value: " << atomicInt.load() << std::endl;
    
    atomicInt.increment();
    std::cout << "After increment: " << atomicInt.load() << std::endl;
    
    atomicInt.add(5);
    std::cout << "After adding 5: " << atomicInt.load() << std::endl;
    
    int expected = 6;
    int desired = 10;
    bool wasSuccessful = atomicInt.compare_and_exchange_strong(expected, desired);
    std::cout << "Compare and exchange from 6 to 10 successful? " << wasSuccessful << std::endl;
    std::cout << "Current value: " << atomicInt.load() << std::endl;

    expected = atomicInt.load(); 
    desired = 15;
    wasSuccessful = atomicInt.compare_and_exchange_strong(expected, desired);
    std::cout << "Compare and exchange from 10 to 15 successful? " << wasSuccessful << std::endl;
    std::cout << "Current value: " << atomicInt.load() << std::endl;

    int originalValue = atomicInt.fetch_and_add(5);
    std::cout << "Value before fetch and add 5: " << originalValue << std::endl;
    std::cout << "Current value after fetch and add 5: " << atomicInt.load() << std::endl;

    return 0;
}
