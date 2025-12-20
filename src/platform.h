#ifndef PLATFORM_H_INCLUDE
#define PLATFORM_H_INCLUDE

#ifdef __linux__
#define GNEISS_OS_LINUX
#endif

#ifdef _WIN32
#define GNEISS_OS_WINDOWS
#endif

#ifdef __APPLE__
#define GNEISS_OS_MACOS
#endif

#endif /* PLATFORM_H_INCLUDE */
