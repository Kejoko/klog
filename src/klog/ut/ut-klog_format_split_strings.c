#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_format.h"

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Test utility ----------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

int do_comparison(
    const char* test_name,
    const KlogFormatSplitInfo* const p_result,
    const uint32_t expected_number_format_strings,
    char** expected_format_strings,
    const uint32_t* expected_format_string_lengths
) {
    if (p_result->number_strings != expected_number_format_strings) {
        printf("[%s] number format strings (%d) != expected (%d)\n", test_name, p_result->number_strings, expected_number_format_strings);
        return 1;
    }

    for (uint32_t i_format_string = 0; i_format_string < expected_number_format_strings; ++i_format_string) {
        // Ensure format string is valid
        if (p_result->ls_strings[i_format_string] == NULL) {
            printf("[%s] format string %d is invalid\n", test_name, i_format_string);
            return 1;
        }
        
        // Ensure result char* == expected char*
        if (p_result->ls_strings[i_format_string] != expected_format_strings[i_format_string]) {
            printf("[%s] format string %d address (%p) != expected (%p)\n", test_name, i_format_string, (void*)p_result->ls_strings[i_format_string], (void*)expected_format_strings[i_format_string]);
            return 1;
        }

        // Ensure result length == expected length
        if (p_result->a_string_lengths[i_format_string] != expected_format_string_lengths[i_format_string]) {
            printf("[%s] format string %d length (%d) != expected (%d)\n", test_name, i_format_string, p_result->a_string_lengths[i_format_string], expected_format_string_lengths[i_format_string]);
            return 1;
        }

        // Ensure string equality between result and expected
        // We might need to consider strcmp > 1 because we won't have null terminated characters
        if (strcmp(p_result->ls_strings[i_format_string], expected_format_strings[i_format_string]) < 0) {
            printf("[%s] format string %d (\"%.*s\") != expected (\"%.*s\")\n", test_name, i_format_string, expected_format_string_lengths[i_format_string], p_result->ls_strings[i_format_string], expected_format_string_lengths[i_format_string], expected_format_strings[i_format_string]);
            return 1;
        }
    }
    return 0;
}

void free_things(
    KlogFormatSplitInfo* const p_result,
    char** format_strings,
    uint32_t* format_string_lengths
) {
    free(p_result->ls_strings);
    free((uint32_t*)p_result->a_string_lengths);

    free(format_strings);
    free(format_string_lengths);
}

int run_test(
    const char* test_name,
    const char* input,
    const uint32_t expected_number_format_strings,
    char** expected_format_strings,
    uint32_t* expected_format_string_lengths
) {
    printf("===================================================================\n");
    printf("%s\n", test_name);
    printf("===================================================================\n");

    // Get the result value

    KlogFormatSplitInfo result = klog_format_split_strings(input);
    /* @todo kjk 2026/01/13 Wrap this print statement behind the KLOG_DEBUG macro */
    printf("===================================================================\n");

    // Check equality

    const int success = do_comparison(
        test_name,
        &result,
        expected_number_format_strings,
        expected_format_strings,
        expected_format_string_lengths
    );
    printf("===================================================================\n");

    // Clean up and return

    free_things(&result, expected_format_strings, expected_format_string_lengths);
    printf("===================================================================\n");
    
    printf("\n\n\n");

    return success;
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Base case -------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

int base_case(void) {
    char* input = "Base case";

    uint32_t expected_number_format_strings = 1;

    char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 9;

    return run_test("base_case", input, expected_number_format_strings, expected_format_strings, expected_format_string_lengths);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */
/* Newlines --------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------ */

int single_newline(void) {
    char* input = "two\nlines";

    uint32_t expected_number_format_strings = 2;

    char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 4;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 3;
    expected_format_string_lengths[1] = 5;

    return run_test("single_newline", input, expected_number_format_strings, expected_format_strings, expected_format_string_lengths);
}

int four_lines(void) {
    /* "1"                                      */
    /* "two"                                    */
    /* " 33333 "                                */
    /* "four"                                   */

    /* Line Count:       1  2    3        4     */
    /* Lengths:                                 */
    /*                   1  123  1234567  1234  */
    char* input = "1\ntwo\n 33333 \nfour";
    /* Start indices:                           */
    /*   00 +            01 2345 6789           */
    /*   10 +                        0123 4567  */

    uint32_t expected_number_format_strings = 4;

    char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 2;
    expected_format_strings[2] = input + 6;
    expected_format_strings[3] = input + 14;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 1;
    expected_format_string_lengths[1] = 3;
    expected_format_string_lengths[2] = 7;
    expected_format_string_lengths[3] = 4;

    return run_test("four_lines", input, expected_number_format_strings, expected_format_strings, expected_format_string_lengths);
}

int two_consecutive_newlines(void) {
    /* "  1"                                    */
    /* ""                                       */
    /* "two  "                                  */

    /* Line Count:       1    2 3               */
    /* Lengths:                                 */
    /*                   123  0 12345           */
    char* input = "  1\n\ntwo  ";
    /* Start Indices:                           */
    /*   00 +            0123 4 56789           */

    uint32_t expected_number_format_strings = 3;

    char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 4;
    expected_format_strings[2] = input + 5;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 3;
    expected_format_string_lengths[1] = 0;
    expected_format_string_lengths[2] = 5;

    return run_test("two_consecutive_newlines", input, expected_number_format_strings, expected_format_strings, expected_format_string_lengths);
}

int five_consecutive_newlines(void) {
    /* "1"                                      */
    /* ""                                       */
    /* ""                                       */
    /* ""                                       */
    /* ""                                       */

    /* "another"                                */
    /* Line Count:       1  2 3 4 5 6           */
    /* Lengths:                                 */
    /*                   1  0 0 0 0 1234567     */
    char* input = "1\n\n\n\n\nanother";
    /* Start Indices:                           */
    /*   00 +            01 2 3 4 5 6789        */
    /*   10 +                           012     */

    uint32_t expected_number_format_strings = 6;

    char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
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

    return run_test("five_consecutive_newlines", input, expected_number_format_strings, expected_format_strings, expected_format_string_lengths);
}

int first_last(void) {
    /* ""                                       */
    /* "aa"                                     */
    /* "bbbb"                                   */
    /* ""                                       */

    /* Line Count:       1 2   3     4          */
    /* Lengths:                                 */
    /*                   0 12  1234  0          */
    char* input = "\naa\nbbbb\n";
    /* Start Indices:                           */
    /*   00 +            0 123 45678 9          */

    uint32_t expected_number_format_strings = 4;

    char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 1;
    expected_format_strings[2] = input + 4;
    expected_format_strings[3] = input + 9;
    
    uint32_t* expected_format_string_lengths = malloc(expected_number_format_strings * sizeof(uint32_t));
    expected_format_string_lengths[0] = 0;
    expected_format_string_lengths[1] = 2;
    expected_format_string_lengths[2] = 4;
    expected_format_string_lengths[3] = 0;

    return run_test("first_last", input, expected_number_format_strings, expected_format_strings, expected_format_string_lengths);
}

int many_newlines(void) {
    /* "one line"                               */
    /* "second   "                              */
    /* " third "                                */
    /* "fourth"                                 */
    /* "fifth"                                  */
    /* ""                                       */
    /* "seventh"                                */
    /* ""                                       */
    /* " 9th"                                   */

    /* Line Count:       1         2          3        4       5      6 7        8 9        */
    /* Lengths:                                                                             */
    /*   00 +            12345678  123456789  1234567  123456  12345  0 1234567  0 1234     */
    char* input = "one line\nsecond   \n third \nfourth\nfifth\n\nseventh\n\n 9th";
    /* Start Indices:    |         |          |        |       |      | |        | |        */
    /*   00 +            012345678 9          |        |       |      | |        | |        */
    /*   10 +                       012345678 9        |       |      | |        | |        */
    /*   20 +                                  0123456 789     |      | |        | |        */
    /*   30 +                                             0123 456789 | |        | |        */
    /*   40 +                                                         0 12345678 9 |        */
    /*   50 +                                                                      0123     */

    uint32_t expected_number_format_strings = 9;

    char** expected_format_strings = malloc(expected_number_format_strings * sizeof(char*));
    expected_format_strings[0] = input;
    expected_format_strings[1] = input + 9;
    expected_format_strings[2] = input + 19;
    expected_format_strings[3] = input + 27;
    expected_format_strings[4] = input + 34;
    expected_format_strings[5] = input + 40;
    expected_format_strings[6] = input + 41;
    expected_format_strings[7] = input + 49;
    expected_format_strings[8] = input + 50;
    
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

    return run_test("many_newlines", input, expected_number_format_strings, expected_format_strings, expected_format_string_lengths);
}

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
        four_lines() ||
        two_consecutive_newlines() ||
        first_last() ||
        many_newlines() ||
        noop()
    ;

    return result;
}
