#ifndef KLOG_DEBUG_UTIL_INCLUDED
#define KLOG_DEBUG_UTIL_INCLUDED

/**
 * @brief For internal debugging. Gated printf().
 * @details This is just a wrapper around printf(), gated by the
 *      KLOG_DEBUG preprocessor definition.
 */
void kdprintf(
    const char* s_format,
    ...
);

#endif /* KLOG_DEBUG_UTIL_INCLUDED */
