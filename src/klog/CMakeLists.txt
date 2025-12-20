# =========================================================================== #
# Klog                                                                        #
# =========================================================================== #

add_library(klog SHARED ${GNEISS_INCLUDE_DIR}/klog/klog.h klog.c)
target_include_directories(klog PUBLIC ${GNEISS_INCLUDE_DIR})
target_compile_options(klog PUBLIC ${GNEISS_CMAKE_C_FLAGS})
target_compile_definitions(klog PUBLIC ${GNEISS_COMPILE_DEFINITIONS})
target_compile_definitions(klog PUBLIC ${GNEISS_COMPILE_DEFINITIONS})

