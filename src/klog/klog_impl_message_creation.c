#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "klog/klog.h"
#include "./klog_impl_message_creation.h"
#include "./klog_impl_state.h"

pid_t klog_impl_get_current_thread_id(void) {
    /* @todo kjk 2025/12/31 Call this once per thread so we can avoid switching into kernel mode repeatedly */
    return syscall(SYS_gettid);
}

const char* klog_impl_get_level_string(const enum klog_level_e requested_level) {
    /* @todo kjk 2025/12/31 Take level string buffer as parameter? Ensure it is correctly sized? */
    const uint32_t i_level_string = G_klog_level_string_length * requested_level;
    return &gp_klog_level_strings[i_level_string];
}

split_format_t klog_impl_split_format_strings(char *full_format) {
    printf("Splitting %s\n", full_format);

    split_format_t result = {1, &full_format, 0, 0};

    return result;
}

char* klog_impl_create_message(const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, va_list args) {
    const pid_t thread_id = klog_impl_get_current_thread_id();
    const char* logger_name = &gp_klog_logger_names[logger_handle.value * g_klog_logger_name_max_length];
    const char* level_string = klog_impl_get_level_string(requested_level);
    /**
     * @brief String format is "XXXXX [logger_name] [level] <message>"
     * @details So we have
     *      - 5 characters for thread id
     *      - 1 for space
     *      - 2 for brackets around logger name
     *      - logger name length is determined by user
     *      - 1 for space
     *      - 2 for brackets around logger level
     *      - 8 for level
     *      - 1 for space
     *      - the message
     *      In total: (5+1+2+1+2+8+1 + logger name) + message
     *      In total: (20 + logger name) + message
     */

    /* We need to make a copy of the args (for the second vsnprintf call) before we consume them with the first vsnprintf call */
    va_list args_copy;
    va_copy(args_copy, args);

    /* Calculate the length of the input message */
    /* +1 for null termination */
    const int input_message_length = vsnprintf(0, 0, format, args) + 1;

    /* Actually create and format the input message, now that we know the length */
    char* input_message = malloc(input_message_length);
    vsnprintf(input_message, input_message_length, format, args_copy);
    va_end(args_copy);

    /* Create the resultant message, with the klog prefixes inserted at the front */
    const uint32_t prefix_length = 20 + g_klog_logger_name_max_length;
    const uint32_t total_message_length = prefix_length + input_message_length;
    char* total_message = malloc(total_message_length);
    sprintf(total_message, "%5d [%.*s] [%.*s] %s", thread_id, g_klog_logger_name_max_length, logger_name, G_klog_level_string_length, level_string, input_message);

    free(input_message);

    return total_message;
}
