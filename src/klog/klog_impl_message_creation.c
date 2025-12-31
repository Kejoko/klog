#include <stdint.h>

#include "klog/klog.h"
#include "./klog_impl_message_creation.h"
#include "./klog_impl_state.h"

pid_t klog_impl_get_current_thread_id(void) {
    return syscall(SYS_gettid);
}

const char* klog_impl_get_level_string(const enum klog_level_e requested_level) {
    const uint32_t i_level_string = G_klog_level_string_length * requested_level;
    return &gp_klog_level_strings[i_level_string];
}
