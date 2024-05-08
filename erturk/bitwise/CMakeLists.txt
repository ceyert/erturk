cmake_minimum_required(VERSION 3.20)

add_library(bitwise INTERFACE)

target_include_directories(
        bitwise INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/allocator/MultiDimensionalBitArray.hpp)

