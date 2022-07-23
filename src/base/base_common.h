/* date = July 21st 2022 6:51 pm */

#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#include <assert.h> // abort()
#include <string.h> // memcpy(), memset()

// helper macros
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#define MAX(a, b) (((a)<(b)) ? (b) : (a))
#define CLAMP(a, x, b) (((x)<(a))?(a):\
((x)>(b))?(b):(x))
#define ABS(a) (((a) >= 0) ? (a) : -(a))
#define ABSF(a) (((a) >= 0.0f) ? (a) : -(a))
#define SIGN(x) (((x) > 0) - ((x) < 0))

#define KILOBYTES(value) ((value)*1024LL)
#define MEGABYTES(value) (KILOBYTES(value)*1024LL)
#define GIGABYTES(value) (MEGABYTES(value)*1024LL)
#define TERABYTES(value) (GIGABYTES(value)*1024LL)

#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

#define STATEMENT(statement) do { statement } while (0)
#define ABORT() STATEMENT(abort();)
#define ASSERT(expression) STATEMENT(assert(expression);)
#define INVALID_CODE_PATH(message) STATEMENT(fprintf(stderr, message); ABORT();)

#define MEMORY_COPY(destination, source, size) memcpy(destination, source, size)
#define MEMORY_SET(destination, value, size) memset(destination, value, size)

#endif //BASE_COMMON_H
