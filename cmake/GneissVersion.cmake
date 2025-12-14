# =========================================================================== #
# The Major, Minor, and Patch versions of Quartz
# =========================================================================== #

function(set_gneiss_major_minor_patch_versions major minor patch)
    set(GNEISS_MAJOR_VERSION ${major} PARENT_SCOPE)
    set(GNEISS_MINOR_VERSION ${minor} PARENT_SCOPE)
    set(GNEISS_PATCH_VERSION ${patch} PARENT_SCOPE)
endfunction()

# ----- Major Version 0 ----- #

set_gneiss_major_minor_patch_versions(0 0 0)
