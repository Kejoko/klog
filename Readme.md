# Klog

## Testing

Run unit tests using the ctest command.

Test all unit tests for memory leaks using valgrind (from within your build directory):`valgrind --tool=memcheck ./bin/ut/<your_test>`)

# Tech Debt

- [ ] Rename GNEISS_MAJOR_VERSION etc to GNEISS_VERSION_MAJOR etc
- [ ] Rename the klog_format_split_t struct members to omit the "format" word
- [ ] Cmake flag to turn off scratch executables
- [ ] Cmake flag to turn on debugging of klog (KLOG_DEBUG macro enabling a klog print statement)
    - [ ] Use the custom klog debugging print statement
- [ ] Helper tool to free klog_format_split_t
- [ ] Unit test the rest of klog's implementation
- [ ] CMake tool to make valgrind-ing everything (all unit tests, unless marked otherwise) easy
- [ ] Ensure const correctness everywhere
