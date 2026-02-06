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

int noop(void) {
    return 0;
}

int main(void) {
    int result =
        validate() ||
        noop()
    ;

    return result;
}
