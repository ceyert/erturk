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

inline void divideFloatsSSE(float* a, float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 4)
    {
        __m128 aVec = _mm_loadu_ps(&a[i]);
        __m128 bVec = _mm_loadu_ps(&b[i]);
        __m128 resultVec = _mm_div_ps(aVec, bVec);
        _mm_storeu_ps(&result[i], resultVec);
    }
}

inline void reciprocalFloatsAVX(float* a, float* result, int n)
{
    for (int i = 0; i < n; i += 8)
    {
        __m256 aVec = _mm256_loadu_ps(&a[i]);
        __m256 resultVec = _mm256_rcp_ps(aVec);
        _mm256_storeu_ps(&result[i], resultVec);
    }
}

inline void minElementsSSE(const float* a, const float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 4)
    {
        __m128 aVec = _mm_loadu_ps(&a[i]);
        __m128 bVec = _mm_loadu_ps(&b[i]);
        __m128 minVec = _mm_min_ps(aVec, bVec);
        _mm_storeu_ps(&result[i], minVec);
    }
}

inline float dotProductAVX(const float* a, const float* b, int n)
{
    __m256 sumVec = _mm256_setzero_ps();
    for (int i = 0; i < n; i += 8)
    {
        __m256 aVec = _mm256_loadu_ps(&a[i]);
        __m256 bVec = _mm256_loadu_ps(&b[i]);
        __m256 multVec = _mm256_mul_ps(aVec, bVec);
        sumVec = _mm256_add_ps(sumVec, multVec);
    }
    float sumArray[8];
    _mm256_storeu_ps(sumArray, sumVec);
    float dotProduct = 0;
    for (int i = 0; i < 8; ++i)
    {
        dotProduct += sumArray[i];
    }
    return dotProduct;
}

inline float dotProductSSE(const float* a, const float* b, const int n)
{
    __m128 sum = _mm_setzero_ps();

    for (int i = 0; i < n; i += 4)
    {
        __m128 a_chunk = _mm_loadu_ps(&a[i]);
        __m128 b_chunk = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(a_chunk, b_chunk);
        sum = _mm_add_ps(sum, prod);
    }

    float result[4];
    _mm_storeu_ps(result, sum);
    return result[0] + result[1] + result[2] + result[3];
}

inline void addVectorsSSE(const float* a, const float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 4)
    {
        __m128 aVec = _mm_loadu_ps(&a[i]);
        __m128 bVec = _mm_loadu_ps(&b[i]);
        __m128 resultVec = _mm_add_ps(aVec, bVec);
        _mm_storeu_ps(&result[i], resultVec);
    }
}

inline void maxElementsAVX(const float* a, const float* b, float* result, int n)
{
    for (int i = 0; i < n; i += 8)
    {
        __m256 aVec = _mm256_loadu_ps(&a[i]);
        __m256 bVec = _mm256_loadu_ps(&b[i]);
        __m256 maxVec = _mm256_max_ps(aVec, bVec);
        _mm256_storeu_ps(&result[i], maxVec);
    }
}

inline void simdSort(float* data)
{
    __m128 a = _mm_loadu_ps(&data[0]);  // Load 4 floats
    __m128 b = _mm_loadu_ps(&data[4]);  // Load next 4 floats

    // Sort the vectors
    __m128 min = _mm_min_ps(a, b);
    __m128 max = _mm_max_ps(a, b);

    // Store the results back into the array
    _mm_storeu_ps(&data[0], min);
    _mm_storeu_ps(&data[4], max);
}

inline void simdBubbleSort(float* data, size_t size)
{
    bool swapped;
    do
    {
        swapped = false;
        for (size_t i = 0; i < size - 4; i += 4)
        {
            __m128 a = _mm_loadu_ps(&data[i]);
            __m128 b = _mm_loadu_ps(&data[i + 4]);

            __m128 min = _mm_min_ps(a, b);
            __m128 max = _mm_max_ps(a, b);

            _mm_storeu_ps(&data[i], min);
            _mm_storeu_ps(&data[i + 4], max);

            if (!_mm_comieq_ss(a, min))
            {
                swapped = true;
            }
        }
    } while (swapped);
}

void matrixMultiplySSE(float* a, float* b, float* result, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j += 4)
        {
            __m128 sum = _mm_setzero_ps();
            for (int k = 0; k < cols; k++)
            {
                __m128 a_chunk = _mm_set1_ps(a[i * cols + k]);
                __m128 b_chunk = _mm_loadu_ps(&b[k * cols + j]);
                sum = _mm_add_ps(sum, _mm_mul_ps(a_chunk, b_chunk));
            }
            _mm_storeu_ps(&result[i * cols + j], sum);
        }
    }
}

}  // namespace simd
}  // namespace erturk

#else
#error "This SIMD functionality is only supported on x86 and x86_64 architectures."
#endif

#endif  // ERTURK_SIMD_H
