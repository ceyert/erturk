cmake_minimum_required(VERSION 3.20)

add_library(vectorization INTERFACE)

target_include_directories(
        vectorization INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/vectorization/Simd.hpp)