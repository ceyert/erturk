#ifndef ERTURK_SIMD_H
#define ERTURK_SIMD_H

#if defined(__x86_64__) || defined(__i386__)

#include <immintrin.h>  // AVX
#include <xmmintrin.h>  // SSE

/*
Data Alignment: For optimal performance, ensure that your data is aligned on 16-byte (for SSE) or 32-byte (for AVX)
boundaries.

Compiler Flags: Use appropriate compiler flags to enable SIMD instructions, like -msse, -msse2, -mavx, etc., when
compiling your code. g++ -mavx -msse -o simd_example simd_example.cpp
*/
namespace erturk
{
namespace simd
{


}  // namespace simd
}  // namespace erturk

#else
#error "This SIMD functionality is only supported on x86 and x86_64 architectures."
#endif

#endif  // ERTURK_SIMD_H
