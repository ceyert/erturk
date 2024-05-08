cmake_minimum_required(VERSION 3.20)

add_library(allocator INTERFACE)

target_include_directories(allocator INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/allocator/AlignedSystemAllocator.hpp)
