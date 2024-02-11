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

inline void addFloatsSSE(float* a, float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 4)
    {
        __m128 aVec = _mm_loadu_ps(&a[i]);
        __m128 bVec = _mm_loadu_ps(&b[i]);
        __m128 resultVec = _mm_add_ps(aVec, bVec);
        _mm_storeu_ps(&result[i], resultVec);
    }
}

inline void addFloatsAVX(float* a, float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 8)
    {
        __m256 aVec = _mm256_loadu_ps(&a[i]);
        __m256 bVec = _mm256_loadu_ps(&b[i]);
        __m256 resultVec = _mm256_add_ps(aVec, bVec);
        _mm256_storeu_ps(&result[i], resultVec);
    }
}

inline void multiplyFloatsSSE(const float* a, const float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 4)
    {
        __m128 aVec = _mm_loadu_ps(&a[i]);
        __m128 bVec = _mm_loadu_ps(&b[i]);
        __m128 resultVec = _mm_mul_ps(aVec, bVec);
        _mm_storeu_ps(&result[i], resultVec);
    }
}

inline void subtractFloatsSSE(float* a, float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 4)
    {
        __m128 aVec = _mm_loadu_ps(&a[i]);
        __m128 bVec = _mm_loadu_ps(&b[i]);
        __m128 resultVec = _mm_sub_ps(aVec, bVec);
        _mm_storeu_ps(&result[i], resultVec);
    }
}

inline void subtractFloatsAVX(float* a, float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 8)
    {
        __m256 aVec = _mm256_loadu_ps(&a[i]);
        __m256 bVec = _mm256_loadu_ps(&b[i]);
        __m256 resultVec = _mm256_sub_ps(aVec, bVec);
        _mm256_storeu_ps(&result[i], resultVec);
    }
}

}  // namespace simd
}  // namespace erturk

#else
#error "This SIMD functionality is only supported on x86 and x86_64 architectures."
#endif

#endif  // ERTURK_SIMD_H
