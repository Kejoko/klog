#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const uint32_t number;
    const char** const strings;
    const uint32_t* array;
} my_t;

my_t create_my_t(const char* fmt, const uint32_t number) {
    const char** const s = malloc(number * sizeof(char*));
    uint32_t* const p = malloc(number * sizeof(uint32_t));

    for (uint32_t i = 0; i < number; ++i) {
        s[i] = fmt + i;
        p[i] = i * 2;
    }

    const my_t value = {number, s, p};
    return value;
}

void do_something(my_t* p_value) {
    uint32_t result = 0;
    for (uint32_t i = 0; i < p_value->number; ++i) {
        printf("%s\n", p_value->strings[i]);
        result = result + p_value->array[i];
    }
    printf("Result: %d\n", result);
}

void free_my_t(my_t* p_value) {
    free(p_value->strings);
    free((uint32_t*)p_value->array);
}

int main(void) {
    const char* boing = "Hello world!";
    my_t value = create_my_t(boing, 10);
    do_something(&value);
    free_my_t(&value);
    return 0;
}
