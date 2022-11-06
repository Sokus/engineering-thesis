#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h> // size_t

class RingBuffer
{
private:
    void *buffer;
    size_t size;
    size_t write_offset;
    size_t read_offset;
    size_t bytes_written;

    bool free_on_destroy;
public:
    RingBuffer();
    RingBuffer(void *buffer, size_t size);
    RingBuffer(size_t size);
    ~RingBuffer();

    void Clear();
    bool WouldOverflow(size_t bytes);
    size_t BytesWritten();

    void Write(void *data, size_t bytes);
    size_t WriteOffset();
    void RewindWrite(size_t position);

    void Read(void *data, size_t bytes);
    size_t ReadOffset();
    void RewindRead(size_t position);



};

#endif // DATA_STRUCTURES_H