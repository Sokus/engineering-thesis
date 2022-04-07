/* date = April 1st 2022 9:27 am */

#ifndef PI_HELPERS_H
#define PI_HELPERS_H

#include <stdio.h> // fprintf
#include <string.h> // memset, memcpy
#include <stdint.h> // uint8_t

// KEYWORDS
// ========

#define global static
#define internal static
#define local_persist static

// HELPER MACROS
// =============

#define STATEMENT(statement) do { statement } while (0)

#define RAISE_SEGMENTATION_FAULT() (*((char*)-1) = 'x')
#define ASSERT_BREAK(message) \
STATEMENT(fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, message); \
RAISE_SEGMENTATION_FAULT();)
#define ASSERT(expression) \
STATEMENT( if(!(expression)) { ASSERT_BREAK(#expression); })
#define INVALID_CODE_PATH ASSERT_BREAK("")


#define ARRAY_COUNT(array) ( sizeof(array)/sizeof((array)[0]) )

#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#define MAX(a, b) (((a)<(b)) ? (b) : (a))
#define CLAMP(a, x, b) (((x)<(a))?(a):\
((x)>(b))?(b):(x))
#define ABS(a) ((a) >= 0) ? (a) : -(a)
#define SIGN(x) (((x) > 0) - ((x) < 0))

#define SWAP(type, a, b) STATEMENT(type swap=a; a=b; b=swap;)

#define MEMORY_COPY(destination, source, size) memcpy(destination, source, size)
#define MEMORY_SET(destination, value, size) memset(destination, value, size)

// MEMORY ARENAS
// =============

typedef struct MemoryArena
{
    uint8_t *base;
    size_t size;
    size_t used;
} MemoryArena;

#define ARENA_PUSH_STRUCT(arena, type) (type *)MemoryArenaPushSize(arena, sizeof(type))
#define ARENA_PUSH_ARRAY(arena, type, count) (type *)MemoryArenaPushSize(arena, (count)*sizeof(type)) 
#define ARENA_POP_STRUCT(arena, type) MemoryArenaPopSize(arena, sizeof(type))
#define ARENA_POP_ARRAY(arena, type, count) MemoryArenaPopSize(arena, (count)*sizeof(type))

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

size_t MemoryArenaFreeSpace(MemoryArena *arena)
{
    size_t result = (arena->size - arena->used);
    return result;
}

void *MemoryArenaPushSize(MemoryArena *arena, size_t size)
{
    void *result = 0;
    bool can_fit = ((arena->used + size) <= arena->size);
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

// RING BUFFER
// ===========

struct RingBuffer
{
    uint8_t *base;
    size_t write;
    size_t read;
    size_t size;
    size_t used;
};

void InitializeRingBuffer(RingBuffer *rbuf, uint8_t *base, size_t size)
{
    ASSERT(rbuf != 0);
    ASSERT(base != 0);
    ASSERT(size > 0);
    rbuf->base = base;
    rbuf->write = 0;
    rbuf->read = 0;
    rbuf->size = size;
    rbuf->used = 0;
}

void ClearRingBuffer(RingBuffer *rbuf)
{
    ASSERT(rbuf != 0);
    rbuf->write = 0;
    rbuf->read = 0;
    rbuf->used = 0;
}

bool RingBufferIsFull(RingBuffer *rbuf)
{
    ASSERT(rbuf != 0);
    bool result = (rbuf->used == rbuf->size);
    return result;
}

bool RingBufferIsEmpty(RingBuffer *rbuf)
{
    ASSERT(rbuf != 0);
    bool result = (rbuf->used == 0);
    return result;
}

size_t RingBufferSize(RingBuffer *rbuf)
{
    ASSERT(rbuf != 0);
    return rbuf->size;
}

size_t RingBufferUsedSpace(RingBuffer *rbuf)
{
    ASSERT(rbuf != 0);
    return rbuf->used;
}

size_t RingBufferFreeSpace(RingBuffer *rbuf)
{
    ASSERT(rbuf != 0);
    size_t result = rbuf->size - rbuf->used;
    return result;
}

void RingBufferPushSize(RingBuffer *rbuf, uint8_t *data, size_t data_size)
{
    ASSERT(rbuf != 0);
    bool can_fit = (rbuf->used + data_size <= rbuf->size);
    ASSERT(can_fit);
    
    size_t contiguous_space = rbuf->size - rbuf->write;
    if(data_size <= contiguous_space)
    {
        MEMORY_COPY(rbuf->base + rbuf->write, data, data_size);
    }
    else
    {
        size_t overhead = rbuf->write + data_size - rbuf->size;
        MEMORY_COPY(rbuf->base + rbuf->write, data, contiguous_space);
        MEMORY_COPY(rbuf->base, data + contiguous_space, overhead);
    }
    
    rbuf->write = (rbuf->write + data_size) % rbuf->size;
    rbuf->used += data_size;
}

void RingBufferPopSize(RingBuffer *rbuf, uint8_t *out_data, size_t data_size)
{
    ASSERT(rbuf != 0);
    bool can_be_read = (data_size <= rbuf->used);
    ASSERT(can_be_read);
    
    size_t contiguous_space = rbuf->size - rbuf->read;
    if(data_size <= contiguous_space)
    {
        MEMORY_COPY(out_data, rbuf->base + rbuf->read, data_size);
    }
    else
    {
        size_t overhead = rbuf->read + data_size - rbuf->size;
        MEMORY_COPY(out_data, rbuf->base + rbuf->read, contiguous_space);
        MEMORY_COPY(out_data + contiguous_space, rbuf->base, overhead);
    }
    
    rbuf->read = (rbuf->read + data_size) % rbuf->size;
    rbuf->used -= data_size;
}


// STRING
// ======

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

#endif //PI_HELPERS_H
