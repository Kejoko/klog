# Klog

## Building

Prerequisites are cmake and ninja, and a c compiler of your choice.
You also need valgrind if you wish to run all of the tests.

1. create a build directory
1. cd into your build directory
1. `cmake ..`, optionally with additional flags (listed below)
1. `ninja`

### Additional Flags

- `KLOG_DEBUG` for verbose debugging print information, for debugging Klog itself
- `KLOG_BUILD_TESTS` to build Klog's tests
- `KLOG_BUILD_EXAMPLES` to build Klog's examples
- `KLOG_BUILD_SCRATCH` to build Klog's scratch executables (you should not use this)

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
    - single string (char*): <name> - don't need a prefix
    - list of strings (char**:) ls_<name>
    - single string char[]: <name>
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

# Planned Features

- [ ] stdout logging with color based on level (if enabled)
- [ ] log to file, whose file name prefix is determined by user
- [ ] log the time, if enabled by user
- [ ] log the source location (file, line), if enabled by user
- [ ] async logging, with number of backing threads determined by user (using the message queue for consumers/producers design)
- [ ] windows support

# Tech Debt
## Source
- [ ] Move everything (name, level) within the handle?
- [ ] Ensure const correctness everywhere
