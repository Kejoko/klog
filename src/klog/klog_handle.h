#ifndef KLOG_HANDLE_INCLUDED
#define KLOG_HANDLE_INCLUDED

#include <stdint.h>

/**
 * @brief The hidden handle structure. All that the user gets
 *      is a pointer to this with no visibility into the contents.
 */
struct KlogLoggerHandle {
    uint32_t value;
};

#endif /* KLOG_HANDLE_INCLUDED */
