#ifndef KLOG_IMPL_MESSAGE_CREATION_INCLUDED
#define KLOG_IMPL_MESSAGE_CREATION_INCLUDED

#include <stdint.h>

#ifdef _WIN32
#error "Klog does not support Windows"
#endif

#ifdef __APPLE__
#error "Klog does not support MacOS"
#endif

#ifdef  __linux__
#include <unistd.h>
#include <sys/syscall.h>
#ifndef SYS_gettid
#error "SYS_gettid is unavailable on this system"
#endif
#endif

#include "klog/klog.h"

pid_t klog_impl_get_current_thread_id(void);

const char* klog_impl_get_level_string(const enum klog_level_e requested_level);

#endif /* KLOG_IMPL_MESSAGE_CREATION_INCLUDED */
