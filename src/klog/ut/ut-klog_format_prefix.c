/**
 * @brief Ensure that the message's prefix is being created correctly
 *      - Ensure the prefix buffer is initialized correctly (considering thread id, timestamp, logger name length, source location length)
 *      - Ensure that when we log, the prefix buffer is populated accordingly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_state.h"
#include "../klog_constants.h"
#include "../klog_format.h"
#include "klog/klog.h"

int length_0(void) {
    const char* s = "[hello] [fatal] ";
    /*  00+          123456789          */
    /*  10+                   0123456   */
    const uint32_t expected = strlen(s);

    const uint32_t actual = klog_format_prefix_length_get(false, false, 5, false, 0);

    if (actual != expected) {
        printf("Prefix length for \"%s\" should be %d, got %d\n", s, expected, actual);
        return 1;
    }

    return 0;
}

int length_color(void) {
    const char* s = "[hello12345] [\x1b[36mdebug\x1b[0m] ";
    /*  00+          123456789                              */
    /*  10+                   012345    6789                */
    /*  20+                                 01234    5678   */
    const uint32_t expected = strlen(s);

    const uint32_t actual = klog_format_prefix_length_get(false, false, 10, true, 0);

    if (actual != expected) {
        printf("Prefix length for \"%s\" should be %d, got %d\n", s, expected, actual);
        return 1;
    }

    return 0;
}

int length_source_location(void) {
    const char* s = "[hel] [\x1b[33mWARN \x1b[0m] [aaaaBBBBccccDDDD:9999] ";
    const uint32_t expected = strlen(s);

    const uint32_t actual = klog_format_prefix_length_get(false, false, 3, true, 16);

    if (actual != expected) {
        printf("Prefix length for \"%s\" should be %d, got %d\n", s, expected, actual);
        return 1;
    }

    return 0;
}

int length_timestamp(void) {
    const char* s = "ddd:hh:mm:ss:uuuuuu [123456789] [debug] [aaaabbbb:9999] ";
    const uint32_t expected = strlen(s);

    const uint32_t actual = klog_format_prefix_length_get(false, true, 9, false, 8);

    if (actual != expected) {
        printf("prefix length for \"%s\" should be %d, got %d\n", s, expected, actual);
        return 1;
    }

    return 0;
}

int length_thread_id(void) {
    const char* s = "1234567 ddd:hh:mm:ss:uuuuuu [123456789] [warn ] ";
    const uint32_t expected = strlen(s);

    const uint32_t actual = klog_format_prefix_length_get(true, true, 9, false, 0);

    if (actual != expected) {
        printf("prefix length for \"%s\" should be %d, got %d\n", s, expected, actual);
        return 1;
    }

    return 0;
}

int length_all(void) {
    const char* s = "1234567 ddd:hh:mm:ss:uuuuuu [logger] [\x1b[31mERROR\x1b[0m] [12345  :   1] ";
    const uint32_t expected = strlen(s);

    const uint32_t actual = klog_format_prefix_length_get(true, true, 6, true, 7);

    if (actual != expected) {
        printf("prefix length for \"%s\" should be %d, got %d\n", s, expected, actual);
        return 1;
    }

    return 0;
}

int none(void) {
    KlogString result = klog_format_message_prefix(NULL, NULL, NULL, NULL, NULL, NULL);

    if (result.length != 0) {
        printf("Resulting KlogString's length should be 0 when nothing is given to the prefix\n");
        return 1;
    }

    if (result.s != NULL) {
        printf("Resulting KlogString's string should be NULL when nothing is given to the prefix\n");
        return 1;
    }

    return 0;
}

int empty_strings(void) {
    const char* s_time = "mustard!";
    KlogString packed_time = {0, s_time};

    const char* s_name = "purple";
    KlogString packed_name = {0, s_name};

    const char* s_level = "herobrine is real";
    KlogString packed_level = {0, s_level};

    const char* s_source_location = "googoo";
    KlogString packed_source_location = {0, s_source_location};

    KlogString result = klog_format_message_prefix(NULL, NULL, &packed_time, &packed_name, &packed_level, &packed_source_location);

    if (result.length != 0) {
        printf("Resulting KlogString's length should be 0 when nothing is given to the prefix\n");
        return 1;
    }

    if (result.s != NULL) {
        printf("Resulting KlogString's string should be NULL when nothing is given to the prefix\n");
        return 1;
    }

    return 0;
}

int just_thread_id(void) {
    const uint32_t length_expected = 8;
    const uint32_t thread_id = 1234;

    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, &thread_id, NULL, NULL, NULL, NULL);

    if (result.length != length_expected) { printf("Resulting KlogString's length is %d but should be %d when only the thread id is provided\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL when the thread id is provided\n");
        return 1;
    }

    const char* s_expected = "0001234 ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int just_time(void) {
    const uint32_t length_provided = 10;
    const char* s_provided = "0123456789";
    KlogString packed_time = {length_provided, s_provided};
    
    const uint32_t length_expected = length_provided + 1;
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, NULL, &packed_time, NULL, NULL, NULL);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d due to only the time string being provided length\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL when the time is provided\n");
        return 1;
    }

    const char* s_expected = "0123456789 ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int just_name(void) {
    const uint32_t length_provided = 6;
    const char* s_provided = "LOGGER";
    KlogString packed_name = {length_provided, s_provided};

    const uint32_t length_expected = length_provided + 3;
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, NULL, NULL, &packed_name, NULL, NULL);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d due to only the logger name string being provided\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL when the logger name is provided\n");
        return 1;
    }

    const char* s_expected = "[LOGGER] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int just_level(void) {
    const uint32_t length_provided = 20;
    const char* s_provided = "AAAAbbbbCCCCdddd    ";
    KlogString packed_level = {length_provided, s_provided};

    const uint32_t length_expected = length_provided + 3;
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, NULL, NULL, NULL, &packed_level, NULL);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d due to only the level string being provided\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL when the level is provided\n");
        return 1;
    }

    const char* s_expected = "[AAAAbbbbCCCCdddd    ] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int just_source_location(void) {
    const uint32_t length_provided = 3;
    const char* s_provided = "X:2";
    KlogString packed_source_location = {length_provided, s_provided};
    
    const uint32_t length_expected = length_provided + 3;
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, NULL, NULL, NULL, NULL, &packed_source_location);


    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d due to only the source location being provided\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL when the source location is provided\n");
        return 1;
    }

    const char* s_expected = "[X:2] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int subset_1(void) {
    const uint32_t thread_id = 42;

    const uint32_t time_length_provided = 18;
    const char* s_time = "_0.9826sfHEHEWOW  ";
    KlogString packed_time = {time_length_provided, s_time};

    const uint32_t name_length_provided = 14;
    const char* s_name = " HOHO xD Rawr ";
    KlogString packed_name = {name_length_provided, s_name};

    const uint32_t length_expected = 8 + (time_length_provided + 1) + (name_length_provided + 3);
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, &thread_id, &packed_time, &packed_name, NULL, NULL);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL\n");
        return 1;
    }

    const char* s_expected = "0000042 _0.9826sfHEHEWOW   [ HOHO xD Rawr ] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int subset_2(void) {
    const uint32_t time_length_provided = 22;
    const char* s_time = "THE FIRST OF THE MONTH";
    KlogString packed_time = {time_length_provided, s_time};

    const uint32_t name_length_provided = 4;
    const char* s_name = "WAKE";
    KlogString packed_name = {name_length_provided, s_name};

    const uint32_t level_length_provided = 3;
    const char* s_level = "UP!";
    KlogString packed_level = {level_length_provided, s_level};

    const uint32_t length_expected = (time_length_provided + 1) + (name_length_provided + 3) + (level_length_provided + 3);
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, NULL, &packed_time, &packed_name, &packed_level, NULL);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL\n");
        return 1;
    }

    const char* s_expected = "THE FIRST OF THE MONTH [WAKE] [UP!] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int subset_3(void) {
    const uint32_t name_length_provided = 15;
    const char* s_name = "__111__222__333";
    KlogString packed_name = {name_length_provided, s_name};

    const uint32_t level_length_provided = 4;
    const char* s_level = ".!/$";
    KlogString packed_level = {level_length_provided, s_level};

    const uint32_t source_location_length_provided = 10;
    const char* s_source_location = "  hehehe  ";
    KlogString packed_source_location = {source_location_length_provided, s_source_location};

    const uint32_t length_expected = (name_length_provided + 3) + (level_length_provided + 3) + (source_location_length_provided + 3);
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, NULL, NULL, &packed_name, &packed_level, &packed_source_location);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL\n");
        return 1;
    }

    const char* s_expected = "[__111__222__333] [.!/$] [  hehehe  ] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int everything(void) {
    const uint32_t thread_id = 123456;

    const uint32_t time_length_provided = 8;
    const char* s_time = "mustard!";
    KlogString packed_time = {time_length_provided, s_time};

    const uint32_t name_length_provided = 6;
    const char* s_name = "purple";
    KlogString packed_name = {name_length_provided, s_name};

    const uint32_t level_length_provided = 17;
    const char* s_level = "herobrine is real";
    KlogString packed_level = {level_length_provided, s_level};

    const uint32_t source_location_length_provided = 6;
    const char* s_source_location = "googoo";
    KlogString packed_source_location = {source_location_length_provided, s_source_location};

    const uint32_t length_expected = 8 + (time_length_provided + 1) + (name_length_provided + 3) + (level_length_provided + 3) + (source_location_length_provided + 3);
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, &thread_id, &packed_time, &packed_name, &packed_level, &packed_source_location);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL\n");
        return 1;
    }

    const char* s_expected = "0123456 mustard! [purple] [herobrine is real] [googoo] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int shortened_strings(void) {
    const uint32_t time_length_provided = 3;
    const char* s_time = "mustard!";
    KlogString packed_time = {time_length_provided, s_time};

    const uint32_t name_length_provided = 4;
    const char* s_name = "purple";
    KlogString packed_name = {name_length_provided, s_name};

    const uint32_t level_length_provided = 10;
    const char* s_level = "herobrine is real";
    KlogString packed_level = {level_length_provided, s_level};

    const uint32_t source_location_length_provided = 4;
    const char* s_source_location = "googoo";
    KlogString packed_source_location = {source_location_length_provided, s_source_location};

    const uint32_t length_expected = (time_length_provided + 1) + (name_length_provided + 3) + (level_length_provided + 3) + (source_location_length_provided + 3);
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, NULL, &packed_time, &packed_name, &packed_level, &packed_source_location);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL\n");
        return 1;
    }

    const char* s_expected = "mus [purp] [herobrine ] [goog] ";
    const int comparison = strcmp(result.s, s_expected);
    if (comparison != 0) {
        printf("strcmp result (%d) shows that resulting string was \"%.*s\" instead of \"%.*s\"\n", comparison, length_expected, result.s, length_expected, s_expected);
        return 1;
    }

    free((char*)result.s);

    return 0;
}

int nominal_parameters(void) {
    const uint32_t thread_id = 0xBEEF;

    char* b_time = malloc(G_klog_time_string_length + 1);
    const KlogString packed_time = klog_format_time(b_time);

    const char* const s_logger_name = "boo_yah "; /* Because we are not using klog_logger_create, this name will not be sanitized */
    const KlogString packed_name = {8, s_logger_name};
    
    const char* const s_level = "FATALERRORWARN info debugtrace";
    const KlogString packed_level = {5, s_level};

    const char* s_filename = "WEEWEE YA!";
    const uint32_t line_number = 0x4269; /* greater than 9999 */
    char* b_source_location = malloc(21);
    const KlogString packed_source_location = klog_format_source_location(b_source_location, 15, s_filename, line_number);

    const uint32_t length_expected = 8 + (packed_time.length + 1) + (packed_name.length + 3) + (packed_level.length + 3) + (packed_source_location.length + 3);
    char* s = malloc(length_expected + 1);
    KlogString result = klog_format_message_prefix(s, &thread_id, &packed_time, &packed_name, &packed_level, &packed_source_location);

    if (result.length != length_expected) {
        printf("Resulting KlogString's length is %d but should be %d\n", result.length, length_expected);
        return 1;
    }

    if (result.s == NULL) {
        printf("Resulting KlogString's string should not be NULL\n");
        return 1;
    }

    const char* s_expected = "0048879 ddd:hh:mm:ss:uuuuuu [boo_yah ] [FATAL] [WEEWEE YA!     :9999] ";
    /*                        |       |                   |          |       |                       */
    /* length 00+             123456789                   |          |       |                       */
    /*        10+                      0123456789         |          |       |                       */
    /*        20+                                0123456789          |       |                       */
    /*        30+                                          0123456789|       |                       */
    /*        40+                                                    0123456789                     */
    /*        50+                                                              0123456789           */
    /*        60+                                                                        0123456789 */

    const int comparison_thread_id = strncmp(result.s, s_expected, 8);
    if (comparison_thread_id != 0) {
        printf("Thread id strncmp result (%d) shows that resulting string contains \"%.*s\" instead of \"%.*s\"\n", comparison_thread_id, 8, result.s, 8, s_expected);
        return 1;
    }

    const int comparison_remainder = strcmp(result.s + 29, s_expected + 29);
    if (comparison_remainder != 0) {
        printf("Remainder strcmp result (%d) shows that resulting string contains \"%s\" instead of \"%s\"\n", comparison_remainder, result.s + 29, s_expected + 29);
        return 1;
    }

    free((char*)packed_time.s);
    free((char*)packed_source_location.s);
    free((char*)result.s);

    return 0;
}

int noop(void) {
    return 0;
}

int main(void) {
    return
        length_0() ||
        length_color() ||
        length_source_location() ||
        length_timestamp() ||
        length_thread_id() ||
        length_all() ||
        none() ||
        empty_strings() ||
        just_thread_id() ||
        just_time() ||
        just_name() ||
        just_level() ||
        just_source_location() ||
        subset_1() ||
        subset_2() ||
        subset_3() ||
        everything() ||
        shortened_strings() ||
        nominal_parameters() ||
        noop()
    ;
}
