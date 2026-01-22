#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * This is validating that you can malloc something and index into it normally as if it were an array,
 * regardless of the underlying data type
 *
 * I care about this for semantics and naming purposes, to ensure it makes sense to use the `a_`
 * prefix to denote that something is an array
 */

struct Thing {
    uint32_t a;
    double b;
    uint16_t c;
    bool d;
};

int main(void) {
    printf("Thing size: %ld\n", sizeof(struct Thing));

    const uint32_t num_things = 4;

    /* Malloc an "array" and populate it */
    const uint32_t thing_size = sizeof(struct Thing);
    const uint32_t total_things_size = num_things * thing_size;
    /* struct Thing* p_things = (struct Thing*) malloc(total_things_size); */
    struct Thing* p_things = malloc(total_things_size);
    printf("Things array:\n");
    printf("  start : %p\n", (void*)p_things);
    printf("  end   : %p\n", (void*)(p_things + total_things_size));
    printf("  size  : %d\n", total_things_size);
    printf("  length: %d\n", (total_things_size / thing_size));

    /* Populate it */
    {
        struct Thing* p_thing_0 = &p_things[0];
        p_thing_0->a = 420;
        p_thing_0->b = 999.666;
        p_thing_0->c = 69;
        p_thing_0->d = false;

        struct Thing* p_thing_1 = &p_things[1];
        p_thing_1->a = 111;
        p_thing_1->b = 111;
        p_thing_1->c = 111;
        p_thing_1->d = true;

        struct Thing thing_2 = {222, 222.222, 22, true};
        p_things[2] = thing_2;

        struct Thing thing_3 = {333, 0.333, 3, false};
        p_things[3] = thing_3;
    }

    /* Print everything */
    for (uint32_t i = 0; i < num_things; ++i) {
        struct Thing thing = p_things[i];
        printf("Thing %d\n", i);
        printf("  a: %d\n", thing.a);
        printf("  b: %f\n", thing.b);
        printf("  c: %d\n", thing.c);
        printf("  d: %d\n", thing.d);
    }

    /* Clean up */

    free(p_things);

    return 0;
}
