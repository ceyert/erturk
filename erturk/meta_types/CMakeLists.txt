cmake_minimum_required(VERSION 3.20)

add_library(meta_types INTERFACE)

target_include_directories(
        meta_types INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/meta_types/TypeTrait.hpp
        ${CMAKE_SOURCE_DIR}/erturk/meta_types/Computational.hpp
        ${CMAKE_SOURCE_DIR}/erturk/meta_types/Sequential.hpp)