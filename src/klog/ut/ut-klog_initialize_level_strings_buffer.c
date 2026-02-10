#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_initialize.h"

int validate(void) {
    char* b_level_strings = klog_initialize_level_strings_buffer();

    int result = 0;

    if (!result && !b_level_strings) {
        printf("The resultant level strings buffer is not valid\n");
        result = 1;
    }

    const char* s_expected = "off  FATALERRORWARN info debugtrace";
    uint32_t expected_length = strlen(s_expected);
    int strcmp_result = strncmp(s_expected, b_level_strings, expected_length);
    if (!result && strcmp_result) {
        printf("Buffer's contents do not equal expected (strcmp = %d)\n", strcmp_result);

        printf("  Expected: %s\n", s_expected);
        printf("  Actual  : %.*s\n", (int)strlen(s_expected), b_level_strings);
        result = 1;
    }

    free(b_level_strings);
    return result;
}

int validate_colors(void) {
    char* b_level_strings_colored = klog_initialize_colored_level_strings_buffer();

    int result = 0;

    if (!result && !b_level_strings_colored) {
        printf("The resultant level strings buffer is not valid\n");
        result = 1;
    }

    const char* s_expected = "\x1b[37moff  \x1b[0m\x1b[41mFATAL\x1b[0m\x1b[31mERROR\x1b[0m\x1b[33mWARN \x1b[0m\x1b[32minfo \x1b[0m\x1b[36mdebug\x1b[0m\x1b[37mtrace\x1b[0m";
    uint32_t expected_length = strlen(s_expected);
    int strcmp_result = strncmp(s_expected, b_level_strings_colored, expected_length);
    if (!result && strcmp_result) {
        printf("Buffer's contents do not equal expected (strcmp = %d)\n", strcmp_result);

        printf("  Expected: %s\n", s_expected);
        printf("  Actual  : %.*s\n", (int)strlen(s_expected), b_level_strings_colored);
        result = 1;
    }

    free(b_level_strings_colored);
    return result;
}

int noop(void) {
    return 0;
}

int main(void) {
    int result =
        validate() ||
        validate_colors() ||
        noop()
    ;

    return result;
}
