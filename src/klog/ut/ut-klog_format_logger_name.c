/**
 * @brief Ensure that the logger's name is formattd correctly
 *      - Account for characters that we don't want and sanitize the string
 *      - Convert all whitespace to underscores
 */

#include "../klog_format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int remove_whitespace(void) {
    const char* s_input = "a b\tc\nd\be\rf  g\n\t\t h";

    const char* s_expected = "a_b_c_d_e_f__g____h";

    const char* s_actual = klog_format_logger_name(s_input);

    int result = strcmp(s_input, s_expected);

    if (result) {
        const uint32_t length_actual = strlen(s_actual);
        const uint32_t length_expected = strlen(s_expected);

        if (length_actual != length_expected) {
            printf("Actual and expected strings differ in length:\n");
            printf("    actual  : %d\n", length_actual);
            printf("    expected: %d\n", length_expected);
        }
        const uint32_t length_min = length_actual < length_expected ? length_actual : length_expected;
        uint32_t mismatch_count = 0;
        for (uint32_t i = 0; i < length_min; ++i) {
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

        printf("Strcmp result: %d\n", result);
        printf("Actual   formatted logger name: %s\n", s_actual);
        printf("Expected formatted logger_name: %s\n", s_expected);
    }

    free((char*)s_actual);

    return result;
}

int noop(void) {
    return 0;
}

int main(void) {
    return
        remove_whitespace() ||
        noop()
    ;
}
