# =========================================================================== #
# Example executables                                                         #
# =========================================================================== #

include(CreateExampleExecutable)

if (KLOG_BUILD_EXAMPLES)
    create_example_executable(async.c)
    create_example_executable(basic_console.c)
    create_example_executable(basic_file.c)
    create_example_executable(extra_formatting.c)
    create_example_executable(hello_world.c)
    create_example_executable(klog_version.c)
endif ()
