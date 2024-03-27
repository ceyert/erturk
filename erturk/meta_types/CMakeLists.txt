cmake_minimum_required(VERSION 3.20)

target_sources(
        ${CMAKE_PROJECT_NAME}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/erturk/meta/TypeTrait.hpp
        ${CMAKE_SOURCE_DIR}/erturk/meta/Computational.hpp
        ${CMAKE_SOURCE_DIR}/erturk/meta/Sequential.hpp)


target_include_directories(
        ${CMAKE_PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}
        PRIVATE ${CONAN_INCLUDE_DIRS})