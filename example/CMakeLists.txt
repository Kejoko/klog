# =========================================================================== #
# Example executables                                                         #
# =========================================================================== #

include(CreateExampleExecutable)

if (KLOG_BUILD_EXAMPLES)
    create_example_executable(hello_world.c)
endif ()
