#ifndef ERTURK_ATOMIC_H
#define ERTURK_ATOMIC_H

#include "../../meta_types/TypeTrait.hpp"
#include <cstdint>

namespace erturk::experimental::atomic
{

typedef enum class memory_order
    : unsigned char
{
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;

/*
"memory" clobber tells the compiler not to reorder the atomic operation relative to other memory operations, which is
essential for maintaining memory order semantics.

Including "cc" in the clobber list for operations that affect flags (like inc and dec) to indicate that the condition
codes may be altered by the instruction.
*/

// "lock" and "xchg" inherently enforce strong memory ordering on x86/x86_64.
// "lock" and "xchg" instruction behavior are specific to x86/x86_64 and ensure atomicity and a memory barrier.

constexpr inline void atomic_memory_fence(memory_order order)
{
    switch (order)
    {
        case memory_order::memory_order_relaxed:
            // No fence is required for relaxed memory order operations.
            // This explicitly states the intention for documentation and clarity,
            // even though the case does nothing.
            break;
        case memory_order::memory_order_consume:
            // Historically, 'consume' semantics could be differentiated from 'acquire',
            // but in practice, it's often treated as 'acquire' due to the difficulty
            // in correctly implementing 'consume' semantics in compilers.
            // Thus, using lfence for consume as well, even though hardware might not distinguish.
#if defined(__aarch64__)
            __asm__ __volatile__("dmb ishld" ::: "memory");
#elif defined(__x86_64__)
            __asm__ __volatile__("lfence" ::: "memory");
#endif
            break;
        case memory_order::memory_order_acquire:
            // Ensures that all reads/writes issued after the fence will be executed
            // after reads/writes before the fence.
#if defined(__aarch64__)
            __asm__ __volatile__("dmb ishld" ::: "memory");
#elif defined(__x86_64__)
            __asm__ __volatile__("lfence" ::: "memory");
#endif
            break;
        case memory_order::memory_order_release:
            // Ensures that all reads/writes issued before the fence will be executed
            // before reads/writes after the fence.
#if defined(__aarch64__)
            __asm__ __volatile__("dmb ishst" ::: "memory");
#elif defined(__x86_64__)
            __asm__ __volatile__("sfence" ::: "memory");
#endif
            break;
        case memory_order::memory_order_acq_rel:
            // Combines the effects of both acquire and release barriers.
            // mfence is used here because it provides a full barrier,
            // ensuring both acquire and release semantics.
#if defined(__aarch64__)
            __asm__ __volatile__("dmb ish" ::: "memory");
#elif defined(__x86_64__)
            __asm__ __volatile__("mfence" ::: "memory");
#endif
            break;
        case memory_order::memory_order_seq_cst:
            // Provides a full barrier, ensuring total order with respect to all
            // other sequentially consistent operations.
#if defined(__aarch64__)
            __asm__ __volatile__("dmb ish" ::: "memory");
#elif defined(__x86_64__)
            __asm__ __volatile__("mfence" ::: "memory");
#endif
            break;
        default:
            // Ideally, all cases should be covered, and default should not be reached.
            // A full fence is used as a safety precaution.
#if defined(__aarch64__)
            __asm__ __volatile__("dmb ish" ::: "memory");
#elif defined(__x86_64__)
            __asm__ __volatile__("mfence" ::: "memory");
#endif
            break;
    }
}

template <typename T>
constexpr inline void atomic_increment(T& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock incl %0" : "+m"(val) : : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock incq %0" : "+m"(val) : : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t tmp;
    __asm__ __volatile__(
        "ldxr %x0, %1\n"
        "add %x0, %x0, #1\n"
        "stxr %w2, %x0, %1"
        : "=&r"(tmp), "+Q"(val)
        : "r"(tmp)
        : "memory");
#endif
}

template <typename T>
constexpr inline void atomic_increment(T&& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock incl %0" : "+m"(val) : : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock incq %0" : "+m"(val) : : "cc", "memory");
    }
#elif defined(__aarch64__)
    int32_t tmp;
    __asm__ __volatile__(
        "ldxr %w0, %1\n"
        "add %w0, %w0, #1\n"
        "stxr %w2, %w0, %1"
        : "=&r"(tmp), "+Q"(val)
        : "r"(tmp)
        : "memory");
#endif
}

template <typename T>
constexpr inline void atomic_decrement(T& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock decl %0" : "+m"(val) : : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock decq %0" : "+m"(val) : : "cc", "memory");
    }
#elif defined(__aarch64__)
    int32_t tmp;
    int32_t fail;
    __asm__ __volatile__(
        "1: ldxr %w0, %2\n"
        "   sub %w0, %w0, #1\n"
        "   stxr %w1, %w0, %2\n"
        "   cbnz %w1, 1b"
        : "=&r"(tmp), "=&r"(fail), "+Q"(val)
        :
        : "memory");
#endif
}

template <typename T>
constexpr inline void atomic_decrement(T&& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock decl %0" : "+m"(val) : : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock decq %0" : "+m"(val) : : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t tmp;
    int64_t fail;
    __asm__ __volatile__(
        "1: ldxr %x0, %2\n"
        "   sub %x0, %x0, #1\n"
        "   stxr %w1, %x0, %2\n"
        "   cbnz %w1, 1b"
        : "=&r"(tmp), "=&r"(fail), "+Q"(val)
        :
        : "memory");
#endif
}

template <typename T>
constexpr inline void atomic_add(T& ptr, T& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock addl %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock addq %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t tmp;
    int64_t fail;
    __asm__ __volatile__(
        "1: ldxr %x0, %2\n"
        "   add %x0, %x0, %x3\n"
        "   stxr %w1, %x0, %2\n"
        "   cbnz %w1, 1b"
        : "=&r"(tmp), "=&r"(fail), "+Q"(ptr)
        : "r"(val)
        : "memory");
#endif
}

template <typename T>
constexpr inline void atomic_add(T& ptr, T&& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock addl %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock addq %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t tmp;
    int64_t fail;
    __asm__ __volatile__(
        "1: ldxr %x0, %2\n"
        "   add %x0, %x0, %x3\n"
        "   stxr %w1, %x0, %2\n"
        "   cbnz %w1, 1b"
        : "=&r"(tmp), "=&r"(fail), "+Q"(ptr)
        : "r"(val)
        : "memory");
#endif
}

template <typename T>
constexpr inline void atomic_subtract(T& ptr, T&& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock subl %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock subq %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t tmp;
    int64_t fail;
    __asm__ __volatile__(
        "1: ldxr %x0, %2\n"
        "   sub %x0, %x0, %x3\n"
        "   stxr %w1, %x0, %2\n"
        "   cbnz %w1, 1b"
        : "=&r"(tmp), "=&r"(fail), "+Q"(ptr)
        : "r"(val)
        : "memory");
#endif
}

template <typename T>
constexpr inline void atomic_subtract(T& ptr, T& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock subl %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock subq %1, %0" : "+m"(ptr) : "ir"(val) : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t tmp;
    int64_t fail;
    __asm__ __volatile__(
        "1: ldxr %x0, %2\n"
        "   sub %x0, %x0, %x3\n"
        "   stxr %w1, %x0, %2\n"
        "   cbnz %w1, 1b"
        : "=&r"(tmp), "=&r"(fail), "+Q"(ptr)
        : "r"(val)
        : "memory");
#endif
}

template <typename T>
constexpr inline T atomic_exchange(T& ptr, T& newVal)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

    T oldVal{};

#if defined(__x86_64__)
    if constexpr (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(ptr) : "0"(newVal) : "memory");
    }
    else if constexpr (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(ptr) : "0"(newVal) : "memory");
    }
#elif defined(__aarch64__)
    int64_t success;
    do
    {
        __asm__ __volatile__(
            "ldxr %x0, %2\n"
            "stlxr %w1, %x3, %2"
            : "=&r"(oldVal), "=&r"(success), "+Q"(ptr)
            : "r"(newVal)
            : "memory");
    } while (success != 0);
#endif

    return oldVal;
}

template <typename T>
constexpr inline T atomic_exchange(T& ptr, T&& newVal)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

    T oldVal{};

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(ptr) : "0"(newVal) : "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(ptr) : "0"(newVal) : "memory");
    }
#elif defined(__aarch64__)
    int64_t success;
    do
    {
        __asm__ __volatile__(
            "ldxr %x0, %2\n"
            "stlxr %w1, %x3, %2"
            : "=&r"(oldVal), "=&r"(success), "+Q"(ptr)
            : "r"(newVal)
            : "memory");
    } while (success != 0);
#endif

    return oldVal;
}

template <typename T>
constexpr inline bool atomic_compare_and_exchange_strong(T& ptr, T& expected, T& desired)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

#if defined(__x86_64__)
    bool success;

    if constexpr (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock cmpxchgl %2, %1"
                             : "=a"(success), "+m"(ptr)
                             : "r"(desired), "0"(expected)
                             : "cc", "memory");
    }
    else if constexpr (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock cmpxchgq %2, %1"
                             : "=a"(success), "+m"(ptr)
                             : "r"(desired), "0"(expected)
                             : "cc", "memory");
    }
    return success == 0;
#elif defined(__aarch64__)
    bool success;

    T original{};

    __asm__ __volatile__(
        "1: ldxr %x1, %2\n"
        "   cmp %x1, %x3\n"
        "   b.ne 2f\n"
        "   stxr %w0, %x4, %2\n"
        "   cbnz %w0, 1b\n"
        "2:"
        : "=&r"(success), "=&r"(original), "+Q"(ptr)
        : "r"(expected), "r"(desired)
        : "cc", "memory");

    return success == 0;
#endif
}

template <typename T>
constexpr inline bool atomic_compare_and_exchange_weak(T& ptr, T& expected, T& desired)
{
    // Directly use the strong variant as weak offers no advantage on x86/x86_64
    return atomic_compare_and_exchange_strong(ptr, expected, desired);
}

template <typename T>
constexpr inline bool atomic_compare_and_swap(T& ptr, T& oldVal, T& newVal)
{
    // Atomic Compare and Swap - Convenience wrapper around compare and exchange strong
    return atomic_compare_and_exchange_strong(ptr, oldVal, newVal);
}

template <typename T>
constexpr inline T atomic_fetch_and_add(T& ptr, T& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

    T original = val;

#if defined(__x86_64__)
    if constexpr (sizeof(T) == sizeof(int32_t))
    {
        __asm__ __volatile__("lock xaddl %0, %1" : "+r"(original), "+m"(ptr) : : "cc", "memory");
    }
    else if constexpr (sizeof(T) == sizeof(int64_t))
    {
        __asm__ __volatile__("lock xaddq %0, %1" : "+r"(original), "+m"(ptr) : : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t success;
    do
    {
        __asm__ __volatile__(
            "ldxr %x0, %2\n"
            "add %x0, %x0, %x3\n"
            "stxr %w1, %x0, %2"
            : "=&r"(original), "=&r"(success), "+Q"(ptr)
            : "r"(val)
            : "memory");
    } while (success != 0);
#endif
    return original;  // Returns the original value before the add
}

template <typename T>
constexpr inline T atomic_fetch_and_add(T& ptr, T&& val)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");
    static_assert(erturk::meta::is_arithmetic<T>::value, "Atomic operations require integral types.");
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

    T original = val;

#if defined(__x86_64__)
    if constexpr (sizeof(void*) == sizeof(uint32_t))
    {
        __asm__ __volatile__("lock xaddl %0, %1" : "+r"(original), "+m"(ptr) : : "cc", "memory");
    }
    else if constexpr (sizeof(void*) == sizeof(uint64_t))
    {
        __asm__ __volatile__("lock xaddq %0, %1" : "+r"(original), "+m"(ptr) : : "cc", "memory");
    }
#elif defined(__aarch64__)
    int64_t success;
    do
    {
        __asm__ __volatile__(
            "ldxr %x0, %2\n"
            "add %x0, %x0, %x3\n"
            "stxr %w1, %x0, %2"
            : "=&r"(original), "=&r"(success), "+Q"(ptr)
            : "r"(val)
            : "memory");
    } while (success != 0);
#endif
    return original;  // Returns the original value before the add
}

inline bool atomic_test_and_set(bool& ptr)
{
    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");

    bool oldValue = true;

#if defined(__x86_64__)
    // x86-64 version using xchg
    __asm__ __volatile__("xchgb %0, %1" : "=r"(oldValue), "+m"(ptr) : "0"(oldValue) : "cc", "memory");
#elif defined(__aarch64__)
    int32_t success;
    do
    {
        __asm__ __volatile__(
            "ldxr %w0, %2\n"       // Load the current value at ptr into oldValue
            "stxr %w1, %w3, %2\n"  // Attempt to store true (1) into ptr
            : "=&r"(oldValue), "=&r"(success), "+Q"(ptr)
            : "r"(1)  // We want to set ptr to true
            : "memory");
    } while (success != 0);  // Retry if stxr failed
#endif

    return oldValue;  // Return the original value of ptr
}

template <typename T>
class Atomic final
{
    static_assert((erturk::meta::is_trivial<T>::value && erturk::meta::is_trivially_copyable<T>::value),
                  "Atomic types must be trivially-copyable!");

    static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unsupported operand size for atomic operations.");

public:
    explicit constexpr Atomic(T initialValue = T{}) : value_(initialValue) {}

public:
    Atomic() noexcept = default;
    ~Atomic() noexcept = default;

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;

    void increment()
    {
        atomic_increment(value_);
    }

    void decrement()
    {
        atomic_decrement(value_);
    }

    void add(T& val)
    {
        atomic_add(value_, val);
    }

    void subtract(T& val)
    {
        atomic_subtract(value_, val);
    }

    void add(T&& val)
    {
        atomic_add(value_, val);
    }

    void subtract(T&& val)
    {
        atomic_subtract(value_, val);
    }

    constexpr T fetch_and_add(T& val, memory_order order = memory_order::memory_order_seq_cst)
    {
        return atomic_fetch_and_add(value_, val);
    }

    constexpr T fetch_and_add(T&& val, memory_order order = memory_order::memory_order_seq_cst)
    {
        return atomic_fetch_and_add(value_, val);
    }

    constexpr T fetch_and_increment()
    {
        return fetch_and_add(1);
    }

    constexpr T fetch_and_decrement()
    {
        return fetch_and_add(-1);
    }

    constexpr bool compare_and_exchange_strong(T& expected, T& desired,
                                               memory_order order = memory_order::memory_order_seq_cst)
    {
        return atomic_compare_and_exchange_strong(value_, expected, desired);
    }

    constexpr bool compare_and_exchange_strong(T&& expected, T&& desired,
                                               memory_order order = memory_order::memory_order_seq_cst)
    {
        return atomic_compare_and_exchange_strong(value_, expected, desired);
    }

    constexpr bool compare_and_swap(T& oldValue, T& newValue)
    {
        return compare_and_exchange_strong(oldValue, newValue, memory_order::memory_order_seq_cst);
    }

    constexpr bool compare_and_swap(T& oldValue, T&& newValue)
    {
        return compare_and_exchange_strong(oldValue, newValue, memory_order::memory_order_seq_cst);
    }

    constexpr T load(memory_order order = memory_order::memory_order_seq_cst) const
    {
        T val{};

        switch (order)
        {
            case memory_order::memory_order_relaxed:
                // A relaxed load might simply move the value without additional barriers.
                if constexpr (sizeof(T) == sizeof(int32_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("ldr %w0, %1" : "=r"(val) : "Q"(value_));
#elif defined(__x86_64__)
                    __asm__ __volatile__("movl %1, %0" : "=r"(val) : "m"(value_));
#endif
                }
                else if constexpr (sizeof(T) == sizeof(int64_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("ldr %x0, %1" : "=r"(val) : "Q"(value_));
#elif defined(__x86_64__)
                    __asm__ __volatile__("movq %1, %0" : "=r"(val) : "m"(value_));
#endif
                }
                break;
            case memory_order::memory_order_consume:
            case memory_order::memory_order_acquire:
                // Acquire semantics can be ensured by a subsequent barrier.
                // x86 loads have implicit acquire semantics, but we include a compiler barrier for illustration.
                apply_memory_fence(memory_order::memory_order_acquire);
                if constexpr (sizeof(T) == sizeof(int32_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("ldar %w0, %1" : "=r"(val) : "Q"(value_) : "memory");
#elif defined(__x86_64__)
                    __asm__ __volatile__("movl %1, %0" : "=r"(val) : "m"(value_) : "memory");
#endif
                }
                else if constexpr (sizeof(T) == sizeof(int64_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("ldar %x0, %1" : "=r"(val) : "Q"(value_) : "memory");
#elif defined(__x86_64__)
                    __asm__ __volatile__("movq %1, %0" : "=r"(val) : "m"(value_) : "memory");
#endif
                }
                break;
            default:
                // Sequentially consistent load requires a full fence before the load.
                // x86's strong model makes this mostly unnecessary, but we include it for completeness.
                apply_memory_fence(memory_order::memory_order_seq_cst);
                if constexpr (sizeof(T) == sizeof(int32_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("ldr %w0, %1" : "=r"(val) : "Q"(value_) : "memory");
#elif defined(__x86_64__)
                    __asm__ __volatile__("movl %1, %0" : "=r"(val) : "m"(value_) : "memory");
#endif
                }
                else if constexpr (sizeof(T) == sizeof(int64_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("ldr %x0, %1" : "=r"(val) : "Q"(value_) : "memory");
#elif defined(__x86_64__)
                    __asm__ __volatile__("movq %1, %0" : "=r"(val) : "m"(value_) : "memory");
#endif
                }
                apply_memory_fence(memory_order::memory_order_seq_cst);
                break;
        }
        return val;
    }

    void store(T newVal, memory_order order = memory_order::memory_order_seq_cst)
    {
        switch (order)
        {
            case memory_order::memory_order_relaxed:
                // A relaxed store directly writes the value.
                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("str %w1, %0" : "=Q"(value_) : "r"(newVal));
#elif defined(__x86_64__)
                    __asm__ __volatile__("movl %1, %0" : "=m"(value_) : "ri"(newVal));
#endif
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("str %x1, %0" : "=Q"(value_) : "r"(newVal));
#elif defined(__x86_64__)
                    __asm__ __volatile__("movq %1, %0" : "=m"(value_) : "ri"(newVal));
#endif
                }
                break;
            case memory_order::memory_order_release:
                // Release semantics can be ensured by a barrier before the store.
                apply_memory_fence(memory_order::memory_order_release);

                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("stlr %w1, %0" : "=Q"(value_) : "r"(newVal));
#elif defined(__x86_64__)
                    __asm__ __volatile__("movl %1, %0" : "=m"(value_) : "ri"(newVal));
#endif
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("stlr %x1, %0" : "=Q"(value_) : "r"(newVal));
#elif defined(__x86_64__)
                    __asm__ __volatile__("movq %1, %0" : "=m"(value_) : "ri"(newVal));
#endif
                }
                break;
            default:
                // Sequentially consistent store requires a full fence before the store.
                apply_memory_fence(memory_order::memory_order_seq_cst);

                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("stlr %w1, %0" : "=Q"(value_) : "r"(newVal) : "memory");
#elif defined(__x86_64__)
                    __asm__ __volatile__("movl %1, %0" : "=m"(value_) : "ri"(newVal) : "memory");
#endif
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
#if defined(__aarch64__)
                    __asm__ __volatile__("stlr %x1, %0" : "=Q"(value_) : "r"(newVal) : "memory");
#elif defined(__x86_64__)
                    __asm__ __volatile__("movq %1, %0" : "=m"(value_) : "ri"(newVal) : "memory");
#endif
                }
                apply_memory_fence(memory_order::memory_order_seq_cst);
                break;
        }
    }

    constexpr T exchange(T& newVal, memory_order order = memory_order::memory_order_seq_cst)
    {
        T oldVal{};

        switch (order)
        {
            case memory_order::memory_order_relaxed:
                // For a relaxed exchange, although x86's `xchg` has an implicit lock,
                // we illustrate a relaxed approach without additional barriers.
                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
                }
                break;
            case memory_order::memory_order_acquire:
            case memory_order::memory_order_release:
            case memory_order::memory_order_acq_rel:
                // Since `xchg` inherently acts as a full barrier on x86,
                // these cases don't differ in implementation. However, explicit barriers
                // are added for illustration aligned with memory order semantics.
                apply_memory_fence(memory_order::memory_order_acq_rel);

                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
                }

                apply_memory_fence(memory_order::memory_order_acq_rel);
                break;
            case memory_order::memory_order_seq_cst:
            default:
                // Sequential consistency is naturally ensured by the `xchg` operation's
                // implicit lock behavior, but we include a fence for explicitness.
                apply_memory_fence(memory_order::memory_order_seq_cst);

                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
                }

                apply_memory_fence(memory_order::memory_order_seq_cst);
                break;
        }
        return oldVal;
    }

    constexpr T exchange(T newVal, memory_order order = memory_order::memory_order_seq_cst)
    {
        T oldVal{};

        switch (order)
        {
            case memory_order::memory_order_relaxed:
                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
#if defined(__aarch64__)
                    int32_t success;
                    do
                    {
                        __asm__ __volatile__(
                            "ldxr %w0, %2\n"
                            "stlxr %w1, %w3, %2"
                            : "=&r"(oldVal), "=&r"(success), "+Q"(value_)
                            : "r"(newVal)
                            : "memory");
                    } while (success != 0);
#elif defined(__x86_64__)
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
#endif
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
#if defined(__aarch64__)
                    int64_t success;
                    do
                    {
                        __asm__ __volatile__(
                            "ldxr %x0, %2\n"
                            "stlxr %w1, %x3, %2"
                            : "=&r"(oldVal), "=&r"(success), "+Q"(value_)
                            : "r"(newVal)
                            : "memory");
                    } while (success != 0);
#elif defined(__x86_64__)
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
#endif
                }
                break;
            case memory_order::memory_order_acquire:
            case memory_order::memory_order_release:
            case memory_order::memory_order_acq_rel:
                apply_memory_fence(memory_order::memory_order_acq_rel);

                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
#if defined(__aarch64__)
                    int32_t success;
                    do
                    {
                        __asm__ __volatile__(
                            "ldaxr %w0, %2\n"
                            "stlxr %w1, %w3, %2"
                            : "=&r"(oldVal), "=&r"(success), "+Q"(value_)
                            : "r"(newVal)
                            : "memory");
                    } while (success != 0);
#elif defined(__x86_64__)
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
#endif
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
#if defined(__aarch64__)
                    int64_t success;
                    do
                    {
                        __asm__ __volatile__(
                            "ldaxr %x0, %2\n"
                            "stlxr %w1, %x3, %2"
                            : "=&r"(oldVal), "=&r"(success), "+Q"(value_)
                            : "r"(newVal)
                            : "memory");
                    } while (success != 0);
#elif defined(__x86_64__)
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
#endif
                }

                apply_memory_fence(memory_order::memory_order_acq_rel);
                break;
            case memory_order::memory_order_seq_cst:
            default:
                apply_memory_fence(memory_order::memory_order_seq_cst);

                if constexpr (sizeof(void*) == sizeof(uint32_t))
                {
#if defined(__aarch64__)
                    int32_t success;
                    do
                    {
                        __asm__ __volatile__(
                            "ldaxr %w0, %2\n"
                            "stlxr %w1, %w3, %2"
                            : "=&r"(oldVal), "=&r"(success), "+Q"(value_)
                            : "r"(newVal)
                            : "memory");
                    } while (success != 0);
#elif defined(__x86_64__)
                    __asm__ __volatile__("xchgl %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
#endif
                }
                else if constexpr (sizeof(void*) == sizeof(uint64_t))
                {
#if defined(__aarch64__)
                    int64_t success;
                    do
                    {
                        __asm__ __volatile__(
                            "ldaxr %x0, %2\n"
                            "stlxr %w1, %x3, %2"
                            : "=&r"(oldVal), "=&r"(success), "+Q"(value_)
                            : "r"(newVal)
                            : "memory");
                    } while (success != 0);
#elif defined(__x86_64__)
                    __asm__ __volatile__("xchgq %0, %1" : "=r"(oldVal), "+m"(value_) : "0"(newVal) : "memory");
#endif
                }

                apply_memory_fence(memory_order::memory_order_seq_cst);
                break;
        }
        return oldVal;
    }

private:
    static void apply_memory_fence(memory_order order)
    {
        atomic_memory_fence(order);
    }

private:
    T value_{};
};

}  // namespace erturk::experimental::atomic

#endif  // ERTURK_ATOMIC_H