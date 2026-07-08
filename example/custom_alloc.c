/**
 * @file custom_alloc.c
 * @brief An example executable showcasing how to use custom alloc and free logic.
 *      This is useful if you're using an arena allocate or some other sort of
 *      custom allocator with fancy logic.
 */

#include <stdio.h>
#include <stdlib.h>

#include "klog/klog.h"

uint32_t alloc_count = 0;
uint32_t free_count  = 0;

void* custom_alloc(
    const size_t size
) {
    alloc_count += 1;
    printf("Allocation count %d\n", alloc_count);
    return malloc(size);
}

void custom_free(
    void* p
) {
    free_count += 1;
    printf("Free count %d\n", free_count);
    free(p);
}

int main(
    void
) {
    KlogFormatInfo  format_info  = { 6, 100, 0, false, true };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    KlogAllocInfo   alloc_info   = { &custom_alloc, &custom_free }; /* Our custom allocator and free */
    klog_initialize(3, format_info, NULL, &console_info, NULL, &alloc_info);

    const KlogLoggerHandle* p_handle = klog_logger_create("alloc", 5);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);
    klog_trace(p_handle, "We are cool for custom allocating yippee!");

    klog_deinitialize();

    return 0;
}
