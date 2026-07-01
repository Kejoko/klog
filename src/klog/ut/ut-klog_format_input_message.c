/**
 * @brief Ensure that we are formatting messages correctly. The primary check
 *      here is to ensure that we are adhering to the maximum length
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_format.h"

uint32_t wrapper(
    char* const       b_output,
    const uint32_t    size_output,
    const char* const s_format,
    ...
) {
    va_list p_args;
    va_start(p_args, s_format);

    const uint32_t result = klog_format_input_message(
        b_output,
        size_output,
        s_format,
        p_args
    );

    va_end(p_args);

    return result;
}

void print_buffer(
    const char*    buffer,
    const uint32_t size
) {
    for (uint32_t idx_char = 0; idx_char < size; ++idx_char) {
        printf("%02d : '%c'\n", idx_char, buffer[idx_char]);
    }
}

int real_input_zero_length() {
    const uint32_t size_input  = 10;
    const uint32_t size_output = 1;
    const uint32_t size_malloc = size_input + 1;

    const char* s_format = "%s";
    const char* s_input  = "0123456789";
    char*       b_output = malloc(size_malloc);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_input);

    const uint32_t result = wrapper(b_output, size_output, s_format, s_input);

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != 1) {
        printf("Error in real_input_zero_length(): resulting length is %d, should be 1\n", result);
        free(b_output);
        return 1;
    }

    if (b_output[0] != '\0') {
        printf("Error in real_input_zero_length(): Character at index 0 should be null terminator\n");
        free(b_output);
        return 1;
    }

    for (uint32_t idx_char = 1; idx_char < size_malloc; ++idx_char) {
        if (b_output[idx_char] != 'A') {
            printf("Error in real_input_zero_length(): Character at index %d should be 'A' but is '%c'\n", idx_char, b_output[idx_char]);
            free(b_output);
            return 1;
        }
    }

    free(b_output);

    return 0;
}

int empty_input_zero_length() {
    const uint32_t size_input  = 0;
    const uint32_t size_output = 1;
    const uint32_t size_malloc = size_input + 1;

    const char* s_format = "%s";
    const char* s_input  = "";
    char*       b_output = malloc(size_malloc);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_malloc);

    const uint32_t result = wrapper(b_output, size_output, s_format, s_input);

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != 1) {
        printf("Error in empty_input_zero_length(): resulting length is %d, should be 1\n", result);
        free(b_output);
        return 1;
    }

    if (b_output[0] != '\0') {
        printf("Error in empty_input_zero_length(): Character at index 0 should be null terminator\n");
        free(b_output);
        return 1;
    }

    free(b_output);

    return 0;
}

int empty_input_real_length() {
    const uint32_t size_input  = 0;
    const uint32_t size_output = 10;
    const uint32_t size_malloc = size_output;

    const char* s_format = "%s";
    const char* s_input  = "";
    char*       b_output = malloc(size_malloc);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_malloc);

    const uint32_t result = wrapper(b_output, size_output, s_format, s_input);

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != 1) {
        printf("Error in empty_input_real_length(): resulting length is %d, should be %d\n", result, size_input + 1);
        free(b_output);
        return 1;
    }

    if (b_output[size_input] != '\0') {
        printf("Error in empty_input_real_length(): Character at index %d should be null terminator\n", size_input);
        free(b_output);
        return 1;
    }

    for (uint32_t idx_char = 1; idx_char < size_malloc; ++idx_char) {
        if (b_output[idx_char] != 'A') {
            printf("Error in empty_input_real_length(): Character at index %d should be 'A' but is '%c'\n", idx_char, b_output[idx_char]);
            free(b_output);
            return 1;
        }
    }

    free(b_output);

    return 0;
}

int shorter_input() {
    const uint32_t size_input  = 5;
    const uint32_t size_output = 10;
    const uint32_t size_malloc = size_input + 1;

    const char* s_format = "%s";
    const char* s_input  = "01234";
    char*       b_output = malloc(size_input + 1);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_malloc);

    const uint32_t result = wrapper(b_output, size_output, s_format, s_input);

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != size_input + 1) {
        printf("Error in shorter_input(): resulting length is %d, should be %d\n", result, size_input + 1);
        free(b_output);
        return 1;
    }

    if (b_output[size_input] != '\0') {
        printf("Error in shorter_input(): Character at index %d should be null terminator\n", size_input);
        free(b_output);
        return 1;
    }

    const char* s_expected = "01234";
    for (uint32_t idx_char = 0; idx_char < result; ++idx_char) {
        if (b_output[idx_char] != s_expected[idx_char]) {
            printf(
                "Error in shorter_input(): Character at index %d is '%c', should be '%c'\n",
                idx_char,
                b_output[idx_char],
                s_expected[idx_char]
            );
            free(b_output);
            return 1;
        }
    }

    for (uint32_t idx_char = result; idx_char < size_malloc; ++idx_char) {
        if (b_output[idx_char] != 'A') {
            printf("Error in shorter_input(): Character at index %d should be 'A' but is '%c'\n", idx_char, b_output[idx_char]);
            free(b_output);
            return 1;
        }
    }

    free(b_output);

    return 0;
}

int shorter_input_off_by_one() {
    const uint32_t size_input  = 9;
    const uint32_t size_output = 10;
    const uint32_t size_malloc = size_output;

    const char* s_format = "%s";
    const char* s_input  = "012345678";
    char*       b_output = malloc(size_malloc);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_malloc);

    const uint32_t result = wrapper(b_output, size_output, s_format, s_input);

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != size_output) {
        printf("Error in shorter_input_off_by_one(): resulting length is %d, should be %d\n", result, size_input + 1);
        free(b_output);
        return 1;
    }

    if (b_output[size_input] != '\0') {
        printf("Error in shorter_input_off_by_one(): Character at index %d should be null terminator\n", size_input);
        free(b_output);
        return 1;
    }

    const char* s_expected = "012345678";
    for (uint32_t idx_char = 0; idx_char < size_input; ++idx_char) {
        if (b_output[idx_char] != s_expected[idx_char]) {
            printf(
                "Error in shorter_input_off_by_one(): Character at index %d is '%c', should be '%c'\n",
                idx_char,
                b_output[idx_char],
                s_expected[idx_char]
            );
            free(b_output);
            return 1;
        }
    }

    free(b_output);

    return 0;
}

int exact_length_input() {
    const uint32_t size_input  = 10;
    const uint32_t size_output = size_input;
    const uint32_t size_malloc = size_output;

    const char* s_format = "%s";
    const char* s_input  = "0123456789";
    char*       b_output = malloc(size_malloc);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_malloc);

    const uint32_t result = wrapper(b_output, size_output, s_format, s_input);

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != size_input) {
        printf("Error in exact_length_input(): resulting length is %d, should be %d\n", result, size_input + 1);
        free(b_output);
        return 1;
    }

    if (b_output[size_input - 1] != '\0') {
        printf("Error in exact_length_input(): Character at index %d should be null terminator\n", size_input);
        free(b_output);
        return 1;
    }

    const char* s_expected = "0123456789";
    for (uint32_t idx_char = 0; idx_char < result - 1; ++idx_char) {
        if (b_output[idx_char] != s_expected[idx_char]) {
            printf(
                "Error in exact_length_input(): Character at index %d is '%c', should be '%c'\n",
                idx_char,
                b_output[idx_char],
                s_expected[idx_char]
            );
            free(b_output);
            return 1;
        }
    }

    free(b_output);

    return 0;
}

int longer_input() {
    const uint32_t size_input  = 20;
    const uint32_t size_output = 10;
    const uint32_t size_malloc = size_input;

    const char* s_format = "%s";
    /*                 00 + 0123456789            */
    /*                 10 +           0123456789  */
    /*                 20 +                     0 */
    const char* s_input  = "0123456789ABCDEFGHIJK";
    char*       b_output = malloc(size_malloc);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_malloc);

    const uint32_t result = wrapper(b_output, size_output, s_format, s_input);

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != size_output) {
        printf("Error in longer_input(): resulting length is %d, should be %d\n", result, size_output);
        free(b_output);
        return 1;
    }

    if (b_output[size_output - 1] != '\0') {
        printf("Error in longer_input(): Character at index %d should be null terminator\n", size_output);
        free(b_output);
        return 1;
    }

    /* Check all formatted characters (up until the null terminator) */
    const char* s_expected = "012345678";
    for (uint32_t idx_char = 0; idx_char < result - 1; ++idx_char) {
        if (b_output[idx_char] != s_expected[idx_char]) {
            printf(
                "Error in longer_input(): Character at index %d is '%c', should be '%c'\n",
                idx_char,
                b_output[idx_char],
                s_expected[idx_char]
            );
            free(b_output);
            return 1;
        }
    }

    /* Ensure the non-populated characters are as we set them (all As) */
    for (uint32_t idx_char = result; idx_char < size_malloc; ++idx_char) {
        if (b_output[idx_char] != 'A') {
            printf(
                "Error in longer_input(): Character at index %d is '%c', should be 'A'\n",
                idx_char,
                b_output[idx_char]
            );
            free(b_output);
            return 1;
        }
    }

    free(b_output);

    return 0;
}

int interesting_format() {
    const uint32_t size_input  = 15;
    const uint32_t size_output = 10;
    const uint32_t size_malloc = size_input;

    const char* s_format = "%02d %07X";
    char*       b_output = malloc(size_malloc);
    memset(b_output, 'A', size_malloc);

    printf("Freshly memset-ed buffer:\n");
    print_buffer(b_output, size_malloc);

    const uint32_t result = wrapper(b_output, size_output, s_format, 7, 912081); /* 912081 is 0xDEAD1 */

    printf("Freshly formatted buffer:\n");
    print_buffer(b_output, size_malloc);

    if (result != size_output) {
        printf("Error in interesting_format(): resulting length is %d, should be %d\n", result, size_output + 1);
        free(b_output);
        return 1;
    }

    if (b_output[size_output - 1] != '\0') {
        printf("Error in interesting_format(): Character at index %d should be null terminator\n", size_output - 1);
        free(b_output);
        return 1;
    }

    /* Ensure the formatted bit is correct */
    const char* s_expected = "07 00DEAD1";
    for (uint32_t idx_char = 0; idx_char < result - 1; ++idx_char) {
        if (b_output[idx_char] != s_expected[idx_char]) {
            printf(
                "Error in interesting_format(): Character at index %d is '%c', should be '%c'\n",
                idx_char,
                b_output[idx_char],
                s_expected[idx_char]
            );
            free(b_output);
            return 1;
        }
    }

    /* Ensure the non-populated characters are as we set them (all As) */
    for (uint32_t idx_char = result; idx_char < size_malloc; ++idx_char) {
        if (b_output[idx_char] != 'A') {
            printf(
                "Error in interesting_format(): Character at index %d is '%c', should be 'A'\n",
                idx_char,
                b_output[idx_char]
            );
            free(b_output);
            return 1;
        }
    }

    free(b_output);

    return 0;
}

int main(
    void
) {
    return 0
           || real_input_zero_length()
           || empty_input_zero_length()
           || empty_input_real_length()
           || shorter_input()
           || shorter_input_off_by_one()
           || exact_length_input()
           || longer_input()
           || interesting_format()
           || 0;
}
