cmake_minimum_required(VERSION 3.20)

project(erturk VERSION 0.0.1)

if (WIN32)
    message(FATAL_ERROR "erturk framework not supported on windows machines.")
endif()

# Set build types
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

# Compiler-specific settings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Configuring for Clang")
    add_compile_options(-O3 -Wall -Wextra -std=c++20)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Configuring for GCC")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
        message(FATAL_ERROR "GCC version should be at least 5.1")
    endif()
    add_compile_options(-O3 -Wall -Wextra -std=c++20)
else()
    message(WARNING "You are using a non-GCC/Clang compiler for C++17")
endif()

# Define the library as an INTERFACE since it is header-only
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/erturk)

add_subdirectory(erturk)