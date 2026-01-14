#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_format.h"

#define DO_BASE_CASE

#define DO_SIMPLE_NEWLINES
/* #define DO_COMPLEX_NEWLINES */
 
/* #define DO_SIMPLE_ARGS */
/* #define DO_COMPLEX_ARGS */
 
/* #define DO_SIMPLE_COMBINED */
/* #define DO_COMPLEX_COMBINED */

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Test utility ----------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

int do_comparison(
    const char* test_name,
    const klog_format_split_t* const p_result,
    const uint32_t expected_number_format_strings,
    const char** expected_format_strings,
    const uint32_t* expected_format_string_lengths,
    const uint32_t* expected_va_arg_start_indices,
    const uint32_t* expected_va_arg_amounts
) {
    if (p_result->number_format_strings != expected_number_format_strings) {
        printf("[%s] number format strings (%d) != expected (%d)\n", test_name, p_result->number_format_strings, expected_number_format_strings);
        return 1;
    }

    for (uint32_t i_format_string = 0; i_format_string < expected_number_format_strings; ++i_format_string) {
        // Ensure format string is valid
        if (p_result->format_strings[i_format_string] == NULL) {
            printf("[%s] format string %d is invalid\n", test_name, i_format_string);
            return 1;
        }
        
        // Ensure result char* == expected char*
        if (p_result->format_strings[i_format_string] != expected_format_strings[i_format_string]) {
            printf("[%s] format string %d address (%p) != expected (%p)\n", test_name, i_format_string, (void*)p_result->format_strings[i_format_string], (void*)expected_format_strings[i_format_string]);
            return 1;
        }

        // Ensure result length == expected length
        if (p_result->format_string_lengths[i_format_string] != expected_format_string_lengths[i_format_string]) {
            printf("[%s] format string %d length (%d) != expected (%d)\n", test_name, i_format_string, p_result->format_string_lengths[i_format_string], expected_format_string_lengths[i_format_string]);
            return 1;
        }

        // Ensure string equality between result and expected
        // We might need to consider strcmp > 1 because we won't have null terminated characters
        if (strcmp(p_result->format_strings[i_format_string], expected_format_strings[i_format_string]) < 0) {
            printf("[%s] format string %d (\"%.*s\") != expected (\"%.*s\")\n", test_name, i_format_string, expected_format_string_lengths[i_format_string], p_result->format_strings[i_format_string], expected_format_string_lengths[i_format_string], expected_format_strings[i_format_string]);
            return 1;
        }
        
        // Ensure result va arg start index == expected va arg start index
        if (p_result->va_arg_start_indices[i_format_string] != expected_va_arg_start_indices[i_format_string]) {
            printf("[%s] format string %d va arg start index (%d) != expected (%d)\n", test_name, i_format_string, p_result->va_arg_start_indices[i_format_string], expected_va_arg_start_indices[i_format_string]);
            return 1;
        }
        
        // Ensure result va arg amount == expected va arg amount
        if (p_result->va_arg_amounts[i_format_string] != expected_va_arg_amounts[i_format_string]) {
            printf("[%s] format string %d va arg amount (%d) != expected (%d)\n", test_name, i_format_string, p_result->va_arg_amounts[i_format_string], expected_va_arg_amounts[i_format_string]);
            return 1;
        }
    }
    return 0;
}

void free_things(
    const klog_format_split_t* p_result,
    const char** expected_format_strings,
    uint32_t* expected_format_string_lengths,
    uint32_t* expected_va_arg_start_indices,
    uint32_t* expected_va_arg_amounts
) {
    free(p_result->format_strings);
    free(p_result->format_string_lengths);
    free(p_result->va_arg_start_indices);
    free(p_result->va_arg_amounts);

    free(expected_format_strings);
    free(expected_format_string_lengths);
    free(expected_va_arg_start_indices);
    free(expected_va_arg_amounts);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Base case -------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

int base_case(void) {
#ifdef DO_BASE_CASE
    // Create expected

    const char* input = "Base case";

    uint32_t expected_number_format_strings = 1;

    const char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 9;
    
    uint32_t* expected_va_arg_start_indices = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    
    uint32_t* expected_va_arg_amounts = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;

    // Get the result value

    const klog_format_split_t result = klog_format_split_strings(input);

    // Check equality

    const int success = do_comparison(
        "base_case",
        &result,
        expected_number_format_strings,
        expected_format_strings,
        expected_format_string_lengths,
        expected_va_arg_start_indices,
        expected_va_arg_amounts
    );

    // Clean up and return

    free_things(&result, expected_format_strings, expected_format_string_lengths, expected_va_arg_start_indices, expected_va_arg_amounts);
    
    return success;
#else
    return 0;
#endif
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Newlines --------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

int single_newline(void) {
#ifdef DO_SIMPLE_NEWLINES
    // Create expected

    const char* input = "two\nlines";

    uint32_t expected_number_format_strings = 2;

    const char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 4;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 3;
    expected_format_string_lengths[1] = 5;
    
    uint32_t* expected_va_arg_start_indices = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    
    uint32_t* expected_va_arg_amounts = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;

    // Get the result value

    const klog_format_split_t result = klog_format_split_strings(input);

    // Check equality

    const int success = do_comparison(
        "single_newline",
        &result,
        expected_number_format_strings,
        expected_format_strings,
        expected_format_string_lengths,
        expected_va_arg_start_indices,
        expected_va_arg_amounts
    );

    // Clean up and return

    free_things(&result, expected_format_strings, expected_format_string_lengths, expected_va_arg_start_indices, expected_va_arg_amounts);
    
    return success;
#else
    return 0;
#endif
}

int four_lines(void) {
#ifdef DO_SIMPLE_NEWLINES
    // Create expected

    /* "1" */
    /* "two" */
    /* " 33333 " */
    /* "four" */
    const char* input = "1\ntwo\n 33333 \nfour";

    uint32_t expected_number_format_strings = 4;

    const char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 2;
    expected_format_strings[2] = input + 8;
    expected_format_strings[3] = input + 16;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 1;
    expected_format_string_lengths[1] = 3;
    expected_format_string_lengths[2] = 7;
    expected_format_string_lengths[3] = 4;
    
    uint32_t* expected_va_arg_start_indices = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;
    expected_va_arg_start_indices[3] = 0;
    
    uint32_t* expected_va_arg_amounts = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;
    expected_va_arg_start_indices[3] = 0;

    // Get the result value

    const klog_format_split_t result = klog_format_split_strings(input);

    // Check equality

    const int success = do_comparison(
        "four_lines",
        &result,
        expected_number_format_strings,
        expected_format_strings,
        expected_format_string_lengths,
        expected_va_arg_start_indices,
        expected_va_arg_amounts
    );

    // Clean up and return

    free_things(&result, expected_format_strings, expected_format_string_lengths, expected_va_arg_start_indices, expected_va_arg_amounts);
    
    return success;
#else
    return 0;
#endif
}

int two_consecutive_newlines(void) {
#ifdef DO_SIMPLE_NEWLINES
    // Create expected

    /* "  1" */
    /* "" */
    /* "two  " */
    const char* input = "  1\n\ntwo  ";
    /* start indices     0123 4 56789 */
    /* line count        1    2 3*/

    uint32_t expected_number_format_strings = 3;

    const char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 4;
    expected_format_strings[2] = input + 5;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 3;
    expected_format_string_lengths[1] = 0;
    expected_format_string_lengths[2] = 5;
    
    uint32_t* expected_va_arg_start_indices = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;
    
    uint32_t* expected_va_arg_amounts = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;

    // Get the result value

    const klog_format_split_t result = klog_format_split_strings(input);

    // Check equality

    const int success = do_comparison(
        "two_consecutive_newlines",
        &result,
        expected_number_format_strings,
        expected_format_strings,
        expected_format_string_lengths,
        expected_va_arg_start_indices,
        expected_va_arg_amounts
    );

    // Clean up and return

    free_things(&result, expected_format_strings, expected_format_string_lengths, expected_va_arg_start_indices, expected_va_arg_amounts);
    
    return success;
#else
    return 0;
#endif

}

int five_consecutive_newlines(void) {
#ifdef DO_COMPLEX_NEWLINES
    // Create expected

    const char* input = "1\n\n\n\n\nanother";
    /* start indices     01 2 3 4 5 6 */
    /* line count        1  2 3 4 5 6*/

    uint32_t expected_number_format_strings = 6;

    const char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 2;
    expected_format_strings[2] = input + 3;
    expected_format_strings[3] = input + 4;
    expected_format_strings[4] = input + 5;
    expected_format_strings[5] = input + 6;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 1;
    expected_format_string_lengths[1] = 0;
    expected_format_string_lengths[2] = 0;
    expected_format_string_lengths[3] = 0;
    expected_format_string_lengths[4] = 0;
    expected_format_string_lengths[5] = 7;
    
    uint32_t* expected_va_arg_start_indices = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;
    expected_va_arg_start_indices[3] = 0;
    expected_va_arg_start_indices[4] = 0;
    expected_va_arg_start_indices[5] = 0;
    
    uint32_t* expected_va_arg_amounts = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;
    expected_va_arg_start_indices[3] = 0;
    expected_va_arg_start_indices[4] = 0;
    expected_va_arg_start_indices[5] = 0;

    // Get the result value

    const klog_format_split_t result = klog_format_split_strings(input);

    // Check equality

    const int success = do_comparison(
        "five_consecutive_newlines",
        &result,
        expected_number_format_strings,
        expected_format_strings,
        expected_format_string_lengths,
        expected_va_arg_start_indices,
        expected_va_arg_amounts
    );

    // Clean up and return

    free_things(&result, expected_format_strings, expected_format_string_lengths, expected_va_arg_start_indices, expected_va_arg_amounts);
    
    return success;
#else
    return 0;
#endif


}

int many_newlines(void) {
#ifdef DO_COMPLEX_NEWLINES
    // Create expected

    const char* input = "one line\nsecond   \n third \nfourth\nfifth\n\nseventh\n\n 9th";
    /*                   01234567  012345678  0123456  012345  01234    0123456    0123 */

    uint32_t expected_number_format_strings = 9;

    const char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 9;
    expected_format_strings[2] = input + 20;
    expected_format_strings[3] = input + 28;
    expected_format_strings[4] = input + 35;
    expected_format_strings[5] = input + 41;
    expected_format_strings[6] = input + 43;
    expected_format_strings[7] = input + 51;
    expected_format_strings[8] = input + 52;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 8;
    expected_format_string_lengths[1] = 9;
    expected_format_string_lengths[2] = 7;
    expected_format_string_lengths[3] = 6;
    expected_format_string_lengths[4] = 5;
    expected_format_string_lengths[5] = 0;
    expected_format_string_lengths[6] = 7;
    expected_format_string_lengths[7] = 0;
    expected_format_string_lengths[8] = 4;
    
    uint32_t* expected_va_arg_start_indices = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;
    expected_va_arg_start_indices[3] = 0;
    expected_va_arg_start_indices[4] = 0;
    expected_va_arg_start_indices[5] = 0;
    expected_va_arg_start_indices[6] = 0;
    expected_va_arg_start_indices[7] = 0;
    expected_va_arg_start_indices[8] = 0;
    
    uint32_t* expected_va_arg_amounts = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_va_arg_start_indices[0] = 0;
    expected_va_arg_start_indices[1] = 0;
    expected_va_arg_start_indices[2] = 0;
    expected_va_arg_start_indices[3] = 0;
    expected_va_arg_start_indices[4] = 0;
    expected_va_arg_start_indices[5] = 0;
    expected_va_arg_start_indices[6] = 0;
    expected_va_arg_start_indices[7] = 0;
    expected_va_arg_start_indices[8] = 0;

    // Get the result value

    const klog_format_split_t result = klog_format_split_strings(input);

    // Check equality

    const int success = do_comparison(
        "single_newline",
        &result,
        expected_number_format_strings,
        expected_format_strings,
        expected_format_string_lengths,
        expected_va_arg_start_indices,
        expected_va_arg_amounts
    );

    // Clean up and return

    free_things(&result, expected_format_strings, expected_format_string_lengths, expected_va_arg_start_indices, expected_va_arg_amounts);
    
    return success;
#else
    return 0;
#endif
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Args ------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Combined --------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Main ------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

int noop(void) { return 0; }

int main(void) {
    const int result =
        base_case() ||
        single_newline() ||
        noop()
    ;

    return result;
}
