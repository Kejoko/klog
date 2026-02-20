# =========================================================================== #
# The Major, Minor, and Patch versions of Klog
# =========================================================================== #

function(set_klog_versions major minor patch)
    set(KLOG_VERSION_MAJOR ${major} PARENT_SCOPE)
    set(KLOG_VERSION_MINOR ${minor} PARENT_SCOPE)
    set(KLOG_VERSION_PATCH ${patch} PARENT_SCOPE)
endfunction()

# ----- Major Version 0 ----- #

# set_klog_versions(0 0 0)
# set_klog_versions(0 0 7) # Logging to stdout and files according to user configuration
set_klog_versions(0 0 8) # Pre-allocate message prefix buffers
