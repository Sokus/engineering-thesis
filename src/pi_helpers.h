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
    void *base;
    size_t size;
    size_t used;
} MemoryArena;

#define ARENA_PUSH_STRUCT(arena, type) (type *)MemoryArenaPushSize(arena, sizeof(type))
#define ARENA_PUSH_ARRAY(arena, type, count) (type *)MemoryArenaPushSize(arena, (count)*sizeof(type)) 
#define ARENA_POP_STRUCT(arena, type) MemoryArenaPopSize(arena, sizeof(type))
#define ARENA_POP_ARRAY(arena, type, count) MemoryArenaPopSize(arena, (count)*sizeof(type))

void InitializeArena(MemoryArena *arena, void *base, size_t size)
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
        result = (uint8_t *)arena->base + arena->used;
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
    void *result = (uint8_t *)arena->base + new_used;
    return result;
}

// RING BUFFER
// ===========

struct RingBuffer
{
    void *base;
    size_t write;
    size_t read;
    size_t size;
    size_t used;
};

void InitializeRingBuffer(RingBuffer *rbuf, void *base, size_t size)
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

struct RingBufferArea
{
    void *first_part;
    size_t first_part_size;
    void *second_part;
    size_t second_part_size;
};

RingBufferArea RingBufferPop(RingBuffer *rbuf, size_t amount)
{
    ASSERT(rbuf != 0);
    RingBufferArea area = {};
    
    if(amount <= rbuf->used)
    {
        size_t contiguous_space = rbuf->size - rbuf->read;
        if(amount <= contiguous_space)
        {
            area.first_part = (uint8_t *)rbuf->base + rbuf->read;
            area.first_part_size = amount;
        }
        else
        {
            size_t overhead = rbuf->read + amount - rbuf->size;
            area.first_part = (uint8_t *)rbuf->base + rbuf->read;
            area.first_part_size = contiguous_space;
            area.second_part = rbuf->base;
            area.second_part_size = overhead;
        }
        
        rbuf->read = (rbuf->read + amount) % rbuf->size;
        rbuf->used -= amount;
    }
    
    return area;
}

RingBufferArea RingBufferPush(RingBuffer *rbuf, size_t amount)
{
    ASSERT(rbuf != 0);
    RingBufferArea area = {};
    
    if(amount <= rbuf->size - rbuf->used)
    {
        size_t contiguous_space = rbuf->size - rbuf->write;
        if(amount <= contiguous_space)
        {
            area.first_part = (uint8_t *)rbuf->base + rbuf->write;
            area.first_part_size = amount;
        }
        else
        {
            size_t overhead = rbuf->write + amount - rbuf->size;
            area.first_part = (uint8_t *)rbuf->base + rbuf->write;
            area.first_part_size = contiguous_space;
            area.second_part = rbuf->base;
            area.second_part_size = overhead;
        }
        
        rbuf->write = (rbuf->write + amount) % rbuf->size;
        rbuf->used += amount;
    }
    
    return area;
}

void RingBufferWrite(RingBuffer *rbuf, void *data, size_t data_size)
{
    RingBufferArea area = RingBufferPush(rbuf, data_size);
    
    if(area.first_part != 0)
    {
        MEMORY_COPY(area.first_part, data, area.first_part_size);
        
        if(area.second_part != 0)
        {
            MEMORY_COPY(area.second_part,
                        (uint8_t *)data + area.first_part_size,
                        area.second_part_size);
        }
    }
}

void RingBufferRead(RingBuffer *rbuf, void *data, size_t data_size)
{
    RingBufferArea area = RingBufferPop(rbuf, data_size);
    if(area.first_part != 0)
    {
        MEMORY_COPY(data, area.first_part, area.first_part_size);
        
        if(area.second_part != 0)
        {
            MEMORY_COPY((uint8_t *)data + area.first_part_size,
                        area.second_part,
                        area.second_part_size);
        }
    }
}

void RingBufferAdvanceReadPointer(RingBuffer *rbuf, size_t move_amount)
{
    ASSERT(move_amount <= RingBufferUsedSpace(rbuf));
    rbuf->read = (rbuf->read + rbuf->size - move_amount) % rbuf->size;
    rbuf->used -= move_amount;
}

void RingBufferRewindReadPointer(RingBuffer *rbuf, size_t move_amount)
{
    ASSERT(move_amount <= RingBufferFreeSpace(rbuf));
    rbuf->read = (rbuf->read + move_amount) % rbuf->size;
    rbuf->used += move_amount;
}

// STREAM BUFFER
// =============

struct StreamBuffer
{
    void *base;
    size_t head;
    size_t size;
    size_t used;
};

#define STREAM_PUSH_STRUCT(sbuf, type) (type *)StreamBufferPush(sbuf, sizeof(type))
#define STREAM_PUSH_ARRAY(sbuf, type, count) (type *)StreamBufferPush(sbuf, (count)*sizeof(type)) 
#define STREAM_POP_STRUCT(sbuf, type) StreamBufferPop(sbuf, sizeof(type))
#define STREAM_POP_ARRAY(sbuf, type, count) StreamBufferPop(sbuf, (count)*sizeof(type))

void InitializeStreamBuffer(StreamBuffer *sbuf, void *base, size_t size)
{
    ASSERT(sbuf != 0);
    ASSERT(base != 0);
    ASSERT(size > 0);
    sbuf->base = base;
    sbuf->head = 0;
    sbuf->size = size;
    sbuf->used = 0;
}

StreamBuffer CreateStreamBuffer(void *base, size_t size)
{
    StreamBuffer result;
    InitializeStreamBuffer(&result, base, size);
    return result;
}

void ClearStreamBuffer(StreamBuffer *sbuf)
{
    ASSERT(sbuf != 0);
    sbuf->head = 0;
    sbuf->used = 0;
}

bool StreamBufferIsFull(StreamBuffer *sbuf)
{
    ASSERT(sbuf != 0);
    bool result = (sbuf->used == sbuf->size);
    return result;
}

bool StreamBufferIsEmpty(StreamBuffer *sbuf)
{
    ASSERT(sbuf != 0);
    bool result = (sbuf->used == 0);
    return result;
}

size_t StreamBufferSize(StreamBuffer *sbuf)
{
    ASSERT(sbuf != 0);
    return sbuf->size;
}

size_t StreamBufferUsedSpace(StreamBuffer *sbuf)
{
    ASSERT(sbuf != 0);
    return sbuf->used;
}

size_t StreamBufferFreeSpace(StreamBuffer *sbuf)
{
    ASSERT(sbuf != 0);
    size_t result = sbuf->size - sbuf->used;
    return result;
}

size_t StreamBufferHeadPosition(StreamBuffer *sbuf)
{
    ASSERT(sbuf != 0);
    return sbuf->head;
}

void StreamBufferSetPointer(StreamBuffer *sbuf, size_t position)
{
    ASSERT(sbuf != 0);
    ASSERT(position <= sbuf->used);
    sbuf->head = position;
}

void *StreamBufferPush(StreamBuffer *sbuf, size_t data_size)
{
    ASSERT(sbuf != 0);
    void *result = 0;
    
    if(sbuf->head + data_size <= sbuf->size)
    {
        result = (uint8_t *)sbuf->base + sbuf->head;
        sbuf->head += data_size;
        sbuf->used = MAX(sbuf->used, sbuf->head);
    }
    
    return result;
}

void *StreamBufferPop(StreamBuffer *sbuf, size_t data_size)
{
    ASSERT(sbuf != 0);
    void *result = 0;
    
    if(sbuf->head + data_size <= sbuf->used)
    {
        result = (uint8_t *)sbuf->base + sbuf->head;
        sbuf->head += data_size;
    }
    
    return result;
}

bool StreamBufferWrite(StreamBuffer *sbuf, void *data, size_t data_size)
{
    ASSERT(sbuf != 0);
    bool success = false;
    
    void *destination = StreamBufferPush(sbuf, data_size);
    
    if(destination != 0)
    {
        MEMORY_COPY(destination, data, data_size);
        success = true;
    }
    return success;
}

bool StreamBufferRead(StreamBuffer *sbuf, void *data, size_t data_size)
{
    ASSERT(sbuf != 0);
    bool success = false;
    
    void *source = StreamBufferPop(sbuf, data_size);
    
    if(source != 0)
    {
        MEMORY_COPY(data, source, data_size);
        success = true;
    }
    return success;
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
