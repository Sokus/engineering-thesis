#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#include <stdio.h>
#include <assert.h>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

#define PE_STRINGIFY2(x) #x
#define PE_STRINGIFY(x) PE_STRINGIFY2(x)
#define PE_FILE_LINE __FILE__ ":" PE_STRINGIFY(__LINE__)

#ifdef NDEBUG
#define BREAK() do {} while (0)
#else // NDEBUG
#ifdef _WIN32
#define BREAK() __debugbreak()
#else // _WIN32
#include <signal.h>
#define BREAK() raise(SIGTRAP)
#endif // _WIN32
#endif // NDEBUG

#define ASSERT(condition) assert(condition)


#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#define MAX(a, b) (((a)<(b)) ? (b) : (a))
//#define CLAMP(a, x, b) (((x)<(a))?(a):\
//                        ((x)>(b))?(b):(x))
#define ABS(a) (((a) >= 0) ? (a) : -(a))
#define ABSF(a) (((a) >= 0.0f) ? (a) : (-a))
#define SIGN(x) (((x) > 0) - ((x) < 0))
#define SIGNF(x) (((x) > 0.0001f) - ((x) < 0.0001f))

#endif //BASE_COMMON_H