cmake_minimum_required(VERSION 3.20)

project(erturk VERSION 0.0.1)

# Set build type
if (NOT WIN32)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif ()
endif ()

option(ENABLE_QA_TEST_APP "Enable/Disable qa_test_app" ON)
option(ENABLE_LOGS "Enable/Disable logging" ON)

# Extend a list of directories specifying a search path for CMake modules to be
# loaded by the the include() or find_package() commands.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_BINARY_DIR}")

# Compile Cache Setup
if (ENABLE_COMPILE_CACHE)
    # NOTE Uses CONAN_ variable(s) provided by include(conanbuildinfo)
    include(CompileCache)
endif ()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF) # This line is optional and ensures -std=c++14 instead of -std=gnu++14


if (NOT CONAN_EXPORTED)
    include(conan)

    conan_cmake_autodetect(settings BUILD_TYPE ${OVERRIDE_CONAN_BUILD_TYPE})
    conan_cmake_install(
            PATH_OR_REFERENCE
            ${CMAKE_SOURCE_DIR}
            BUILD
            missing
            ${CONAN_BUILD_EXTRA_ARGS}
            SETTINGS
            ${settings})
endif ()

# NOTE Provides CONAN_ variables and some useful macros. Provided by Conan
# generator 'cmake' (see conanfile).
include(conanbuildinfo)

conan_basic_setup()

add_library(${PROJECT_NAME})


if (ENABLE_LOGS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_LOGGING=4)
else ()
    target_compile_definitions(${PROJECT_NAME}
            PUBLIC
            $<$<CONFIG:Debug>:ENABLE_LOGGING=0>
            $<$<CONFIG:Release>:ENABLE_LOGGING=0>
            $<$<CONFIG:RelWithDebInfo>:ENABLE_LOGGING=0>
    )
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CONAN_INCLUDE_DIRS})

# Private header files should be part of `src` to avoid re-distribution
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

add_subdirectory(erturk)

if (ENABLE_QA_TEST_APP)
    add_subdirectory(qa_app)
endif ()