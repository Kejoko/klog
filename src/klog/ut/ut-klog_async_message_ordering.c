#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"

#include "../klog_state.c"

/*              00+               123456789    */
/*              10+                        012 */
const char*    g_prefix        = "[X] [debug] ";
const uint32_t g_prefix_length = 12;

const uint32_t g_num_messages        = 5000;
const uint32_t g_message_digit_count = 4;
char*          g_message_buffer      = "0000\n"; /* 1 character for each digit, and a newline */

char* populate_file(
    void
) {
    /* Initialize klog with async and file information */
    KlogFormatInfo format_info = { 1, 4, 0, false, false };
    KlogAsyncInfo  async_info  = { 5000, 1, false, false };
    KlogFileInfo   file_info   = { KLOG_LEVEL_TRACE, "async_message_ordering" };
    klog_initialize(1, format_info, &async_info, NULL, &file_info, NULL);

    /* Create a logger and log the messages */
    const KlogLoggerHandle* p_handle = klog_logger_create("X", 1);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);
    for (uint32_t idx_message = 0; idx_message < g_num_messages; ++idx_message) {
        klog(p_handle, KLOG_LEVEL_DEBUG, "%04d", idx_message);
    }

    /* Copy the filename and return it for later use */
    const uint32_t filename_length = strlen(g_klog_state.s_filename);
    char*          filename_copy   = malloc(filename_length + 1);
    filename_copy[filename_length] = '\0'; /* null terminator */
    memcpy(filename_copy, g_klog_state.s_filename, filename_length);
    printf("Filename =  %s\n", filename_copy);

    /* Deinitialize klog */
    klog_deinitialize();

    return filename_copy;
}

int check(
    void
) {
    /* Populate things with klog */
    char* filename = populate_file();
    if (!filename) {
        printf("Klog initialize did not create a valid filename for some reason?\n");
        return 1;
    }

    /* Open the file */
    FILE* const p_file = fopen(filename, "r");
    if (!p_file) {
        printf("Couldn't open file at %s\n", filename);
        return 1;
    }

    /* Get the size of the file */
    if (fseek(p_file, 0, SEEK_END) != 0) {
        printf("Problem when invoking fseek to put cursor at the end for getting the file size\n");
        return 1;
    }
    const int32_t file_size = ftell(p_file);
    if (file_size < 0) {
        printf("Problem when invoking ftell to get the file size\n");
        return 1;
    }
    const uint32_t line_size          = g_prefix_length + g_message_digit_count + 1; /* 12 for prefix, 4 for the digits + 1 for newline */
    const uint32_t file_size_expected = line_size * g_num_messages;
    if ((uint32_t)file_size != file_size_expected) {
        printf("Actual file size (%d) != expected file size (%d)\n", file_size, file_size_expected);
        return 1;
    }

    /* Reset the cursor so we can begin reading */
    if (fseek(p_file, 0, SEEK_SET) != 0) {
        printf("Problem when invoking fseek to put cursor at the beginning to prep for reading the file contents\n");
        return 1;
    }

    /* Read each line and validate the contents */
    char* buffer = malloc(line_size);
    for (uint32_t idx_message = 0; idx_message < g_num_messages; ++idx_message) {
        fread(buffer, line_size, 1, p_file); /* Read line_size "items", each of size 1 byte */
        /* printf("Read line %d: \"%.*s\"\n", idx_message, line_size, buffer); */

        /* Compare prefixes */
        if (strncmp(buffer, g_prefix, g_prefix_length) != 0) {
            printf(
                "Line %d does not contain the prefix: \"%.*s\" != \"%.*s\"\n",
                idx_message,
                g_prefix_length,
                buffer,
                g_prefix_length,
                g_prefix
            );
            return 1;
        }

        /* Compare actual value - first create the string representing the current formatted index */
        char* b_digits = malloc(g_message_digit_count + 1); /* Need space for sprintf's null char */
        sprintf(b_digits, "%04d", idx_message);
        if (strncmp(buffer + g_prefix_length, b_digits, g_message_digit_count) != 0) {
            printf(
                "Line %d does not contain the digits: \"%.*s\" != \"%.*s\"\n",
                idx_message,
                g_message_digit_count,
                buffer + g_prefix_length,
                g_message_digit_count,
                b_digits
            );
            return 1;
        }
        free(b_digits);

        /* Check newline at final character */
        if (buffer[line_size - 1] != '\n') {
            printf(
                "Line %d does not contain newline at final character index %d: '%c' (%d)\n",
                idx_message,
                line_size - 1,
                buffer[line_size - 1],
                buffer[line_size - 1]
            );
            return 1;
        }
    }

    /* Free the buffer, global filename, and close the file */
    free(buffer);
    fclose(p_file);
    free(filename);

    return 0;
}

int main(
    void
) {
    return check();
}
