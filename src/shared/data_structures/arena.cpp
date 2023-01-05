#include "data_structures.h"
#include "macros.h"

#include <stdlib.h> // malloc/free
#include <string.h> // memcpy
#include <stdint.h> // uint8_t
#include <stdio.h>

Arena::Arena()
: free_on_destroy(false)
{ }

Arena::Arena(void *buffer, size_t size)
: free_on_destroy(false)
{
    Init(buffer, size);
}

Arena::Arena(size_t size)
: free_on_destroy(false)
{
    Init(buffer, size);
}

Arena::~Arena()
{
    if(free_on_destroy)
        free(buffer);
}

void Arena::Init(void *buffer, size_t size)
{
    if(free_on_destroy)
    {
        free(this->buffer);
        free_on_destroy = false;
    }

    this->buffer = buffer;
    this->size = size;
    Clear();
}

void Arena::Init(size_t size)
{
    if(free_on_destroy)
    {
        free(this->buffer);
        free_on_destroy = false;
    }

    this->buffer = malloc(size);
    this->size = size;
    Clear();
}

void Arena::Clear()
{
    used = 0;
}

bool Arena::WouldOverflow(size_t bytes)
{
    return used + bytes > size;
}

void *Arena::Push(size_t size)
{
    void *result = 0;
    bool can_fit = !WouldOverflow(size);
    ASSERT(can_fit);
    if(can_fit)
    {
        result = (uint8_t *)buffer + used;
        used += size;
    }
    return result;
}

void Arena::Pop(size_t size)
{
    ASSERT(used >= size);
    used -= size;
}

size_t Arena::BytesUsed()
{
    return used;
}

void Arena::Rewind(size_t offset)
{
    ASSERT(offset >= 0 && offset <= size);
    this->used = offset;
}