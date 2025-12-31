# =========================================================================== #
# Klog                                                                        #
# =========================================================================== #

add_library(
    klog
    SHARED
    ${GNEISS_INCLUDE_DIR}/klog/klog.h
    klog.c
    klog_impl_state.h
    klog_impl_state.c
    klog_impl_initialization.h
    klog_impl_initialization.c
    klog_impl_message_creation.h
    klog_impl_message_creation.c
)
target_include_directories(klog PUBLIC ${GNEISS_INCLUDE_DIR})
target_compile_options(klog PUBLIC ${GNEISS_CMAKE_C_FLAGS})
target_compile_definitions(klog PUBLIC ${GNEISS_COMPILE_DEFINITIONS})
target_compile_definitions(klog PUBLIC ${GNEISS_COMPILE_DEFINITIONS})

