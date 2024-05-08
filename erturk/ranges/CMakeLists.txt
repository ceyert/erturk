cmake_minimum_required(VERSION 3.20)

add_library(ranges INTERFACE)

target_include_directories(
        ranges INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/ranges/InitializerList.hpp)

add_subdirectory(views)