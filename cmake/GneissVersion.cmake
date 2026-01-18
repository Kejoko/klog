# =========================================================================== #
# The Major, Minor, and Patch versions of Gneiss
# =========================================================================== #

function(set_gneiss_versions major minor patch)
    set(GNEISS_VERSION_MAJOR ${major} PARENT_SCOPE)
    set(GNEISS_VERSION_MINOR ${minor} PARENT_SCOPE)
    set(GNEISS_VERSION_PATCH ${patch} PARENT_SCOPE)
endfunction()

# ----- Major Version 0 ----- #

set_gneiss_versions(0 0 0)
