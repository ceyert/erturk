cmake_minimum_required(VERSION 3.20)
project(sample_atomics)

add_executable(${CMAKE_PROJECT_NAME} erturk/samples/concurrency/atomics.cpp)

target_include_directories(
        ${CMAKE_PROJECT_NAME}
        PUBLIC
        ${CMAKE_SOURCE_DIR} 
        PRIVATE
        ${CONAN_INCLUDE_DIRS}) 
