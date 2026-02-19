#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t num_elements = 0;

uint32_t* a_elements = NULL;

#define add_element(value) { \
    const uint32_t num_elements_prev = num_elements; \
    num_elements = num_elements + 1; \
    if (!a_elements) { \
        a_elements = malloc(num_elements * sizeof(uint32_t)); \
    } else { \
        uint32_t* a_elements_prev = a_elements; \
        a_elements = malloc(num_elements * sizeof(uint32_t)); \
        memcpy(a_elements, a_elements_prev, num_elements_prev * sizeof(uint32_t)); \
        a_elements[num_elements - 1] = value; \
        free(a_elements_prev); \
    } \
} while(0)

int main(void) {
    for (uint32_t i = 0; i < 30; ++i) {
        add_element(i * i * 42);
    }
    
    printf("num elements: %d\n", num_elements);

    for (uint32_t i = 0; i < num_elements; ++i) {
        printf("%d\n", a_elements[i]);
    }

    free(a_elements);

    return 0;
}
