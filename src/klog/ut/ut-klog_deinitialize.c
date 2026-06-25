/**
 * @brief Make sure we clean up correctly, so if we ever add a new field to klog_state,
 *      then we can catch it here if we forget to reset in in klog_deinitialize
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"
#include "../klog_state.h"

void* dummy_malloc(
    const size_t size
) {
    return malloc(size);
}

void dummy_free(
    void* p
) {
    free(p);
}

void print_memory(
    const void* const p_actual,
    const void* const p_expected,
    const uint32_t    size
) {
    for (uint32_t i = 0; i < size; ++i) {
        printf("%02x : %02x\n", ((uint8_t*)p_actual)[i], ((uint8_t*)p_expected)[i]);
    }
}

int check(
    void
) {
    KlogAsyncInfo   async_info   = { 100, 3 };
    KlogFormatInfo  format_info  = { 6, 100, 10, true, true };
    KlogConsoleInfo console_info = { KLOG_LEVEL_INFO, true };
    KlogFileInfo    file_info    = { KLOG_LEVEL_TRACE, "BASIC\tPREFIX" };
    klog_initialize(4, format_info, &async_info, &console_info, &file_info, NULL);

    /* Ensure allocation/free callbacks were defaulted correctly */
    if (g_klog_config.alloc.alloc_cb != &malloc) {
        printf("g_klog_config.alloc.alloc_cb should by default equal the address of malloc but it does not\n");
        return 1;
    }
    if (g_klog_config.alloc.free_cb != &free) {
        printf("g_klog_config.alloc.free_cb should by default equal the address of free but it does not\n");
        return 1;
    }

    /* Create the expected empty state */
    struct KlogConfig klog_config_empty = { 0 };
    struct KlogState  klog_state_empty  = { 0 };

    /* Check that we are not equivelant to null */
    if (memcmp(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty)) == 0) {
        printf("g_klog_config should not be empty after klog_initialize\n");
        print_memory(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty));
        return 1;
    }
    if (memcmp(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty)) == 0) {
        printf("g_klog_state should not be empty after klog_initialize\n");
        print_memory(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty));
        return 1;
    }

    /* Deinitialize */
    klog_deinitialize();

    /* Check that we **are** equivelant to null */
    if (memcmp(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty)) != 0) {
        printf("g_klog_config should be empty after klog_deinitialize\n");
        print_memory(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty));
        return 1;
    }
    if (memcmp(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty)) != 0) {
        printf("g_klog_state should be empty after klog_deinitialize\n");
        print_memory(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty));
        return 1;
    }

    return 0;
}

int callbacks(
    void
) {
    KlogFormatInfo format_info = { 6, 100, 10, true, true };
    KlogAllocInfo  alloc_info  = { &dummy_malloc, &dummy_free };
    klog_initialize(4, format_info, NULL, NULL, NULL, &alloc_info);

    /* Ensure allocation/free callbacks were set correctly */
    if (g_klog_config.alloc.alloc_cb != &dummy_malloc) {
        printf("g_klog_config.alloc.alloc_cb should equal the address of dummy_malloc but it does not\n");
        return 1;
    }
    if (g_klog_config.alloc.free_cb != &dummy_free) {
        printf("g_klog_config.alloc.free_cb should equal the address of dummy_free but it does not\n");
        return 1;
    }

    /* Create the expected empty state */
    struct KlogConfig klog_config_empty = { 0 };
    struct KlogState  klog_state_empty  = { 0 };

    /* Check that we are not equivelant to null */
    if (memcmp(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty)) == 0) {
        printf("g_klog_config should not be empty after klog_initialize\n");
        print_memory(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty));
        return 1;
    }
    if (memcmp(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty)) == 0) {
        printf("g_klog_state should not be empty after klog_initialize\n");
        print_memory(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty));
        return 1;
    }

    /* Deinitialize */
    klog_deinitialize();

    /* Check that we **are** equivelant to null */
    if (memcmp(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty)) != 0) {
        printf("g_klog_config should be empty after klog_deinitialize\n");
        print_memory(&g_klog_config, &klog_config_empty, sizeof(klog_config_empty));
        return 1;
    }
    if (memcmp(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty)) != 0) {
        printf("g_klog_state should be empty after klog_deinitialize\n");
        print_memory(&g_klog_state, &klog_state_empty, sizeof(klog_state_empty));
        return 1;
    }

    return 0;
}

int noop(
    void
) {
    return 0;
}

int main(
    void
) {
    int result = check() || callbacks() || noop();

    return result;
}
