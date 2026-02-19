#include <stdio.h>

int main(void) {
    printf("__FILE__ = %s\n", __FILE__);

    printf("__FILE_NAME__ = %s\n", __FILE_NAME__);

    printf("__LINE__ = %d\n", __LINE__);

    printf("__func__ = %s\n", __func__);

    // Not available without clang
    // printf("__PRETTY_FUNCTION__ = %s\n", __PRETTY_FUNCTION__);

    return 0;
}
