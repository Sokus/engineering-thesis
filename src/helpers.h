/* date = June 30th 2022 6:46 pm */

#ifndef HELPERS_H
#define HELPERS_H

#include <assert.h>
#include <stdint.h>
#include <string.h>

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

// memory arenas
typedef struct MemoryArena
{
    uint8_t *base;
    size_t size;
    size_t used;
} MemoryArena;

void InitializeArena(MemoryArena *arena, uint8_t *base, size_t size)
{
    arena->base = base;
    arena->size = size;
    arena->used = 0;
}

void ClearArena(MemoryArena *arena)
{
    arena->used = 0;
}

#define PUSH_STRUCT(arena, type) (type *)MemoryArenaPushSize(arena, sizeof(type))
#define PUSH_ARRAY(arena, type, count) (type *)MemoryArenaPushSize(arena, (count)*sizeof(type)) 

bool MemoryArenaCanFit(MemoryArena *arena, size_t size)
{
    bool result = ((arena->used + size) <= arena->size);
    return result;
}

void *MemoryArenaPushSize(MemoryArena *arena, size_t size)
{
    void *result = 0;
    bool can_fit = MemoryArenaCanFit(arena, size);
    ASSERT(can_fit);
    if(can_fit)
    {
        result = arena->base + arena->used;
        arena->used += size;
    }
    return result;
}

void *MemoryArenaPopSize(MemoryArena *arena, size_t size)
{
    ASSERT(size > 0);
    ASSERT(arena->used >= size);
    size_t new_used = arena->used - size;
    arena->used = new_used;
    void *result = arena->base + new_used;
    return result;
}

// strings
void ConcatenateStrings(char *str_a, size_t str_a_size,
                        char *str_b, size_t str_b_size,
                        char *dest, size_t dest_size)
{
    size_t dest_size_clamped = MAX(0, dest_size);
    size_t str_a_size_clamped = MIN(str_a_size, dest_size_clamped);
    size_t size_remaining = dest_size_clamped - str_a_size_clamped;
    size_t str_b_size_clamped = MIN(str_b_size, size_remaining);
    
    MEMORY_COPY(dest, str_a, str_a_size_clamped);
    MEMORY_COPY(dest+str_a_size_clamped, str_b, str_b_size_clamped);
    
    *(dest+str_a_size_clamped+str_b_size_clamped) = 0;
}

unsigned int StringLength(char *str)
{
    unsigned int result = 0;
    while(*str++)
    {
        ++result;
    }
    return result;
}

#endif //HELPERS_H
