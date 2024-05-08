cmake_minimum_required(VERSION 3.20)

add_library(atomics INTERFACE)

target_include_directories(
        atomics INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/concurrency/atomic/Atomic.hpp)

