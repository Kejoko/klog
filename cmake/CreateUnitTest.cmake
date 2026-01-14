# =========================================================================== #
# Functionality for easily creating unit tests                                #
# =========================================================================== #

# Create a unit test without the unit test library. This is really only useful for the logger
#   and the other libraries which the unit test library depends on
function(create_unit_test_plain main_file)
    message(STATUS "Creating plain unit test for ${main_file}")
    get_filename_component(test_name ${main_file} NAME_WE)
    message(STATUS "Test name: ${test_name}")
    add_executable(${test_name} ${main_file})

    target_compile_definitions(${test_name} PRIVATE ${GNEISS_COMPILE_DEFINITIONS})
    target_compile_options(${test_name} PRIVATE ${GNEISS_CMAKE_C_FLAGS})
    target_include_directories(${test_name} PRIVATE ${GNEISS_INCLUDE_DIR})

    set(link_libraries ${ARGN})
    target_link_libraries(${test_name} PRIVATE ${link_libraries})

    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ut)

    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

# Create a unit test which uses the unit testing utility library
function(create_unit_test main_file)
    # create_unit_test_plain(${main_file} UTIL_UnitTest ${ARGN})
    create_unit_test_plain(${main_file} ${ARGN})
endfunction()
