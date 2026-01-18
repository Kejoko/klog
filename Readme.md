# Klog

## Testing

Run unit tests using the ctest command.

Test all unit tests for memory leaks using valgrind (from within your build directory): `valgrind --tool=memcheck ./bin/ut/<your_test>`)

### Types of Tests

#### Unit Tests

These validate the correctness of a single function (the smallest unit of translation).

Unit tests are usually implemented using the unit testing framework, if possible.

The source files are named `ut-<function_name>.c`, and live within the library's `ut` subdirectory.
The compiled executables are output to the `bin/ut` directory.

These can be executed with the `ctest` command.

#### Functional Tests

These validate the correctness of a system or library's functionality at a higher level.

These usually ensure correctness and behavior of a system through its exposed api and are implemented as normal looking executables.

The source files are named `ft-<something>.c`, and live within the library's `ft` subdirectory.
The compiled executables are output to the `bin/ft` directory.

### Running Tests

You can either run tests via the `ctest` command, via the `run_all_tests.sh` script, or run an individual test manually.
The `run_all_tests.sh` script is the preferred way to run tests, because in addition to checking for correctness, it will also check for memory issues.

# Tech Debt

## Build System
- [ ] cmake flag to prevent building all tests
- [ ] Cmake flag to prevent building scratch executables
## Source
- [ ] Helper tool to free klog_format_split_t
- [ ] Hide the implementation of the klog handle, return a pointer to the handle
- [ ] Unit test the rest of klog's implementation
- [ ] Ensure const correctness everywhere
