# Klog

## Building

Prerequisites are cmake and ninja, and a c compiler of your choice with the ability to compile c99 source.
You also need valgrind if you wish to run all of the tests.

1. create a build directory
1. cd into your build directory
1. `cmake ..`, optionally with additional flags (listed below)
1. `ninja`

### Additional Flags

- `KLOG_DEBUG` for verbose debugging print information, for debugging Klog itself
- `KLOG_BUILD_TESTS` to build Klog's tests
- `KLOG_BUILD_EXAMPLES` to build Klog's examples
- `KLOG_BUILD_SCRATCH` to build Klog's scratch executables (you should not use this, these executables are for prototyping and exploring features)

Example: `cmake .. -DCMAKE_BUILD_TYPE=Release -DKLOG_DEBUG=true -DKLOG_BUILD_TESTS=true -DKLOG_BUILD_EXAMPLES=true -DKLOG_BUILD_SCRATCH=true`

## Contributing

### Naming

- Be consistent with hungarian prefixes (for arrays, pointers, etc)
    - Exs for KlogLoggerHandle
        - single instance: klog_logger_handle
        - global: g_klog_logger_handle
        - array (KlogLoggerHandle*): a_klog_logger_handles
        - pointer (KlogLoggerHandle*): p_klog_logger_handle
        - list (array of pointers, KlogLoggerHandle**): l_klog_logger_handle
- Be consistent with hungarian prefixes for strings
    - single string (char*): s_<name> or <name> depending on what the variable represents
    - list of strings (char**:) ls_<name>
    - single string char[]: same as char*
    - buffer (one malloc for many strings, packed together char*) b_<name>

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

# Release Questions
- [ ] Is everything documented correctly?
    - [ ] Structs
    - [ ] Functions
        - [ ] Prefixes
        - [ ] Parameters
        - [ ] How it uses the state which is exposed to the user
- [ ] Is the version incremented accordingly
- [ ] Is all of the state deinitialized correctly?
- [ ] Do all of your names adhere to the naming conventions?
- [ ] Is everything as const as possible?

# Planned Features
- [ ] async logging, with number of backing threads determined by user (using the message queue for consumers/producers design)
- [ ] console logger logs error and fatal logs to stderr
- [ ] windows support

# Tech Debt
## Source
- [ ] Preallocate buffers for the prefix
- [ ] Preallocate buffers for the messages
    - How are we going to handle this with newlines creating multiple messages?
        - As we iterate, populate the buffer, then consume? then populate, then consume?
- [ ] Enforce the message max length when formatting the message
    - Should this go into the split function?

## Utility
- [ ] Ensure we are cmaking the package correctly
- [ ] Create performance/profiling subset of tests (pt)
    - [ ] Introduce timing capabilities for our stress testing
    - [ ] stress tests
    - [ ] move the expansive file test to this (or make it smaller / log less things)

## Documentation
- [ ] Document all preconditions
- [ ] Be consistent with "string" and prefixes in names and order of words in names
    - Ex: "colored_level" vs "level_colored"
    - filename vs file_name
- [ ] Update all argument names to use prefixes and const correctly
- [ ] Update all function names to adhere to correct formatting

## Tests to write (in order of priority)
- [ ] filename prefix formatting
- [ ] source location formatting
- [ ] message prefix formatting
    - color and non-colored level strings
- [ ] actual message formatting
    - Format message using format specifiers ("%.3d", etc) and ensure the resulting message contains the correct values
    - Adherance to the maximum message length
- [ ] Functional test for all formatting
    - Ensure the final output is valid
    - Log to file and ensure line contains the correct message
- [ ] Ensure formatting works as expected
- [ ] More failing tests
- [ ] Performance test using lots of loggers and lots of log statements (hundreds and thousands)
    - [ ] With async backing threads, from single threaded application
    - [ ] With async backing threads, from multithreaded application
    - [ ] With NO async backing threads, from single threaded application
    - [ ] For console, and for file

# Notes

