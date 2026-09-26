#ifndef KLOG_INITIALIZE_INCLUDED
#define KLOG_INITIALIZE_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "klog/klog.h"

/**
 * @brief Validate that the given configuration for klog is valid.
 * @param klog_is_initialized Whether or not klog is initialized.
 * @param logger_count_max    The maximum number of loggers allowed.
 * @param klog_format_info    Formating information.
 * @param p_klog_async_info   Async information.
 * @param p_klog_console_info Console logging information.
 * @param p_klog_file_info    File logging information.
 * @param p_klog_alloc_info   Allocation and free callback information.
 * @returns True if the configuration is valid, false if not.
 */
bool klog_initialize_are_parameters_valid(
    const bool             klog_is_initialized,
    const uint32_t         logger_count_max,
    const KlogFormatInfo   klog_format_info,
    const KlogAsyncInfo*   p_klog_async_info,
    const KlogConsoleInfo* p_klog_console_info,
    const KlogFileInfo*    p_klog_file_info,
    const KlogAllocInfo*   p_klog_alloc_info
);

/**
 * @brief Allocate and initialize a buffer of memory.
 * @details Allocate memory on the heap according to the alloc_cb callback.
 * @details The number of bytes allocate is determined by
 *      ((element_length_max + 1 if null_terminate) * number_elements).
 * @details After allocation is performed, the buffer is filled with the specified
 *      fill_char byte, via memset.
 * @param number_elements    The number of elements to allocate.
 * @param element_length_max The size of each element.
 * @param fill_char          The fill char to use after allocation.
 * @param null_terminate     Whether or not to null terminate each element. This adds
 *      1 byte to the size of each element.
 * @param alloc_cb           The allocation callback to use when allocating memory on the heap.
 * @returns A byte buffer allocated to the desired size and filled with the
 *      desired byte.
 */
char* klog_initialize_buffer(
    const uint32_t number_elements,
    const uint32_t element_length_max,
    const char     fill_char,
    const bool     null_terminate,
    void* (* const alloc_cb)(
        size_t size
    )
);

/**
 * @brief Allocate space for logger_count_max KlogLoggerHandles.
 * @details The size of the allocation is determined by sizeof(KlogLoggerHandle) *
 *      logger_count_max.
 * @param logger_count_max The number of logger handles to allocate space for.
 * @param alloc_cb         The allocation callback.
 * @returns A buffer of logger_count_max KlogLoggerHandles.
 */
KlogLoggerHandle* klog_initialize_logger_handle_array(
    const uint32_t logger_count_max,
    void* (* const alloc_cb)(
        size_t size
    )
);

/**
 * @brief Allocate a buffer for our logger's levels, and initialize them all to
 *      KLOG_LEVEL_OFF.
 * @details Each logger level only occupies one byte of space, so the total size of
 *      the heap allocation is logger_count_max bytes.
 * @param logger_count_max The number of levels to allocate space for.
 * @param alloc_cb         The allocation callback for allocating memory on the heap.
 * @returns A byte buffer allocated for logger_count_max logger levels.
 */
uint8_t* klog_initialize_logger_levels_array(
    const uint32_t logger_count_max,
    void* (* const alloc_cb)(
        size_t size
    )
);

/**
 * @brief Create the buffer stored in the global state which contains all of the strings
 *      representing our different log levels.
 * @details Allocates a packed buffer for G_klog_number_levels strings each of length
 *      G_klog_level_string_length bytes. The strings are not null terminated.
 * @param alloc_cb The heap allocation callback.
 * @returns A buffer of characters containing the packed colorized level strings.
 */
char* klog_initialize_level_strings_buffer(
    void* (*const alloc_cb)(
        size_t size
    )
);

/**
 * @brief Similar to klog_initialize_level_strings_buffer(), but for colored level
 *      strings so we have an additional couple of characters representing the color
 *      codes.
 * @details Allocates a packed buffer for G_klog_number_levels strings each of length
 *      G_klog_colored_level_string_length bytes. The strings are not null terminated.
 * @param alloc_cb The heap allocation callback.
 * @returns A buffer of characters containing the packed colorized level strings.
 */
char* klog_initialize_colored_level_strings_buffer(
    void* (*const alloc_cb)(
        size_t size
    )
);

/**
 * @brief the input filename must be null terminated.
 * @pre s_filename is non-NULL.
 * @param s_filename The name of the file to open.
 * @returns A pointer to the file created with the given filename.
 */
FILE* klog_initialize_file(
    const char* const s_filename
);

#endif /* KLOG_INITIALIZE_INCLUDED */
