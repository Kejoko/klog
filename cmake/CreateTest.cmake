# =========================================================================== #
# Functionality for easily creating tests                                     #
# =========================================================================== #

# --------------------------------------------------------------------------- #
# Base test functionality                                                     #
# --------------------------------------------------------------------------- #

set_property(GLOBAL PROPERTY EXPECT_FAILURE_FILE_ALREADY_SETUP FALSE)
set_property(GLOBAL PROPERTY IGNORE_MEMORY_FILE_ALREADY_SETUP FALSE)
set(TEST_SCRIPT_LINK_ALREADY_SETUP FALSE)

function(update_expect_failure_list_file test_name)
    set(expect_failure_list_file_path ${CMAKE_BINARY_DIR}/tests_expected_failure_list.txt)
    get_property(expect_failure_file_already_setup GLOBAL PROPERTY EXPECT_FAILURE_FILE_ALREADY_SETUP)
    if (NOT ${expect_failure_file_already_setup})
        file(WRITE ${expect_failure_list_file_path} "")
        set_property(GLOBAL PROPERTY EXPECT_FAILURE_FILE_ALREADY_SETUP TRUE)
    endif ()
    file(APPEND ${expect_failure_list_file_path} "${test_name}\n")
endfunction()

function(update_ignore_memory_list_file test_name)
    set(ignore_memory_list_file_path ${CMAKE_BINARY_DIR}/tests_ignore_memory_list.txt)
    get_property(ignore_memory_file_already_setup GLOBAL PROPERTY IGNORE_MEMORY_FILE_ALREADY_SETUP)
    if (NOT ${ignore_memory_file_already_setup})
        file(WRITE ${ignore_memory_list_file_path} "")
        set_property(GLOBAL PROPERTY IGNORE_MEMORY_FILE_ALREADY_SETUP TRUE)
    endif ()
    file(APPEND ${ignore_memory_list_file_path} "${test_name}\n")
endfunction()

function(create_test main_file runtime_output_directory)
    get_filename_component(test_name ${main_file} NAME_WE)
    add_executable(${test_name} ${main_file})

    target_compile_definitions(${test_name} PRIVATE ${GNEISS_COMPILE_DEFINITIONS})
    target_compile_options(${test_name} PRIVATE ${GNEISS_CMAKE_C_FLAGS})
    target_include_directories(${test_name} PRIVATE ${GNEISS_INCLUDE_DIR})

    set(expect_failure FALSE)
    set(ignore_memory FALSE)
    foreach (argument IN LISTS ARGN)
        if ("${argument}" STREQUAL "EXPECT_FAILURE")
            set(expect_failure TRUE)
            continue()
        endif ()
        if ("${argument}" STREQUAL "IGNORE_MEMORY")
            set(ignore_memory TRUE)
            continue()
        endif ()
        list(APPEND link_libraries ${argument})
    endforeach ()

    target_link_libraries(${test_name} PRIVATE ${link_libraries})

    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${runtime_output_directory})

    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    if (expect_failure)
        set_tests_properties(${test_name} PROPERTIES WILL_FAIL TRUE)
        update_expect_failure_list_file(${test_name})
    endif ()
    if (ignore_memory)
        update_ignore_memory_list_file(${test_name})
    endif ()
endfunction()

# --------------------------------------------------------------------------- #
# Unit Tests                                                                  #
# --------------------------------------------------------------------------- #

# Create a unit test without the unit test library. This is really only useful for the logger
#   and the other libraries which the unit test library depends on
function(create_unit_test_plain main_file)
    create_test(${main_file} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ut" ${ARGN})
endfunction()

# Create a unit test which uses the unit testing utility library
function(create_unit_test main_file)
    # create_unit_test_plain(${main_file} UTIL_UnitTest ${ARGN})
    create_unit_test_plain(${main_file} ${ARGN})
endfunction()

# --------------------------------------------------------------------------- #
# Functional Tests                                                            #
# --------------------------------------------------------------------------- #

function(create_functional_test main_file)
    create_test(${main_file} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ft ${ARGN})
endfunction()
