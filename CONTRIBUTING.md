# Contributing

## Naming

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

### Running Tests

You can either run tests via the `ctest` command, via the `run_all_tests.sh` script, or run an individual test manually.
The `run_all_tests.sh` script is the preferred way to run tests, because in addition to checking for correctness, it will also check for memory issues.

### Types of Tests

#### Unit Tests

The source files are named `ut-<function_name>.c`, and live within the library's `ut` subdirectory.
The compiled executables are output to the `bin/ut` directory.

These can be executed with the `ctest` command.

#### Functional Tests

These usually ensure correctness and behavior of a system through its exposed api and are implemented as normal looking executables.

The source files are named `ft-<something>.c`, and live within the library's `ft` subdirectory.
The compiled executables are output to the `bin/ft` directory.
