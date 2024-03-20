
// g++ -mavx -msse -o simd_example simd_example.cpp

#include "../../erturk/vectorization/Simd.hpp"
#include <iostream>
#include <vector>

int main() {
    {
        float a[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
        float b[] = {8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
        float result[8];

        erturk::simd::addFloatsSSE(a, b, result, 8);
        std::cout << "SSE Add: ";
        for (float f : result) 
            std::cout << f << " ";
        std::cout << "\n";

        erturk::simd::addFloatsAVX(a, b, result, 8);
        std::cout << "AVX Add: ";
        for (float f : result) 
            std::cout << f << " ";
        std::cout << "\n";
    }

    //*************************************************************************************

    {
        float a[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
        float b[] = {8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};

        float dotProductSSE = erturk::simd::dotProductSSE(a, b, 8);
        std ::cout << "Dot Product SSE: " << dotProductSSE << "\n";

        float dotProductAVX = erturk::simd::dotProductAVX(a, b, 8);
        std::cout << "Dot Product AVX: " << dotProductAVX << "\n";
    }

    //*************************************************************************************

    {
        const int dim = 4; 
        std::vector<float> a(dim * dim, 1.0f); 
        std::vector<float> b(dim * dim, 2.0f); 
        std::vector<float> result(dim * dim, 0.0f);

        erturk::simd::matrixMultiplySSE(a.data(), b.data(), result.data(), dim, dim);

        std::cout << "Matrix Multiplication Result (SSE):\n";
        for (int i = 0; i < dim; ++i) {
            for (int j = 0; j < dim; ++j) {
                std::cout << result[i * dim + j] << " ";
            }
            std::cout << "\n";
        }
    }
}
