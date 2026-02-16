# =========================================================================== #
# Scratch/Sandbox Tests                                                       #
# =========================================================================== #

include(CreateScratchExecutable)

if (KLOG_BUILD_SCRATCH)
    create_scratch_executable(array_exploration.c)
    create_scratch_executable(color_exploration.c)
    create_scratch_executable(macro_exploration.c)
    create_scratch_executable(munmap_chunk_exploration.c)
    create_scratch_executable(runtime_array_append_exploration.c)
    create_scratch_executable(time_exploration.c)
    create_scratch_executable(type_exploration.c)
endif ()
