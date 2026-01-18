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

## Test Suite
- [x] Update test creation function to denote whether or not it should fail, and whether or not the memory should be valid
    - [x] Expect fail = false default
    - [x] Expect memory errors = false default
- [x] Update test creation function to populate text files containing names of expected failing tests and expected memory issue tests
- [ ] Script to run all tests
    - [ ] Output name of test, with pass or fail information
    - [ ] Execute test and check return code
    - [ ] Run with valgrind
- [ ] CMake copy/link test runner script to binary directory
## Build System
- [ ] move scratch executables out of test/ directory
- [ ] cmake flag to prevent building all tests
- [ ] Cmake flag to prevent building scratch executables
## Source
- [ ] Rename the klog_format_split_t struct members to omit the "format" word
- [ ] Helper tool to free klog_format_split_t
- [ ] Unit test the rest of klog's implementation
- [ ] Ensure const correctness everywhere
