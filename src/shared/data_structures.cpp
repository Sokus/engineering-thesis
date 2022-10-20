#include "data_structures.h"
#include "macros.h"

#include <stdlib.h> // malloc/free
#include <string.h> // memcpy
#include <stdint.h> // uint8_t

RingBuffer::RingBuffer(void *buffer, size_t size)
: buffer(buffer), size(size)
{
    this->free_on_destroy = false;
    Clear();
}

RingBuffer::RingBuffer(size_t size)
: size(size)
{
    buffer = malloc(size);
    free_on_destroy = true;
    Clear();
}

RingBuffer::~RingBuffer()
{
    if(free_on_destroy)
        free(buffer);
}

void RingBuffer::Clear()
{
    write_offset = 0;
    read_offset = 0;
    bytes_written = 0;
}

bool RingBuffer::WouldOverflow(size_t bytes)
{
    return bytes_written + bytes > size;
}

void RingBuffer::Write(void *data, size_t bytes)
{
    ASSERT(bytes_written + bytes <= size);
    size_t bytes_to_wrap = size - write_offset;
    if(bytes <= bytes_to_wrap)
    {
        memcpy((uint8_t *)buffer + write_offset, data, bytes);
    }
    else
    {
        memcpy((uint8_t *)buffer + write_offset, data, bytes_to_wrap);
        memcpy(buffer, (uint8_t *)data + bytes_to_wrap, bytes - bytes_to_wrap);
    }

    write_offset = (write_offset + bytes) % size;
    bytes_written += bytes;
}

void RingBuffer::Write(int value)
{
    Write(&value, sizeof(value));
}

void RingBuffer::Read(void *data, size_t bytes)
{
    ASSERT(bytes <= bytes_written);
    size_t bytes_to_wrap = size - read_offset;
    if(bytes <= bytes_to_wrap)
    {
        memcpy(data, (uint8_t *)buffer + read_offset, bytes);
    }
    else
    {
        memcpy(data, (uint8_t *)buffer + read_offset, bytes_to_wrap);
        memcpy((uint8_t *)data + bytes_to_wrap, buffer, bytes - bytes_to_wrap);
    }

    read_offset = (read_offset + bytes) % size;
    bytes_written -= bytes;
}

void RingBuffer::Read(int& value)
{
    Read(&value, sizeof(value));
}