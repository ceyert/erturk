cmake_minimum_required(VERSION 3.20)

add_library(container INTERFACE)

target_include_directories(
        container INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/container/Array.hpp
        ${CMAKE_SOURCE_DIR}/erturk/container/DynamicArray.hpp
        ${CMAKE_SOURCE_DIR}/erturk/container/TypeBufferArray.hpp)

add_subdirectory(algorithms)
add_subdirectory(sparse)
