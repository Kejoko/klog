#include "./klog_output.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void klog_output_stdout(const uint32_t* const p_thread_id, const KlogString* const p_time, const KlogString* const p_name, const KlogString* const p_level, const KlogString* const p_source_location, const KlogString* const p_message) {
    if (p_thread_id) {
        printf("%.7d ", *p_thread_id);
    }
    if (p_time) {
        printf("%.*s ", p_time->length, p_time->s);
    }
    if (p_name) {
        printf("[%.*s] ", p_name->length, p_name->s);
    }
    if (p_level) {
        printf("[%.*s] ", p_level->length, p_level->s);
    }
    if (p_source_location) {
        printf("[%.*s] ", p_source_location->length, p_source_location->s);
    }
    if (p_message) {
        printf("%.*s", p_message->length, p_message->s);
    }
    printf("\n");
}

void klog_output_file(FILE* const p_file, const uint32_t* const p_thread_id, const KlogString* const p_time, const KlogString* const p_name, const KlogString* const p_level, const KlogString* const p_source_location, const KlogString* const p_message) {
    if (p_thread_id) {
        fprintf(p_file, "%.7d ", *p_thread_id);
    }
    if (p_time) {
        fprintf(p_file, "%.*s ", p_time->length, p_time->s);
    }
    if (p_name) {
        fprintf(p_file, "[%.*s] ", p_name->length, p_name->s);
    }
    if (p_level) {
        fprintf(p_file, "[%.*s] ", p_level->length, p_level->s);
    }
    if (p_source_location) {
        fprintf(p_file, "[%.*s] ", p_source_location->length, p_source_location->s);
    }
    if (p_message) {
        fprintf(p_file, "%.*s", p_message->length, p_message->s);
    }
    fprintf(p_file, "\n");
}

