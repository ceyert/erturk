cmake_minimum_required(VERSION 3.20)

project(samples VERSION 0.0.1)

# Assuming the 'erturk' library target is defined in a parent or the same level directory.
# If the 'erturk' CMake is not included from a higher level, you will need to include it here:
# add_subdirectory(path_to_erturk) where path_to_erturk is the path to the 'erturk' CMakeLists.txt

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/erturk/samples/bitwise/main.cpp)

# Include the headers from the 'erturk' library
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/erturk)

# Link the executable to the 'erturk' library to inherit interface definitions
target_link_libraries(${PROJECT_NAME} erturk)
