#include <stdarg.h>
#include <stdio.h>

void sliceprintf(char* fmt, ...) {
    va_list p_args;

    va_start(p_args, fmt);

    int arg_1 = va_arg(p_args, int);
    int arg_2 = va_arg(p_args, int);
    int arg_3 = va_arg(p_args, int);

    printf("%d %d %d\n", arg_1, arg_2, arg_3);

    va_end(p_args);
}

int main(void) {
    sliceprintf("something", 1, 2, 3, 4, 5);
    sliceprintf("something", 11, 22, 33, 44, 55);
    sliceprintf("something", 111, 222, 333, 444, 555);
    sliceprintf("something", 1111, 2222, 3333, 4444, 5555);
    return 0;
}
