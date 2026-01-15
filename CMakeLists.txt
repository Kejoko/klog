# =========================================================================== #
# Gneiss                                                                      #
# =========================================================================== #

set(PROJECT_NAME "Gneiss")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GneissVersion)

cmake_minimum_required(VERSION 3.20)

project(${PROJECT_NAME} VERSION ${GNEISS_MAJOR_VERSION}.${GNEISS_MINOR_VERSION}.${GNEISS_PATCH_VERSION} LANGUAGES C)

include(CTest)
enable_testing()

include(CreateUnitTest)

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

set(GNEISS_CMAKE_C_DESIRED_WARNING_FLAGS -Wall -Wextra -Werror -Wpedantic)

set(GNEISS_CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${GNEISS_CMAKE_C_DESIRED_WARNING_FLAGS})

message(STATUS "C flags:")
foreach(COMPILE_FLAG ${CMAKE_C_FLAGS})
    message(STATUS "    ${COMPILE_FLAG}")
endforeach()

message(STATUS "Gneiss specific C flags:")
foreach(COMPILE_FLAG ${GNEISS_CMAKE_C_FLAGS})
    message(STATUS "    ${COMPILE_FLAG}")
endforeach()

# =========================================================================== #
# Preprocessor Directives                                                     #
# =========================================================================== #

list(
    APPEND GNEISS_COMPILE_DEFINITIONS
    GNEISS_VERSION
    GNEISS_MAJOR_VERSION=${GNEISS_MAJOR_VERSION}
    GNEISS_MINOR_VERSION=${GNEISS_MINOR_VERSION}
    GNEISS_PATCH_VERSION=${GNEISS_PATCH_VERSION}
)

if (KLOG_DEBUG)
    list(APPEND GNEISS_COMPILE_DEFINITIONS KLOG_DEBUG)
endif ()

message(STATUS "Gneiss specific compile definitions:")
foreach(COMPILE_DEFINITION ${GNEISS_COMPILE_DEFINITIONS})
    message(STATUS "    ${COMPILE_DEFINITION}")
endforeach()

# =========================================================================== #
# Source                                                                      #
# =========================================================================== #

set(GNEISS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(GNEISS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

add_subdirectory("${GNEISS_SOURCE_DIR}/klog")

# =========================================================================== #
# Tests                                                                       #
# =========================================================================== #

set(GNEISS_TEST_DIR "${PROJECT_SOURCE_DIR}/test")
add_subdirectory("${GNEISS_TEST_DIR}/scratch")

# =========================================================================== #
# Examples                                                                    #
# =========================================================================== #

add_subdirectory(example)

