# =========================================================================== #
# Functionality for easily creating unit tests                                #
# =========================================================================== #

function(create_unit_test main_file)
    get_filename_component(test_name ${main_file} NAME_WE)
    add_executable(${test_name} ${main_file})

    target_compile_definitions(${test_name} PRIVATE ${GNEISS_COMPILE_DEFINITIONS})
    target_compile_options(${test_name} PRIVATE ${GNEISS_CMAKE_C_FLAGS})
    target_include_directories(${test_name} PRIVATE ${GNEISS_INCLUDE_DIR})

    set(link_libraries ${ARGN})
    # target_link_libraries(${test_name} PRIVATE ${link_libraries} UTIL_UnitTest)
    target_link_libraries(${test_name} PRIVATE ${link_libraries})

    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ut)

    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

