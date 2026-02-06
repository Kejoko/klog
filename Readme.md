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
- [ ] Guard against newlines in logger names (convert them to spaces)
- [ ] Introduce timing capabilities for our stress testing
- [ ] Create performance/profiling subset of tests (pt)
    - [ ] stress tests
    - [ ] move the expansive file test to this (or make it smaller / log less things)
- [ ] Preallocate buffers for the prefix
- [ ] Preallocate buffers for the messages
- [ ] Enforce the message max length
- [ ] Enforce that the level cannot be a higher value than TRACE
    - exit if we log at a higher level
    - exit if we set the level to a higher level
    - exit if we initialize with a higher level
- [ ] Define klog exit code and exit with this
    - enables user to discern that program exited due to klog issue

## Utility
- [ ] Ensure we are cmaking the package correctly

## Documentation
- [ ] Document all preconditions
- [ ] Be consistent with "string" and prefixes in names and order of words in names
    - Ex: "colored_level" vs "level_colored"
- [ ] Update all argument names to use prefixes and const correctly
- [ ] Update all function names to adhere to correct formatting

## Tests to write
- [ ] Setting level and logging works correctly
    - set the level
    - query the level
    - log to tempfile with various levels, ensure file contents are correct
    - working in conjunction with the max verbosity for the file and console configs
- [ ] Message max length actually truncates the message
- [ ] Lots of loggers and lots of log statements (hundreds and thousands)
- [ ] With async backing threads, from single threaded application
- [ ] With async backing threads, from multithreaded application
- [ ] With NO async backing threads, from single threaded application
- [ ] Stress tests for files too
- [ ] Formatting prefix strings correctly (for all possible combinations)
    - Output to tempfile, read the contents and ensure the prefixes look correct
- [ ] With thread id
- [ ] Without thread id
- [ ] Source location string formatting
    - [ ] Longer filenames than allowed
    - [ ] Shorter filenames than allowed
- [ ] Undefine and redefine KLOG_OFF and other macros before tests
    - probably won't work because we can't change the definition inside of the already compiled source file

# Notes

