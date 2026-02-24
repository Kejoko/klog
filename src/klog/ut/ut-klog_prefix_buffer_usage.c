#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"

#include "../klog_state.h"
#include "../klog_format.h"

int no_async_param(void) {
    const uint32_t logger_name_length = 6;
    KlogFormatInfo format_info = {logger_name_length, 10, 0, false, false};
    KlogConsoleInfo console_info = {KLOG_LEVEL_TRACE, false};
    klog_initialize(5, format_info, NULL, &console_info, NULL);

    if (g_klog_state.prefix_element_index != 0) {
        printf("Klog prefix element index should be 0 before anything is logged\n");
        return 1;
    }

    if (g_klog_state.prefix_element_count != 1) {
        printf("Klog prefix element count should be 1 if no async parameter is provided\n");
        return 1;
    }

    const uint32_t prefix_size = klog_format_prefix_length_get(false, false, logger_name_length, false, 0);
    if (g_klog_state.prefix_file_size != prefix_size) {
        printf("Klog file prefix size should be %d, but actually is %d\n", prefix_size, g_klog_state.prefix_console_size);
        return 1;
    }
    if (g_klog_state.prefix_console_size != prefix_size) {
        printf("Klog console prefix size should be %d, but actually is %d\n", prefix_size, g_klog_state.prefix_console_size);
        return 1;
    }

    char* empty_buffer = malloc(prefix_size);
    memset(empty_buffer, 0, prefix_size);
    if (g_klog_state.b_prefixes_file == NULL) {
        printf("Klog file prefix buffer is not initialized\n");
        return 1;
    }
    if (g_klog_state.b_prefixes_console == NULL) {
        printf("Klog console prefix buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_file, empty_buffer, prefix_size)) {
        printf("Klog file prefix buffer should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, empty_buffer, prefix_size)) {
        printf("Klog console prefix buffer should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p = klog_logger_create("dummy!");
    klog_logger_level_set(p, KLOG_LEVEL_INFO);
    klog_info(p, "test");

    if (g_klog_state.prefix_element_index != 0) {
        printf("Klog prefix element index should still be 0 because the buffer only has 1 element\n");
        return 1;
    }

    char* full_buffer = "[dummy!] [info ] ";
    if (memcmp(g_klog_state.b_prefixes_file, full_buffer, prefix_size)) {
        printf("Klog file prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_file);
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_buffer, prefix_size)) {
        printf("Klog console prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_console);
        return 1;
    }
    
    free(empty_buffer);

    klog_deinitialize();

    return 0;
}

int single_element(void) {
    const uint32_t logger_name_length = 8;
    KlogFormatInfo format_info = {logger_name_length, 10, 0, false, false};
    KlogAsyncInfo async_info = {1, 3};
    KlogConsoleInfo console_info = {KLOG_LEVEL_INFO, false};
    klog_initialize(5, format_info, &async_info, &console_info, NULL);

    if (g_klog_state.prefix_element_index != 0) {
        printf("Klog prefix element index should be 0 before anything is logged\n");
        return 1;
    }

    if (g_klog_state.prefix_element_count != 1) {
        printf("Klog prefix element count should be 1 if only 1 queue element is specified\n");
        return 1;
    }

    const uint32_t prefix_size = klog_format_prefix_length_get(false, false, logger_name_length, false, 0);
    if (g_klog_state.prefix_file_size != prefix_size) {
        printf("Klog file prefix size should be %d, but actually is %d\n", prefix_size, g_klog_state.prefix_console_size);
        return 1;
    }
    if (g_klog_state.prefix_console_size != prefix_size) {
        printf("Klog console prefix size should be %d, but actually is %d\n", prefix_size, g_klog_state.prefix_console_size);
        return 1;
    }

    char* empty_buffer = malloc(prefix_size);
    memset(empty_buffer, 0, prefix_size);
    if (g_klog_state.b_prefixes_file == NULL) {
        printf("Klog file prefix buffer is not initialized\n");
        return 1;
    }
    if (g_klog_state.b_prefixes_console == NULL) {
        printf("Klog console prefix buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_file, empty_buffer, prefix_size)) {
        printf("Klog file prefix buffer should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, empty_buffer, prefix_size)) {
        printf("Klog console prefix buffer should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p = klog_logger_create("dummy123");
    klog_logger_level_set(p, KLOG_LEVEL_INFO);
    klog_info(p, "test");

    if (g_klog_state.prefix_element_index != 0) {
        printf("Klog prefix element index should still be 0 because the buffer only has 1 element\n");
        return 1;
    }

    char* full_buffer = "[dummy123] [info ] ";
    if (memcmp(g_klog_state.b_prefixes_file, full_buffer, prefix_size)) {
        printf("Klog file prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_file);
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_buffer, prefix_size)) {
        printf("Klog console prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_console);
        return 1;
    }
    
    free(empty_buffer);

    klog_deinitialize();

    return 0;
}

int multiple_elements(void) {
    const uint32_t num_elements = 3;
    const uint32_t num_loggers = 4;
    const uint32_t logger_name_length = 3;
    KlogFormatInfo format_info = {logger_name_length, 10, 0, false, false};
    KlogAsyncInfo async_info = {num_elements, 3};
    KlogConsoleInfo console_info = {KLOG_LEVEL_DEBUG, false};
    klog_initialize(num_loggers, format_info, &async_info, &console_info, NULL);

    if (g_klog_state.prefix_element_index != 0) {
        printf("Klog prefix element index should be 0 before anything is logged\n");
        return 1;
    }

    if (g_klog_state.prefix_element_count != num_elements) {
        printf("Klog prefix element count should be %d, but is %d\n", num_elements, g_klog_state.prefix_element_count);
        return 1;
    }

    const uint32_t prefix_size = klog_format_prefix_length_get(false, false, logger_name_length, false, 0);
    if (g_klog_state.prefix_file_size != prefix_size) {
        printf("Klog file prefix size should be %d, but actually is %d\n", prefix_size, g_klog_state.prefix_console_size);
        return 1;
    }
    if (g_klog_state.prefix_console_size != prefix_size) {
        printf("Klog console prefix size should be %d, but actually is %d\n", prefix_size, g_klog_state.prefix_console_size);
        return 1;
    }

    char* empty_buffer = malloc(prefix_size);
    memset(empty_buffer, 0, prefix_size);
    if (g_klog_state.b_prefixes_file == NULL) {
        printf("Klog file prefix buffer is not initialized\n");
        return 1;
    }
    if (g_klog_state.b_prefixes_console == NULL) {
        printf("Klog console prefix buffer is not initialized\n");
        return 1;
    }

    /* First logging statement */

    if (memcmp(g_klog_state.b_prefixes_file, empty_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 0 should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, empty_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 0 should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p = klog_logger_create("dum");
    klog_logger_level_set(p, KLOG_LEVEL_INFO);
    klog_info(p, "test");

    if (g_klog_state.prefix_element_index != 1) {
        printf("Klog prefix element index should be 1 after the first log\n");
        return 1;
    }

    char* full_buffer = "[dum] [info ] ";
    if (memcmp(g_klog_state.b_prefixes_file, full_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_file);
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_console);
        return 1;
    }

    /* Second logging statement */

    if (memcmp(g_klog_state.b_prefixes_file + prefix_size, empty_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 1 should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size, empty_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 1 should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p2 = klog_logger_create("two");
    klog_logger_level_set(p2, KLOG_LEVEL_TRACE);
    klog_trace(p2, "this won't get logged due to the console's min level at debug, and no file logger");
    klog_debug(p2, "test number 2");

    if (g_klog_state.prefix_element_index != 2) {
        printf("Klog prefix element index should be 2 after the second log\n");
        return 1;
    }

    char* full_buffer_2 = "[two] [debug] ";
    if (memcmp(g_klog_state.b_prefixes_file + prefix_size, full_buffer_2, prefix_size)) {
        printf("Klog file prefix buffer element 1 should contain \"%s\" after logging at trace level, but instead it contains \"%s\"\n", full_buffer_2, g_klog_state.b_prefixes_file + prefix_size);
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size, full_buffer_2, prefix_size)) {
        printf("Klog console prefix buffer element 1 should contain \"%s\" after logging at trace level, but instead it contains \"%s\"\n", full_buffer_2, g_klog_state.b_prefixes_console + prefix_size);
        return 1;
    }
    
    /* Third logging statement */

    if (memcmp(g_klog_state.b_prefixes_file + prefix_size * 2, empty_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 2 should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size * 2, empty_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 2 should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p3 = klog_logger_create("3");
    klog_logger_level_set(p3, KLOG_LEVEL_WARN);
    klog_info(p3, "this won't get logged due to the logger's level");
    klog_error(p3, "test 3");

    if (g_klog_state.prefix_element_index != 0) {
        printf("Klog prefix element index should be 0 after the third log due to overflow\n");
        return 1;
    }

    char* full_buffer_3 = "[3  ] [ERROR] ";
    if (memcmp(g_klog_state.b_prefixes_file + prefix_size * 2, full_buffer_3, prefix_size)) {
        printf("Klog file prefix buffer element 2 should contain \"%s\" after logging at error level, but instead it contains \"%s\"\n", full_buffer_3, g_klog_state.b_prefixes_file + prefix_size * 2);
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size * 2, full_buffer_3, prefix_size)) {
        printf("Klog console prefix buffer element 2 should contain \"%s\" after logging at error level, but instead it contains \"%s\"\n", full_buffer_3, g_klog_state.b_prefixes_console + prefix_size * 2);
        return 1;
    }

    /* Fourth logging statement */

    if (memcmp(g_klog_state.b_prefixes_file, full_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 0 should contain \"%s\" before logging after overflow at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_file);
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 0 should contain \"%s\" before logging after overflow at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_file);
        return 1;
    }

    klog_info(p, "test, again!");

    if (g_klog_state.prefix_element_index != 1) {
        printf("Klog prefix element index should be 1 after the first log\n");
        return 1;
    }

    if (memcmp(g_klog_state.b_prefixes_file, full_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_file);
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n", full_buffer, g_klog_state.b_prefixes_console);
        return 1;
    }

    /* Clean up */

    free(empty_buffer);

    klog_deinitialize();

    return 0;
}

int noop(void) {
    return 0;
}

int main(void) {
    int result =
        no_async_param() ||
        single_element() ||
        multiple_elements() ||
        noop()
    ;

    return result;
}
