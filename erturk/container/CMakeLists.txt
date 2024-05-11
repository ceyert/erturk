cmake_minimum_required(VERSION 3.20)

add_library(array INTERFACE)

target_include_directories(
        array INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/container/Array.hpp
        ${CMAKE_SOURCE_DIR}/erturk/container/DynamicArray.hpp)

add_subdirectory(algorithms)
add_subdirectory(sparse)
