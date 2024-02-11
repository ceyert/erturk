cmake_minimum_required(VERSION 3.20)

target_sources(
        ${CMAKE_PROJECT_NAME}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/erturk/simd/Simd.hpp)


target_include_directories(
        ${CMAKE_PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}
        PRIVATE ${CONAN_INCLUDE_DIRS})