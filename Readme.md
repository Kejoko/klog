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

Example: `cmake .. -DCMAKE_BUILD_TYPE=Release -DKLOG_DEBUG=true -DKLOG_BUILD_TESTS=true -DKLOG_BUILD_EXAMPLES=true`

## Contributing

Take a look at the [Contributing.md](./Contributing.md) file.
