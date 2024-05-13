#include "../../erturk/concurrency/atomic/Atomic.hpp"
#include <iostream>

int main()
{
    {
        erturk::experimental::atomic::Atomic<int> atomicInt(0);
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
    }

    {
        // Usage of memory fence to ensure correct order of operations
        erturk::experimental::atomic::atomic_memory_fence(
            erturk::experimental::atomic::memory_order::memory_order_acquire);

        std::cout << "Critical operations are safely executed after the fence.\n";

        erturk::experimental::atomic::atomic_memory_fence(
            erturk::experimental::atomic::memory_order::memory_order_release);
    }

    {
        int value = 0;
        erturk::experimental::atomic::atomic_increment(value);
        std::cout << "Value after atomic increment: " << value << std::endl;
    }

    {
        int value = 10;
        erturk::experimental::atomic::atomic_decrement(value);
        std::cout << "Value after atomic decrement: " << value << std::endl;
    }

    {
        int value = 10;
        erturk::experimental::atomic::atomic_add(value, 5);
        std::cout << "Value after atomic add: " << value << std::endl;
    }

    {
        int oldValue = 10;
        int newValue = 20;
        int result = erturk::experimental::atomic::atomic_exchange(oldValue, newValue);
        std::cout << "Old value: " << result << ", New value: " << oldValue << std::endl;
    }

    {
        int value = 100;
        int expected = 100;
        int desired = 200;
        bool success = erturk::experimental::atomic::atomic_compare_and_exchange_strong(value, expected, desired);
        std::cout << "Operation successful: " << std::boolalpha << success << ", New value: " << value << std::endl;
    }

    {
        int value = 20;
        erturk::experimental::atomic::atomic_subtract(value, 5);
        std::cout << "Value after atomic subtract: " << value << std::endl;
    }

    {
        int value = 30;
        int addValue = 10;
        int original = erturk::experimental::atomic::atomic_fetch_and_add(value, addValue);
        std::cout << "Original value: " << original << ", New value: " << value << std::endl;
    }

    {
        bool lock = false;
        bool wasLocked = erturk::experimental::atomic::atomic_test_and_set(lock);
        std::cout << "Was locked: " << std::boolalpha << wasLocked << std::endl;
        std::cout << "Lock status: " << std::boolalpha << lock << std::endl;
    }

    {
        int value = 300;
        int oldVal = 300;
        int newVal = 500;
        bool success = erturk::experimental::atomic::atomic_compare_and_swap(value, oldVal, newVal);
        std::cout << "Operation successful: " << std::boolalpha << success << ", New value: " << value << std::endl;
    }

    return 0;
}