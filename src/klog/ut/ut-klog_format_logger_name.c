/**
 * @brief Ensure that the logger's name is formattd correctly
 *      - Account for characters that we don't want and sanitize the string
 *      - Convert all whitespace to underscores
 */

#include "../klog_format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int remove_whitespace(
    void
) {
    printf("Creating inputs\n");
    const char*    s_input = "a b\tc\nd\be\rf  g\n\t\t h";
    const uint32_t length  = strlen(s_input);

    /* Length / size calculations                   */
    /* 00 +                  0123456789             */
    /* 10 +                            0123456789   */
    const char* s_expected = "a_b_c_d_e_f__g____h";

    printf("Mallocing output buffer for %d bytes, and memsetting to spaces\n", length);
    char* s_actual = malloc(length);
    memset(s_actual, ' ', length);

    printf("Formatting logger name\n");
    klog_format_logger_name(s_input, length, s_actual, length);

    printf("Performing strcmp\n");
    int result = strcmp(s_input, s_expected);
    printf("Strcmp result: %d\n", result);

    if (result) {
        printf("Result is %d - which is bad?\n", result);

        uint32_t mismatch_count = 0;
        for (uint32_t i = 0; i < length; ++i) {
            printf("Checking chars at index: %d\n", i);
            if (s_actual[i] == s_expected[i]) {
                continue;
            }

            mismatch_count = mismatch_count + 1;
            printf("%d: actual '%c' != expected '%c'\n", i, s_actual[i], s_expected[i]);
        }
        printf("Total number of mismatching characters: %d\n", mismatch_count);

        /* Override the strcmp result. Why does it return -63 when the strings are equal? */
        if (mismatch_count == 0) {
            result = 0;
        }

        printf("Strcmp result: %d\n",                    result);
        printf("Actual   formatted logger name: %*.s\n", length, s_actual);
        printf("Expected formatted logger_name: %s\n",   s_expected);
    }
    printf("All good!\n");

    free((char*)s_actual);

    return result;
}

int extra_length(
    void
) {
    const char*    s_input = "a b";
    const uint32_t length  = 6;

    const char* s_expected = "a_b   ";

    char* s_actual = malloc(length);
    memset(s_actual, ' ', length);
    klog_format_logger_name(s_input, 3, s_actual, length);

    int result = strcmp(s_input, s_expected);

    if (result) {
        uint32_t mismatch_count = 0;
        for (uint32_t i = 0; i < length; ++i) {
            if (s_actual[i] == s_expected[i]) {
                continue;
            }

            mismatch_count = mismatch_count + 1;
            printf("%d: actual '%c' != expected '%c'\n", i, s_actual[i], s_expected[i]);
        }
        printf("Total number of mismatching characters: %d\n", mismatch_count);

        /* Override the strcmp result. Why does it return -63 when the strings are equal? */
        if (mismatch_count == 0) {
            result = 0;
        }

        printf("Strcmp result: %d\n",                    result);
        printf("Actual   formatted logger name: %*.s\n", length, s_actual); /* Not null terminated so we need to specify length manually */
        printf("Expected formatted logger_name: %s\n",   s_expected);
    }

    free((char*)s_actual);

    return result;
}

int noop(
    void
) {
    return 0;
}

int main(
    void
) {
    return remove_whitespace() || extra_length() || noop();
}
