#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#include <assert.h> // assert
#include <stdlib.h> // abort
#include <string.h> // memset, memcpy

#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))
#define ASSERT(expression) assert(expression)
#define STATEMENT(statement) do { statement } while (0)
#define ABORT() abort()

#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#define MAX(a, b) (((a)<(b)) ? (b) : (a))
//#define CLAMP(a, x, b) (((x)<(a))?(a):\
//                        ((x)>(b))?(b):(x))
#define ABS(a) (((a) >= 0) ? (a) : -(a))
#define SIGN(x) (((x) > 0) - ((x) < 0))

#define MEMORY_COPY(destination, source, size) memcpy(destination, source, size)
#define MEMORY_SET(destination, value, size) memset(destination, value, size)

#endif //BASE_COMMON_H