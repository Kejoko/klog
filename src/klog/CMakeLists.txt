# =========================================================================== #
# Klog                                                                        #
# =========================================================================== #

add_library(
    klog
    SHARED
    ${GNEISS_INCLUDE_DIR}/klog/klog.h
    klog.c
    klog_state.h
    klog_state.c
    klog_constants.h
    klog_constants.c
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
target_compile_definitions(klog PUBLIC ${GNEISS_COMPILE_DEFINITIONS})

create_unit_test_plain(ut/test-klog_format_split_strings.c klog)
