cmake_minimum_required(VERSION 3.20)

add_library(container INTERFACE)

target_include_directories(
        container INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/containers/Array.hpp
        ${CMAKE_SOURCE_DIR}/erturk/containers/TypeBufferArray.hpp
        ${CMAKE_SOURCE_DIR}/erturk/containers/DynamicTypeBufferArray.hpp
        ${CMAKE_SOURCE_DIR}/erturk/containers/String.hpp)

add_subdirectory(iterators)
add_subdirectory(ranges)
add_subdirectory(views)
add_subdirectory(sparse)
