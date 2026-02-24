/**
 * @brief Ensure that the time string is formatted correctly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_constants.h"
#include "../klog_format.h"

int check_general_format(void) {
    char* b = malloc(G_klog_time_string_length + 1);

    KlogString result = klog_format_time(b);

    if (result.s == NULL) {
        printf("Resulting time string is NULL when it should not be\n");
        return 1;
    }

    const uint32_t length_expected = G_klog_time_string_length;
    if (result.length != length_expected) {
        printf("Resulting time string has length of %d, when it should have a length of %d\n", result.length, length_expected);
        return 1;
    }

    const uint32_t colon_indices_length = 4;
    const uint32_t colon_indices[] = {3, 6, 9, 12};

    for (uint32_t i = 0; i < colon_indices_length; ++i) {
        const uint32_t current_index = colon_indices[i];
        const char actual_character = result.s[current_index];
        if (actual_character != ':') {
            printf("Index %d should be a ':' but instead it is a '%c'\n", current_index, actual_character);
            return 1;
        }
    }

    const char final_character = result.s[length_expected];
    if (final_character != '\0') {
        printf("Resulting KlogString's final character (index %d, reported length + 1) should be null terminator, but instead it is '%c'\n", length_expected, final_character);
        return 1;
    }

    uint32_t day_year = 0;
    uint32_t hour = 0;
    uint32_t minute = 0;
    uint32_t second = 0;
    uint32_t microsecond = 0;
    const int scanf_success = sscanf(result.s, "%3u:%2u:%2u:%2u:%6u", &day_year, &hour, &minute, &second, &microsecond);
    if (scanf_success != 5) {
        printf("Resulting string contained %d numerical values instead of 5\n", scanf_success);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int noop(void) {
    return 0;
}

int main(void) {
    return
        check_general_format() ||
        noop()
    ;
}
