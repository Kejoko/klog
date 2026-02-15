# =========================================================================== #
# Klog                                                                        #
# =========================================================================== #

add_library(
    klog SHARED
    ${GNEISS_INCLUDE_DIR}/klog/klog.h
    klog.c
    klog_state.h
    klog_state.c
    klog_constants.h
    klog_constants.c
    klog_handle.h
    klog_debug_util.h
    klog_debug_util.c
    klog_platform.h
    klog_platform.c
    klog_initialize.h
    klog_initialize.c
    klog_format.h
    klog_format.c
    klog_output.h
    klog_output.c
)

target_include_directories(klog PUBLIC ${GNEISS_INCLUDE_DIR})
target_compile_options(klog PUBLIC ${GNEISS_CMAKE_C_FLAGS})
target_compile_definitions(klog PUBLIC ${GNEISS_COMPILE_DEFINITIONS})

if (KLOG_BUILD_TESTS)
    create_unit_test_plain(ut/ut-klog_format_logger_name.c klog)
    create_unit_test_plain(ut/ut-klog_format_prefix.c klog)
    create_unit_test_plain(ut/ut-klog_format_source_location.c klog)
    create_unit_test_plain(ut/ut-klog_format_split_strings.c klog)
    create_unit_test_plain(ut/ut-klog_initialize_parameters_are_valid.c klog)
    create_unit_test_plain(ut/ut-klog_initialize_level_strings_buffer.c klog)
    create_unit_test_plain(ut/ut-klog_logger_create.c klog)
    create_unit_test_plain(ut/ut-klog_logger_set_level.c klog)
    create_unit_test_plain(ut/ut-klog_message_max_length.c klog)

    create_functional_test(ft/ft-klog_basic.c klog)
    create_functional_test(ft/ft-klog_console.c klog)
    create_functional_test(ft/ft-klog_file.c klog)
    create_functional_test(ft/ft-klog_fail_uninitialized.c klog EXPECT_FAILURE)
    create_functional_test(ft/ft-klog_fail_logger_limit.c klog EXPECT_FAILURE IGNORE_MEMORY)
    create_functional_test(ft/ft-klog_fail_level.c klog EXPECT_FAILURE IGNORE_MEMORY)

    # create_functional_test(ft/ft-klog_stress_st_sync.c klog)
endif ()
