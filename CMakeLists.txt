# =========================================================================== #
# Klog                                                                      #
# =========================================================================== #

set(PROJECT_NAME "Klog")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(KlogVersion)

cmake_minimum_required(VERSION 3.20)

project(${PROJECT_NAME} VERSION ${KLOG_VERSION_MAJOR}.${KLOG_VERSION_MINOR}.${KLOG_VERSION_PATCH} LANGUAGES C)

include(CTest)
enable_testing()

include(CreateTest)

# =========================================================================== #
# Project config                                                              #
# =========================================================================== #

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    message(STATUS "Creating compile_commands.json link in project root directory")
    file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json" "${PROJECT_SOURCE_DIR}/compile_commands.json")
else ()
    message(STATUS "CMake binary directory does not contain compile_commands.json file. This is probably the first time running cmake")
endif ()

# =========================================================================== #
# C config and compile options                                                #
# =========================================================================== #

# If the compiler is clang, we need to do some buffoonery to get -std=c99 instead of -std=gnu99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

message(STATUS "C Compiler Path: ${CMAKE_C_COMPILER}")
message(STATUS "C Compiler Version: ${CMAKE_C_COMPILER_VERSION}")

set(KLOG_CMAKE_C_DESIRED_WARNING_FLAGS -Wall -Wextra -Werror -Wpedantic)

set(KLOG_CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${KLOG_CMAKE_C_DESIRED_WARNING_FLAGS})

message(STATUS "C flags:")
foreach(COMPILE_FLAG ${CMAKE_C_FLAGS})
    message(STATUS "    ${COMPILE_FLAG}")
endforeach()

message(STATUS "Klog specific C flags:")
foreach(COMPILE_FLAG ${KLOG_CMAKE_C_FLAGS})
    message(STATUS "    ${COMPILE_FLAG}")
endforeach()

# =========================================================================== #
# Preprocessor Directives                                                     #
# =========================================================================== #

list(
    APPEND KLOG_COMPILE_DEFINITIONS
    KLOG_VERSION
    KLOG_VERSION_MAJOR=${KLOG_VERSION_MAJOR}
    KLOG_VERSION_MINOR=${KLOG_VERSION_MINOR}
    KLOG_VERSION_PATCH=${KLOG_VERSION_PATCH}
)

if (KLOG_DEBUG)
    list(APPEND KLOG_COMPILE_DEFINITIONS KLOG_DEBUG)
endif ()

if (KLOG_OFF)
    list(APPEND KLOG_COMPILE_DEFINITIONS KLOG_OFF)
endif ()

message(STATUS "Klog specific compile definitions:")
foreach(COMPILE_DEFINITION ${KLOG_COMPILE_DEFINITIONS})
    message(STATUS "    ${COMPILE_DEFINITION}")
endforeach()

# =========================================================================== #
# Source                                                                      #
# =========================================================================== #

set(KLOG_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(KLOG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

add_subdirectory("${KLOG_SOURCE_DIR}/klog")

# =========================================================================== #
# Tests                                                                       #
# =========================================================================== #

add_subdirectory("${PROJECT_SOURCE_DIR}/scratch")

# =========================================================================== #
# Examples                                                                    #
# =========================================================================== #

add_subdirectory(example)

