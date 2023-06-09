#include "ring_buffer.h"
#include "macros.h"

#include <stdlib.h> // malloc/free
#include <string.h> // memcpy
#include <stdint.h> // uint8_t
#include <stdio.h>

RingBuffer::RingBuffer()
: free_on_destroy(false) { }

RingBuffer::RingBuffer(void *buffer, size_t size)
: free_on_destroy(false)
{
    Init(buffer, size);
}

RingBuffer::RingBuffer(size_t size)
: free_on_destroy(false)
{
    Init(size);
}

RingBuffer::~RingBuffer()
{
    if(free_on_destroy)
        free(buffer);
}

void RingBuffer::Init(void *buffer, size_t size)
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

void RingBuffer::Init(size_t size)
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

size_t RingBuffer::BytesWritten()
{
    return bytes_written;
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

size_t RingBuffer::WriteOffset()
{
    return write_offset;
}

void RingBuffer::RewindWrite(size_t position)
{
    size_t bytes_to_rewind;
    if(position <= write_offset)
        bytes_to_rewind = write_offset - position;
    else
        bytes_to_rewind = write_offset + (size - position);

    ASSERT(bytes_to_rewind <= bytes_written);
    write_offset = position;
    bytes_written -= bytes_to_rewind;
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

size_t RingBuffer::ReadOffset()
{
    return read_offset;
}

void RingBuffer::RewindRead(size_t position)
{
    size_t bytes_to_rewind;
    if(position <= read_offset)
        bytes_to_rewind = read_offset - position;
    else
        bytes_to_rewind = read_offset + (size - position);

    ASSERT(bytes_written + bytes_to_rewind <= size);
    read_offset = position;
    bytes_written += bytes_to_rewind;
}