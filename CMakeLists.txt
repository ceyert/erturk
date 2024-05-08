cmake_minimum_required(VERSION 3.20)

project(erturk VERSION 0.0.1)

# Set build type
if (NOT WIN32)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Only support Linux and macOS
if (NOT UNIX OR NOT APPLE)
    message(FATAL_ERROR "This framework only supported on Linux and macOS.")
endif()

# Compiler-specific settings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Configuring for Clang")
    add_compile_options(-Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Configuring for GCC")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
        message(FATAL_ERROR "GCC version should be at least 5.1")
    endif()
    add_compile_options(-Wall -Wextra)
else()
    message(WARNING "You are using a non-GCC/Clang compiler for C++17")
endif()

# Define the library as an INTERFACE since it is header-only
add_library(${PROJECT_NAME} INTERFACE)

# Include directories
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/erturk)

# Add subdirectories if needed
add_subdirectory(erturk)