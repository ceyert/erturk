cmake_minimum_required(VERSION 3.20)

add_library(memory INTERFACE)

target_include_directories(
        memory INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/memory/Alignment.hpp
        ${CMAKE_SOURCE_DIR}/erturk/memory/Memory.hpp)