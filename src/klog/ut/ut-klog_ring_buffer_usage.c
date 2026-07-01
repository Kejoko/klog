#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"

#include "../klog_state.h"
#include "../klog_format.h"

int no_async_param(
    void
) {
    const uint32_t  logger_name_length          = 6;
    const uint32_t  message_max_length          = 7;
    const uint32_t  message_total_length_length = message_max_length + 1;
    KlogFormatInfo  format_info                 = { logger_name_length, message_max_length, 0, false, false };
    KlogConsoleInfo console_info                = { KLOG_LEVEL_TRACE, false };
    klog_initialize(5, format_info, NULL, &console_info, NULL, NULL);

    if (g_klog_state.message_element_idx != 0) {
        printf("Klog message element index should be 0 before anything is logged\n");
        return 1;
    }

    if (g_klog_state.message_element_count != 1) {
        printf("Klog message element count should be 1 if no async parameter is provided\n");
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

    if (g_klog_state.message_formatted_max_size != message_total_length_length) {
        printf("Klog message max length should be %d but it is %d\n", message_total_length_length, g_klog_state.message_formatted_max_size);
        return 1;
    }

    char* empty_prefix_buffer = malloc(prefix_size);
    memset(empty_prefix_buffer, 0, prefix_size);
    if (g_klog_state.b_prefixes_file == NULL) {
        printf("Klog file prefix buffer is not initialized\n");
        return 1;
    }
    if (g_klog_state.b_prefixes_console == NULL) {
        printf("Klog console prefix buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_file, empty_prefix_buffer, prefix_size)) {
        printf("Klog file prefix buffer should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, empty_prefix_buffer, prefix_size)) {
        printf("Klog console prefix buffer should be all null characters but it is not\n");
        return 1;
    }

    char* empty_message_buffer = malloc(message_total_length_length);
    memset(empty_message_buffer, 0, message_total_length_length);
    if (g_klog_state.b_messages_formatted == NULL) {
        printf("Klog formatted message buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_messages_formatted, empty_message_buffer, message_total_length_length)) {
        printf("Klog formatted messages buffer should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p = klog_logger_create("dummy!");
    klog_logger_level_set(p, KLOG_LEVEL_INFO);
    klog_info(p, "test");

    if (g_klog_state.message_element_idx != 0) {
        printf("Klog message element index should still be 0 because the buffer only has 1 element\n");
        return 1;
    }

    char* full_prefix_buffer = "[dummy!] [info ] ";
    if (memcmp(g_klog_state.b_prefixes_file, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog file prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_file
        );
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog console prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_console
        );
        return 1;
    }

    free(empty_prefix_buffer);
    free(empty_message_buffer);

    klog_deinitialize();

    return 0;
}

int single_element(
    void
) {
    const uint32_t  logger_name_length   = 8;
    const uint32_t  message_max_length   = 11;
    const uint32_t  message_total_length = message_max_length + 1;
    const uint32_t  num_elements         = 1;
    KlogFormatInfo  format_info          = { logger_name_length, message_max_length, 0, false, false };
    KlogAsyncInfo   async_info           = { num_elements, 3 };
    KlogConsoleInfo console_info         = { KLOG_LEVEL_INFO, false };
    klog_initialize(5, format_info, &async_info, &console_info, NULL, NULL);

    if (g_klog_state.message_element_idx != 0) {
        printf("Klog message element index should be 0 before anything is logged\n");
        return 1;
    }

    if (g_klog_state.message_element_count != 1) {
        printf("Klog message element count should be 1 if only 1 queue element is specified\n");
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

    if (g_klog_state.message_formatted_max_size != message_max_length + 1) {
        printf("Klog message max length should be %d but it is %d\n", message_max_length + 1, g_klog_state.message_formatted_max_size);
        return 1;
    }

    char* empty_prefix_buffer = malloc(prefix_size);
    memset(empty_prefix_buffer, 0, prefix_size);
    if (g_klog_state.b_prefixes_file == NULL) {
        printf("Klog file prefix buffer is not initialized\n");
        return 1;
    }
    if (g_klog_state.b_prefixes_console == NULL) {
        printf("Klog console prefix buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_file, empty_prefix_buffer, prefix_size)) {
        printf("Klog file prefix buffer should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, empty_prefix_buffer, prefix_size)) {
        printf("Klog console prefix buffer should be all null characters but it is not\n");
        return 1;
    }

    char* empty_message_buffer = malloc(message_total_length);
    memset(empty_message_buffer, 0, message_total_length);
    if (g_klog_state.b_messages_formatted == NULL) {
        printf("Klog formatted message buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_messages_formatted, empty_message_buffer, message_total_length)) {
        printf("Klog formatted messages buffer should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p = klog_logger_create("dummy123");
    klog_logger_level_set(p, KLOG_LEVEL_INFO);
    klog_info(p, "test");

    if (g_klog_state.message_element_idx != 0) {
        printf("Klog message element index should still be 0 because the buffer only has 1 element\n");
        return 1;
    }

    char* full_prefix_buffer = "[dummy123] [info ] ";
    if (memcmp(g_klog_state.b_prefixes_file, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog file prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_file
        );
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog console prefix buffer should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_console
        );
        return 1;
    }

    free(empty_prefix_buffer);
    free(empty_message_buffer);

    klog_deinitialize();

    return 0;
}

int multiple_elements(
    void
) {
    const uint32_t  num_loggers          = 4;
    const uint32_t  logger_name_length   = 3;
    const uint32_t  message_max_length   = 7; /* good length to set manually if needed */
    const uint32_t  message_total_length = message_max_length + 1; /* +1 because each is null terminated */
    const uint32_t  num_elements         = 3;
    KlogFormatInfo  format_info          = { logger_name_length, message_max_length, 0, false, false };
    KlogAsyncInfo   async_info           = { num_elements, 3 };
    KlogConsoleInfo console_info         = { KLOG_LEVEL_DEBUG, false };
    klog_initialize(num_loggers, format_info, &async_info, &console_info, NULL, NULL);

    if (g_klog_state.message_element_idx != 0) {
        printf("Klog message element index should be 0 before anything is logged\n");
        return 1;
    }

    if (g_klog_state.message_element_count != num_elements) {
        printf("Klog message element count should be %d, but is %d\n", num_elements, g_klog_state.message_element_count);
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

    if (g_klog_state.message_formatted_max_size != message_total_length) {
        printf("Klog message max length should be %d but it is %d\n", message_total_length, g_klog_state.message_formatted_max_size);
        return 1;
    }

    char* empty_prefix_buffer_all = malloc(prefix_size * num_elements);
    memset(empty_prefix_buffer_all, 0, prefix_size * num_elements);
    if (g_klog_state.b_prefixes_file == NULL) {
        printf("Klog file prefix buffer is not initialized\n");
        return 1;
    }
    if (g_klog_state.b_prefixes_console == NULL) {
        printf("Klog console prefix buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_file, empty_prefix_buffer_all, prefix_size * num_elements)) {
        printf("Klog file prefix buffer should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, empty_prefix_buffer_all, prefix_size * num_elements)) {
        printf("Klog console prefix buffer should be all null characters but it is not\n");
        return 1;
    }

    char* empty_message_buffer_all = malloc(message_total_length * num_elements);
    memset(empty_message_buffer_all, 0, message_total_length * num_elements);
    if (g_klog_state.b_messages_formatted == NULL) {
        printf("Klog formatted message buffer is not initialized\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_messages_formatted, empty_message_buffer_all, message_total_length * num_elements)) {
        printf("Klog formatted messages buffer should be all null characters but it is not\n");
        return 1;
    }

    char* dirty_message_buffer_all = malloc(message_total_length * num_elements);
    memset(dirty_message_buffer_all, 'K', message_total_length * num_elements);
    memcpy(g_klog_state.b_messages_formatted, dirty_message_buffer_all, message_total_length * num_elements); /* Dirty all message buffers to validate clear logic */
    if (memcmp(g_klog_state.b_messages_formatted, dirty_message_buffer_all, message_total_length * num_elements)) {
        printf("Klog formatted messages buffer should be all dirty characters but it is not\n");
        return 1;
    }

    /* First logging statement */

    printf("Buffer before first log:\n");
    for (uint32_t byte_idx = 0; byte_idx < message_total_length * num_elements; ++byte_idx) {
        const char    byte   = g_klog_state.b_messages_formatted[byte_idx];
        const int16_t casted = (int16_t)byte;
        printf("%d = %d\n", byte_idx, casted);
    }

    char* dirty_message_buffer = malloc(message_total_length);
    memset(dirty_message_buffer, 'K', message_total_length);
    if (memcmp(g_klog_state.b_messages_formatted + (message_total_length * 0), dirty_message_buffer, message_total_length)) {
        printf(
            "Klog formatted message buffer element 0 should be dirty ('K') before logging anything, but instead it contains \"%.*s\"\n",
            message_total_length,
            g_klog_state.b_messages_formatted + (message_total_length * 0)
        );
        return 1;
    }

    char* empty_prefix_buffer = malloc(prefix_size);
    memset(empty_prefix_buffer, 0, prefix_size);
    if (memcmp(g_klog_state.b_prefixes_file, empty_prefix_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 0 should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, empty_prefix_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 0 should be all null characters but it is not\n");
        return 1;
    }

    char* empty_message_buffer = malloc(message_total_length);
    memset(empty_message_buffer, 0, message_total_length);

    const KlogLoggerHandle* p = klog_logger_create("dum");
    klog_logger_level_set(p, KLOG_LEVEL_INFO);
    klog_info(p, "1234567");

    if (g_klog_state.message_element_idx != 1) {
        printf("Klog message element index should be 1 after the first log\n");
        return 1;
    }

    char* full_prefix_buffer = "[dum] [info ] ";
    if (memcmp(g_klog_state.b_prefixes_file, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog file prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_file
        );
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog console prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_console
        );
        return 1;
    }

    if (memcmp(g_klog_state.b_messages_formatted, empty_message_buffer, message_total_length)) {
        printf(
            "Klog formatted message buffer element 0 should be empty (cleared) after logging at info level, but instead it contains \"%.*s\"\n",
            message_total_length,
            g_klog_state.b_messages_formatted
        );
        return 1;
    }

    /* Second logging statement */

    printf("Buffer after first log:\n");
    for (uint32_t byte_idx = 0; byte_idx < message_total_length * num_elements; ++byte_idx) {
        const char    byte   = g_klog_state.b_messages_formatted[byte_idx];
        const int16_t casted = (int16_t)byte;
        printf("%d = %d\n", byte_idx, casted);
    }

    printf(
        "!!!!!!!!Checking buffer starting %p through %p (%d bytes)\n",
        g_klog_state.b_messages_formatted + (message_total_length * 1),
        g_klog_state.b_messages_formatted + (message_total_length * 1) + message_total_length,
        message_max_length
    );
    if (memcmp(g_klog_state.b_messages_formatted + (message_total_length * 1), dirty_message_buffer, message_total_length)) {
        printf(
            "Klog formatted message buffer element 1 should be dirty ('K') after logging at info level 1 time, but instead it contains \"%.*s\"\n",
            message_total_length,
            g_klog_state.b_messages_formatted + (message_total_length * 1)
        );
        return 1;
    }

    if (memcmp(g_klog_state.b_prefixes_file + prefix_size, empty_prefix_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 1 should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size, empty_prefix_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 1 should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p2 = klog_logger_create("two");
    klog_logger_level_set(p2, KLOG_LEVEL_TRACE);
    klog_trace(p2, "this won't get logged due to the console's min level at debug, and no file logger");
    klog_debug(p2, "test number 2");

    if (g_klog_state.message_element_idx != 2) {
        printf("Klog message element index should be 2 after the second log\n");
        return 1;
    }

    char* full_prefix_buffer_2 = "[two] [debug] ";
    if (memcmp(g_klog_state.b_prefixes_file + prefix_size, full_prefix_buffer_2, prefix_size)) {
        printf(
            "Klog file prefix buffer element 1 should contain \"%s\" after logging at trace level, but instead it contains \"%s\"\n",
            full_prefix_buffer_2,
            g_klog_state.b_prefixes_file + prefix_size
        );
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size, full_prefix_buffer_2, prefix_size)) {
        printf(
            "Klog console prefix buffer element 1 should contain \"%s\" after logging at trace level, but instead it contains \"%s\"\n",
            full_prefix_buffer_2,
            g_klog_state.b_prefixes_console + prefix_size
        );
        return 1;
    }

    if (memcmp(g_klog_state.b_messages_formatted, empty_message_buffer, message_total_length)) {
        printf(
            "Klog formatted message buffer element 1 should be empty (cleared) after logging at info level 2 times, but instead it contains \"%.*s\"\n",
            message_total_length,
            g_klog_state.b_messages_formatted + (message_total_length * 1)
        );
        return 1;
    }

    /* Third logging statement */

    if (memcmp(g_klog_state.b_prefixes_file + prefix_size * 2, empty_prefix_buffer, prefix_size)) {
        printf("Klog file prefix buffer element 2 should be all null characters but it is not\n");
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size * 2, empty_prefix_buffer, prefix_size)) {
        printf("Klog console prefix buffer element 2 should be all null characters but it is not\n");
        return 1;
    }

    const KlogLoggerHandle* p3 = klog_logger_create("3");
    klog_logger_level_set(p3, KLOG_LEVEL_WARN);
    klog_info(p3, "this won't get logged due to the logger's level");
    klog_error(p3, "test 3");

    if (g_klog_state.message_element_idx != 0) {
        printf("Klog message element index should be 0 after the third log due to overflow\n");
        return 1;
    }

    char* full_prefix_buffer_3 = "[3  ] [ERROR] ";
    if (memcmp(g_klog_state.b_prefixes_file + prefix_size * 2, full_prefix_buffer_3, prefix_size)) {
        printf(
            "Klog file prefix buffer element 2 should contain \"%s\" after logging at error level, but instead it contains \"%s\"\n",
            full_prefix_buffer_3,
            g_klog_state.b_prefixes_file + prefix_size * 2
        );
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console + prefix_size * 2, full_prefix_buffer_3, prefix_size)) {
        printf(
            "Klog console prefix buffer element 2 should contain \"%s\" after logging at error level, but instead it contains \"%s\"\n",
            full_prefix_buffer_3,
            g_klog_state.b_prefixes_console + prefix_size * 2
        );
        return 1;
    }

    /* Fourth logging statement */

    if (memcmp(g_klog_state.b_prefixes_file, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog file prefix buffer element 0 should contain \"%s\" before logging after overflow at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_file
        );
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog console prefix buffer element 0 should contain \"%s\" before logging after overflow at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_file
        );
        return 1;
    }

    klog_info(p, "test, again!");

    if (g_klog_state.message_element_idx != 1) {
        printf("Klog message element index should be 1 after the first log\n");
        return 1;
    }

    if (memcmp(g_klog_state.b_prefixes_file, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog file prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_file
        );
        return 1;
    }
    if (memcmp(g_klog_state.b_prefixes_console, full_prefix_buffer, prefix_size)) {
        printf(
            "Klog console prefix buffer element 0 should contain \"%s\" after logging at info level, but instead it contains \"%s\"\n",
            full_prefix_buffer,
            g_klog_state.b_prefixes_console
        );
        return 1;
    }

    /* Clean up */

    free(empty_prefix_buffer_all);
    free(empty_prefix_buffer);
    free(empty_message_buffer_all);
    free(empty_message_buffer);
    free(dirty_message_buffer_all);
    free(dirty_message_buffer);

    klog_deinitialize();

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
    int result = no_async_param() || single_element() || multiple_elements() || noop();

    return result;
}
