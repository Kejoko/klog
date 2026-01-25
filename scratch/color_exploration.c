#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char* red = "\x1b[31mHELLO";
    int red_size = strlen(red);
    printf("Size %d : %s\n", red_size, red);

    char* reset = "\x1b[0m";
    int reset_size = strlen(reset);
    printf("Size %d : %s\n", reset_size, reset);
}
