/**
 * @brief Ensure that the source location is formatted correctly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_format.h"

int perform_checks(KlogString* const p_result, const uint32_t length_provided, const char* s_filename_provided, const uint32_t line_provided) {
    const uint32_t length_expected = length_provided + 1 + 4; /* colon, 4 digit line number */

    if (p_result->length != length_expected) {
        printf("Resulting KlogString (\"%s\") length is actually %d instead of expected %d\n", p_result->s, p_result->length, length_expected);
        return 1;
    }
    
    if (p_result->s == NULL) {
        printf("Resulting KlogString has an invalid string (NULL) when we request a max filename length of %d\n", length_expected);
        return 1;
    }

    char* s_adjusted_filename = malloc(length_provided + 1);
    s_adjusted_filename[length_provided] = '\0';
    memset(s_adjusted_filename, ' ', length_provided);
    const uint32_t filename_provided_length = strlen(s_filename_provided);
    const uint32_t filename_character_copy_amount = filename_provided_length <= length_provided ? filename_provided_length : length_provided;
    memcpy(s_adjusted_filename, s_filename_provided, filename_character_copy_amount);
    const int filename_comparison_result = strncmp(s_adjusted_filename, p_result->s, filename_character_copy_amount);
    if (filename_comparison_result) {
        printf("strncmp result (%d) shows that filename portion was \"%.*s\" instead of \"%.*s\"\n", filename_comparison_result, length_provided, p_result->s, length_provided, s_adjusted_filename);
        free(s_adjusted_filename);
        return 1;
    }
    free(s_adjusted_filename);

    if (p_result->s[length_provided] != ':') {
        printf("Resulting KlogString does not contain a colon at character index %d\n", length_provided);
        return 1;
    }

    const char* s_line = &(p_result->s[length_provided + 1]);
    uint32_t result_line = 0;
    const int scanf_success = sscanf(s_line, "%u", &result_line);
    if (scanf_success != 1) {
        printf("Failed to invoke scanf on the end of the resulting string \"%s\"\n", s_line);
        return 1;
    }
    if ((result_line != line_provided) && (line_provided > 9999) && (result_line != 9999)) {
        printf("Resulting line is %d instead of truncated 9999, when provided line is %d\n", result_line, line_provided);
        return 1;
    }
    if ((result_line != line_provided) && (line_provided <= 9999)) {
        printf("Resulting line is %d instead of %d\n", result_line, line_provided);
        return 1;
    }

    const char final_character = p_result->s[length_expected];
    if (final_character != '\0') {
        printf("Resulting KlogString's final character (index %d, reported length + 1) should be null terminator, but instead it is '%c'\n", length_expected, final_character);
        return 1;
    }

    return 0;
}

int run_test(KlogString* const p_result, const uint32_t length_provided, const char* s_filename_provided, const uint32_t line_provided) {
    const int success = perform_checks(p_result, length_provided, s_filename_provided, line_provided);

    free((char*)p_result->s);

    return success;
}

int none(void) {
    const uint32_t length_expected = 0;
    KlogString result = klog_format_source_location(length_expected, "Hello", 123);
    
    if (result.length != length_expected) {
        printf("Resulting KlogString length is actually %d instead of expected %d\n", result.length, length_expected);
        return 1;
    }
    
    if (result.s != NULL) {
        printf("Resulting KlogString has a valid string instead of NULL when we request a max length of 0\n");
        return 1;
    }

    return 0;
}

int length_1(void) {
    const uint32_t length_provided = 1;
    const char* s_filename_provided = "Hello";
    const uint32_t line_provided = 123;
    
    KlogString result = klog_format_source_location(length_provided, s_filename_provided, line_provided);

    return run_test(&result, length_provided, s_filename_provided, line_provided);
}

int length_normal(void) {
    const uint32_t length_provided = 5;
    const char* s_filename_provided = "Hello";
    const uint32_t line_provided = 420;
    
    KlogString result = klog_format_source_location(length_provided, s_filename_provided, line_provided);

    return run_test(&result, length_provided, s_filename_provided, line_provided);
}

int empty_filename(void) {
    const uint32_t length_provided = 10;
    const char* s_filename_provided = "";
    const uint32_t line_provided = 6969;
    
    KlogString result = klog_format_source_location(length_provided, s_filename_provided, line_provided);

    return run_test(&result, length_provided, s_filename_provided, line_provided);
}

int padded_filename(void) {
    const uint32_t length_provided = 50;
    const char* s_filename_provided = " pad ";
    const uint32_t line_provided = 6;
    
    KlogString result = klog_format_source_location(length_provided, s_filename_provided, line_provided);

    return run_test(&result, length_provided, s_filename_provided, line_provided);
}

int padded_line_number(void) {
    const uint32_t length_provided = 3;
    const char* s_filename_provided = "yippee";
    const uint32_t line_provided = 0;
    
    KlogString result = klog_format_source_location(length_provided, s_filename_provided, line_provided);

    return run_test(&result, length_provided, s_filename_provided, line_provided);
}

int max_line_number(void) {
    const uint32_t length_provided = 3;
    const char* s_filename_provided = "yippee";
    const uint32_t line_provided = 9999;
    
    KlogString result = klog_format_source_location(length_provided, s_filename_provided, line_provided);

    return run_test(&result, length_provided, s_filename_provided, line_provided);
}

int truncated_line_number(void) {
    const uint32_t length_provided = 3;
    const char* s_filename_provided = "yippee";
    const uint32_t line_provided = 123456;
    
    KlogString result = klog_format_source_location(length_provided, s_filename_provided, line_provided);

    return run_test(&result, length_provided, s_filename_provided, line_provided);
}

int noop(void) {
    return 0;
}

int main(void) {
    return
        none() ||
        length_1() ||
        length_normal() ||
        empty_filename() ||
        padded_filename() ||
        padded_line_number() ||
        max_line_number() ||
        truncated_line_number() ||
        noop()
    ;
}
