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


}  // namespace atomic
}  // namespace erturk

#else
#error "This atomic functionality is only supported on x86 and x86_64 architectures."
#endif

#endif  // ERTURK_ATOMIC_H
