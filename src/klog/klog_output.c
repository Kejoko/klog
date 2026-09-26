#include "./klog_output.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "klog/klog.h"

#include "./klog_format.h"
#include "./klog_platform.h"

void klog_output(
    const uint32_t         actual_message_length,
    const char* const      s_message_formatted,
    const KlogString       packed_prefix_console,
    const KlogString       packed_prefix_file,
    const uint8_t          level_requested,
    const uint8_t          level_max_console,
    const uint8_t          level_max_file,
    FILE*                  p_file,
    klog_platform_mutex_t* p_mutex_console,
    klog_platform_mutex_t* p_mutex_file
) {
    uint32_t i_starting_character = 0;
    while (i_starting_character <= actual_message_length) {
        const char* const p_newline         = strchr(s_message_formatted + i_starting_character, '\n');
        const uint32_t    submessage_length = p_newline
            ? p_newline - (s_message_formatted + i_starting_character)
            : actual_message_length;

        const KlogString packed_message = { submessage_length, s_message_formatted + i_starting_character };
        if (level_requested <= level_max_console) {
            klog_platform_mutex_lock(p_mutex_console);
            klog_output_console(&packed_prefix_console, &packed_message);
            klog_platform_mutex_unlock(p_mutex_console);
        }
        if (p_file && (level_requested <= level_max_file)) {
            klog_platform_mutex_lock(p_mutex_file);
            klog_output_file(p_file, &packed_prefix_file, &packed_message);
            klog_platform_mutex_unlock(p_mutex_file);
        }

        i_starting_character = i_starting_character + submessage_length + 1;
    }
}

void klog_output_console(
    const KlogString* const p_prefix,
    const KlogString* const p_message
) {
    if (p_prefix) {
        printf("%.*s", p_prefix->length, p_prefix->s);
    }
    if (p_message) {
        printf("%.*s", p_message->length, p_message->s);
    }
    printf("\n");
}

void klog_output_file(
    FILE* const             p_file,
    const KlogString* const p_prefix,
    const KlogString* const p_message
) {
    if (p_prefix) {
        fprintf(p_file, "%.*s", p_prefix->length, p_prefix->s);
    }
    if (p_message) {
        fprintf(p_file, "%.*s", p_message->length, p_message->s);
    }
    fprintf(p_file, "\n");
}
