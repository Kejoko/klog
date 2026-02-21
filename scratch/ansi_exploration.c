#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void) {
    const char* s = "\x1b[41mFATAL\x1b[0m";
    const uint32_t length = strlen(s);
    printf("%u\n", length);
    return 0;
}
