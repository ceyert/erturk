#ifndef ERTURK_ATOMIC_H
#define ERTURK_ATOMIC_H

#if defined(__x86_64__) || defined(__i386__)

#include "erturk/meta/TypeTrait.hpp"
#include <cstdint>

namespace erturk
{
namespace atomic
{

typedef enum class memory_order
{
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;


/*
"memory" clobber tells the compiler not to reorder the atomic operation relative to other memory operations, which is essential for maintaining memory order semantics.

Including "cc" in the clobber list for operations that affect flags (like inc and dec) to indicate that the condition codes may be altered by the instruction.
*/

// "lock" and "xchg" inherently enforce strong memory ordering on x86/x86_64.
// "lock" and "xchg" instruction behavior are specific to x86/x86_64 and ensure atomicity and a memory barrier.

inline void full_memory_fence()
{
    __asm__ __volatile__("mfence" ::: "memory");
}

inline void store_fence()
{
    __asm__ __volatile__("sfence" ::: "memory");
}

inline void load_fence()
{
    __asm__ __volatile__("lfence" ::: "memory");
}

template <typename T>
inline void atomic_increment(T* ptr)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    if (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock incl %0" : "+m"(*ptr)::"cc", "memory");
    }
    else if (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock incq %0" : "+m"(*ptr)::"cc", "memory");
    }
}


template <typename T>
inline void atomic_decrement(T* ptr)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    if (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock decl %0" : "+m"(*ptr)::"cc", "memory");
    }
    else if (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock decq %0" : "+m"(*ptr)::"cc", "memory");
    }
}


template <typename T>
inline void atomic_add(T* ptr, T val)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    if (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock addl %1, %0"
                             : "+m"(*ptr)
                             : "ir"(val)
                             : "cc", "memory");  // 'add' affects flags, so 'cc' is appropriate.
    }
    else if (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock addq %1, %0" : "+m"(*ptr) : "ir"(val) : "cc", "memory");
    }
}


template <typename T>
inline void atomic_subtract(T* ptr, T val)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    if (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock subl %1, %0" : "+m"(*ptr) : "ir"(val) : "cc", "memory");  // 'sub' affects flags.
    }
    else if (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock subq %1, %0" : "+m"(*ptr) : "ir"(val) : "cc", "memory");
    }
}


template <typename T>
inline T atomic_exchange(T* ptr, T newVal)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    T oldVal;

    if (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(*ptr) : "0"(newVal) : "memory");
    }
    else if (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(*ptr) : "0"(newVal) : "memory");
    }
    return oldVal;
}


template <typename T>
inline bool atomic_compare_and_exchange_strong(T* ptr, T& expected, T desired)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    unsigned char success;

    if (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock cmpxchgl %2, %1"
                             : "=q"(success), "+m"(*ptr), "+a"(expected)
                             : "r"(desired)
                             : "cc", "memory");
    }
    else if (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock cmpxchgq %2, %1"
                             : "=q"(success), "+m"(*ptr), "+a"(expected)
                             : "r"(desired)
                             : "cc", "memory");
    }
    return success;
}


template <typename T>
inline bool atomic_compare_and_exchange_weak(T* ptr, T& expected, T desired)
{
    // Directly use the strong variant as weak offers no advantage on x86/x86_64
    return atomic_compare_and_exchange_strong(ptr, expected, desired);
}

template <typename T>
inline bool atomic_compare_and_swap(T* ptr, T oldVal, T newVal)
{
    // Atomic Compare and Swap - Convenience wrapper around compare and exchange strong
    return atomic_compare_and_exchange_strong(ptr, oldVal, newVal);
}


template <typename T>
inline T atomic_fetch_and_add(T* ptr, T val)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    T original = val;

    if (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock xaddl %0, %1" : "+r"(original), "+m"(*ptr) : : "cc", "memory");
    }
    else if (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock xaddq %0, %1" : "+r"(original), "+m"(*ptr) : : "cc", "memory");
    }
    return original;  // Returns the original value before the add
}

inline bool atomic_test_and_set(bool* ptr)
{
    bool oldValue = true;
    __asm__ __volatile__("xchgb %0, %1" : "=r"(oldValue), "+m"(*ptr) : "0"(oldValue) : "cc", "memory");
    return oldValue;
}


}  // namespace atomic
}  // namespace erturk

#else
#error "This atomic functionality is only supported on x86 and x86_64 architectures."
#endif

#endif  // ERTURK_ATOMIC_H
