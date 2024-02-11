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

template <typename T>
class Atomic
{
public:
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic only supports integral types.");

    explicit constexpr Atomic(T initialValue = T{}) : value(initialValue) {}

public:
    Atomic() noexcept = default;
    ~Atomic() noexcept = default;

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) volatile = delete;

    void increment()
    {
        if (sizeof(T) == 4)
        {
            __asm__ __volatile__("lock incl %0" : "+m"(value) : : "memory");
        }
        else if (sizeof(T) == 8)
        {
            __asm__ __volatile__("lock incq %0" : "+m"(value) : : "memory");
        }
    }

    void decrement()
    {
        if (sizeof(T) == 4)
        {
            __asm__ __volatile__("lock decl %0" : "+m"(value) : : "memory");
        }
        else if (sizeof(T) == 8)
        {
            __asm__ __volatile__("lock decq %0" : "+m"(value) : : "memory");
        }
    }

    void add(T& val)
    {
        if (sizeof(T) == 4)
        {
            __asm__ __volatile__("lock addl %1, %0" : "+m"(value) : "ir"(val) : "memory");
        }
        else if (sizeof(T) == 8)
        {
            __asm__ __volatile__("lock addq %1, %0" : "+m"(value) : "ir"(val) : "memory");
        }
    }

    void subtract(T& val)
    {
        if (sizeof(T) == 4)
        {
            __asm__ __volatile__("lock subl %1, %0" : "+m"(value) : "ir"(val) : "memory");
        }
        else if (sizeof(T) == 8)
        {
            __asm__ __volatile__("lock subq %1, %0" : "+m"(value) : "ir"(val) : "memory");
        }
    }

    T fetch_and_increment()
    {
        return fetch_and_add(1);
    }

    T fetch_and_decrement()
    {
        return fetch_and_add(-1);
    }

    // Modified load and store with explicit fences for demonstration
    T load_acquire() const
    {
        T val;
        // Load the value
        val = value;
        // Followed by a load fence to ensure acquire semantics
        load_fence();
        return val;
    }

    void store_release(T newVal)
    {
        // Issue a store fence before the store to ensure release semantics
        store_fence();
        value = newVal;
    }

    T fetch_and_add(T val, memory_order order = memory_order::memory_order_seq_cst)
    {
        T original = val;
        // The lock prefix ensures atomicity and a full memory barrier, which corresponds to memory_order_seq_cst.
        // For relaxed memory order, x86/x86_64 does not provide a way to perform atomic operations without a full
        // barrier, but we include the case for illustration and completeness.
        switch (order)
        {
            case memory_order::memory_order_relaxed:
                // There's no relaxed version of xadd with lock prefix omitted on x86/x86_64,
                // as omitting 'lock' would not guarantee atomicity.
                // This case is handled the same way as memory_order_seq_cst for demonstration.
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("xaddl %0, %1" : "+r"(original), "+m"(value) : : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("xaddq %0, %1" : "+r"(original), "+m"(value) : : "memory");
                }
                break;
            default:
                // For all other memory orders, including seq_cst, acq_rel, acquire, and release,
                // the lock prefix is used to ensure a full barrier.
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("lock xaddl %0, %1" : "+r"(original), "+m"(value) : : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("lock xaddq %0, %1" : "+r"(original), "+m"(value) : : "memory");
                }
                break;
        }
        return original;  // Returns the original value
    }

    bool compare_and_exchange_strong(T expected, T desired, memory_order order = memory_order::memory_order_seq_cst)
    {
        unsigned char success;
        if (sizeof(T) == 4)
        {
            __asm__ __volatile__(
                "lock cmpxchgl %2, %1\n\t"
                "sete %0\n\t"
                : "=q"(success), "+m"(value), "+a"(expected)
                : "r"(desired)
                : "memory");
        }
        else if (sizeof(T) == 8)
        {
            __asm__ __volatile__(
                "lock cmpxchgq %2, %1\n\t"
                "sete %0\n\t"
                : "=q"(success), "+m"(value), "+a"(expected)
                : "r"(desired)
                : "memory");
        }
        return success;
    }

    bool compare_and_swap(T oldValue, T newValue)
    {
        return compare_and_exchange_strong(oldValue, newValue, memory_order::memory_order_seq_cst);
    }

    T load(memory_order order = memory_order::memory_order_seq_cst) const
    {
        T val;
        switch (order)
        {
            case memory_order::memory_order_relaxed:
                // A relaxed load might simply move the value without additional barriers.
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("movl %1, %0" : "=r"(val) : "m"(value));
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("movq %1, %0" : "=r"(val) : "m"(value));
                }
                break;
            case memory_order::memory_order_consume:
            case memory_order::memory_order_acquire:
                // Acquire semantics can be ensured by a subsequent barrier.
                // x86 loads have implicit acquire semantics, but we include a compiler barrier for illustration.
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("movl %1, %0" : "=r"(val) : "m"(value) : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("movq %1, %0" : "=r"(val) : "m"(value) : "memory");
                }
                break;
            default:
                // Sequentially consistent load requires a full fence before the load.
                // x86's strong model makes this mostly unnecessary, but we include it for completeness.
                full_memory_fence();
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("movl %1, %0" : "=r"(val) : "m"(value) : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("movq %1, %0" : "=r"(val) : "m"(value) : "memory");
                }
                full_memory_fence();
                break;
        }
        return val;
    }

    void store(T& newVal, memory_order order = memory_order::memory_order_seq_cst)
    {
        switch (order)
        {
            case memory_order::memory_order_relaxed:
                // A relaxed store directly writes the value.
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("movl %1, %0" : "=m"(value) : "ri"(newVal));
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("movq %1, %0" : "=m"(value) : "ri"(newVal));
                }
                break;
            case memory_order::memory_order_release:
                // Release semantics can be ensured by a barrier before the store.
                // x86 stores have implicit release semantics, but we include a compiler barrier for illustration.
                store_fence();
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("movl %1, %0" : "=m"(value) : "ri"(newVal));
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("movq %1, %0" : "=m"(value) : "ri"(newVal));
                }
                break;
            default:
                // Sequentially consistent store requires a full fence after the store.
                // Again, x86's model makes this mostly unnecessary, but included for completeness.
                full_memory_fence();
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("movl %1, %0" : "=m"(value) : "ri"(newVal) : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("movq %1, %0" : "=m"(value) : "ri"(newVal) : "memory");
                }
                full_memory_fence();
                break;
        }
    }

    T exchange(T newVal, memory_order order = memory_order::memory_order_seq_cst)
    {
        T oldVal;
        switch (order)
        {
            case memory_order::memory_order_relaxed:
                // For a relaxed exchange, although x86's `xchg` has an implicit lock,
                // we illustrate a relaxed approach without additional barriers.
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value) : "0"(newVal) : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value) : "0"(newVal) : "memory");
                }
                break;
            case memory_order::memory_order_acquire:
            case memory_order::memory_order_release:
            case memory_order::memory_order_acq_rel:
                // Since `xchg` inherently acts as a full barrier on x86,
                // these cases don't differ in implementation. However, explicit barriers
                // are added for illustration aligned with memory order semantics.
                full_memory_fence();
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value) : "0"(newVal) : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value) : "0"(newVal) : "memory");
                }
                full_memory_fence();
                break;
            case memory_order::memory_order_seq_cst:
            default:
                // Sequential consistency is naturally ensured by the `xchg` operation's
                // implicit lock behavior, but we include a fence for explicitness.
                full_memory_fence();
                if (sizeof(T) == 4)
                {
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value) : "0"(newVal) : "memory");
                }
                else if (sizeof(T) == 8)
                {
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value) : "0"(newVal) : "memory");
                }
                full_memory_fence();
                break;
        }
        return oldVal;
    }

private:
    static void full_memory_fence()
    {
        __asm__ __volatile__("mfence" ::: "memory");
    }

    static void store_fence()
    {
        __asm__ __volatile__("sfence" ::: "memory");
    }

    static void load_fence()
    {
        __asm__ __volatile__("lfence" ::: "memory");
    }

private:
    T value;
};

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
