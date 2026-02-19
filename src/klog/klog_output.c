#include "./klog_output.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void klog_output_console(const KlogString* const p_prefix, const KlogString* const p_message) {
    if (p_prefix) {
        printf("%.*s", p_prefix->length, p_prefix->s);
    }
    if (p_message) {
        printf("%.*s", p_message->length, p_message->s);
    }
    printf("\n");
}

void klog_output_file(FILE* const p_file, const KlogString* const p_prefix, const KlogString* const p_message) {
    if (p_prefix) {
        fprintf(p_file, "%.*s", p_prefix->length, p_prefix->s);
    }
    if (p_message) {
        fprintf(p_file, "%.*s", p_message->length, p_message->s);
    }
    fprintf(p_file, "\n");
}

