cmake_minimum_required(VERSION 3.20)

project(samples VERSION 0.0.1)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/erturk/samples/bitwise/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/erturk)

target_link_libraries(${PROJECT_NAME} erturk)