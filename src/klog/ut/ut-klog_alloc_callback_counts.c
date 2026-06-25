/**
 * @brief Ensure the allocation/free callbacks get called a specific number of times, according to
 * the actual number of allocations we should be making. Also ensure the alloc and free callbacks
 * are called the same number of times as eachother.
 */

#include <stdlib.h>
#include <stdint.h>

#include "klog/klog.h"

#include "../klog_state.h"
#include "../klog_handle.h"

/**
 * @brief These are the counters for how many times each callback has been invoked.
 */
uint32_t g_alloc_counter = 0;
uint32_t g_free_counter  = 0;

/**
 * @brief These are the wrappers which increment the counter
 */
void* malloc_wrapper(
    const size_t size
) {
    void* result = malloc(size);

    printf("Allocation index %d - size %ld bytes - result %p\n", g_alloc_counter, size, result);

    g_alloc_counter += 1;

    return result;
}

void free_wrapper(
    void* p
) {
    printf("Free index %d - freeing %p\n", g_free_counter, p);
    g_free_counter += 1;
    free(p);
}

int check(
    void
) {
    /* Reset the counters before the test */
    g_alloc_counter = 0;
    g_free_counter  = 0;

    const uint32_t max_number_loggers = 2;
    const uint32_t max_name_length    = 3;

    const KlogAllocInfo alloc_info = { &malloc_wrapper, &free_wrapper };

    klog_initialize(max_number_loggers, (KlogFormatInfo) { max_name_length, 10, 0, false, false }, NULL, NULL, NULL, &alloc_info);

    /* check post initialization values */
    const uint32_t alloc_counter_init_expected = 10;
    if (g_alloc_counter != alloc_counter_init_expected) {
        printf("Alloc counter is %d after initialization when it should be %d\n", g_alloc_counter, alloc_counter_init_expected);
        return 1;
    }
    const uint32_t free_counter_init_expected = 0;
    if (g_free_counter != free_counter_init_expected) {
        printf("Free counter is %d after initialization when it should be %d\n", g_free_counter, free_counter_init_expected);
        return 1;
    }

    /* create loggers because this does some allocation for now */
    printf("Creating logger 0\n");
    const KlogLoggerHandle* p_handle_0 = klog_logger_create("ABC");
    printf("Creating logger 1\n");
    const KlogLoggerHandle* p_handle_1 = klog_logger_create("DEF");

    /* logging should introduce no allocs or frees */
    printf("Logging with logger 0\n");
    klog(p_handle_0, KLOG_LEVEL_INFO, "Beep \"%s\"\n", "hello");
    printf("Logging with logger 1\n");
    klog(p_handle_1, KLOG_LEVEL_INFO, "Boop \"%s\"\n", "world");

    /* check post logger creation values */
    const uint32_t alloc_counter_created_expected = alloc_counter_init_expected + 2;
    if (g_alloc_counter != alloc_counter_created_expected) {
        printf("Alloc counter is %d after logger creations when it should be %d\n", g_alloc_counter, alloc_counter_created_expected);
        return 1;
    }
    const uint32_t free_counter_created_expected = free_counter_init_expected + 2;
    if (g_free_counter != free_counter_created_expected) {
        printf("Free counter is %d after logger creations when it should be %d\n", g_free_counter, free_counter_created_expected);
        return 1;
    }

    /* deinitialize because this should invoke free many times, but not alloc */
    klog_deinitialize();

    /* check post deinit values */
    const uint32_t alloc_counter_deinit_expected = alloc_counter_created_expected;
    if (g_alloc_counter != alloc_counter_deinit_expected) {
        printf("Alloc counter is %d after deinitializtion when it should be %d\n", g_alloc_counter, alloc_counter_deinit_expected);
        return 1;
    }
    const uint32_t free_counter_deinit_expected = free_counter_created_expected + 10;
    if (g_free_counter != free_counter_deinit_expected) {
        printf("Free counter is %d after deinitialization when it should be %d\n", g_free_counter, free_counter_deinit_expected);
        return 1;
    }

    /* The alloc and free callbacks should be used the same number of times */
    if (g_alloc_counter != g_free_counter) {
        printf("Alloc counter is %d while Free counter is %d. These should be the same\n", g_alloc_counter, g_free_counter);
        return 1;
    }

    return 0;
}

int main(
    void
) {
    return check();
}
