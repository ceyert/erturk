cmake_minimum_required(VERSION 3.20)

add_library(resource_management INTERFACE)

target_include_directories(
        resource_management INTERFACE
        ${CMAKE_SOURCE_DIR}/erturk/resource_management/CowManager.hpp)