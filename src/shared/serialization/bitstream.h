#ifndef PE_BITSTREAM_H
#define PE_BITSTREAM_H

#include "bitpacker.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum BitStreamMode
{
    BIT_STREAM_INVALID = 0,
    WRITE_STREAM,
    READ_STREAM,
    MEASURE_STREAM
} BitStreamMode;

typedef struct BitStream
{
    BitStreamMode mode = BIT_STREAM_INVALID;
    int error = 0;
    union {
        BitWriter bit_writer;
        BitReader bit_reader;
    };
    int bits_read = 0; // READ_STREAM
    int total_bytes = 0; // MEASURE_STREAM
    int bits_written = 0; // MEASURE_STREAM

    BitStream();
} BitStream;
typedef BitStream WriteStream;
typedef BitStream ReadStream;
typedef BitStream MeasureStream;

WriteStream WriteStream_Create(void *buffer, int bytes);
ReadStream ReadStream_Create(void *buffer, int bytes);
MeasureStream MeasureStream_Create(void *buffer, int bytes);

bool BitStream_SerializeInteger(BitStream *stream, int32_t *value, int32_t min, int32_t max);
bool BitStream_SerializeBits(BitStream *stream, uint32_t *value, int bits);
bool BitStream_SerializeBytes(BitStream *stream, uint8_t *data, int bytes);
bool BitStream_SerializeAlign(BitStream *stream);
int BitStream_GetAlignBits(BitStream *stream);
bool BitStream_SerializeCheck(BitStream *stream, const char *string);
void BitStream_Flush(BitStream *stream);
int BitStream_GetBitsProcessed(BitStream *stream);
int BitStream_GetBytesProcessed(BitStream *stream);
int BitStream_GetBitsRemaining(BitStream *stream);
int BitStream_GetBitsRemaining(BitStream *stream);
int BitStream_GetTotalBits(BitStream *stream);
int BitStream_GetTotalBytes(BitStream *stream);
int BitStream_GetBytesRead(BitStream *stream);
int BitStream_GetError(BitStream *stream);

int BitsRequired(unsigned int value);
int BitsRequiredForRange(unsigned int min, unsigned int max);
uint32_t HashString(const char *string, uint32_t hash);

#endif // PE_BITSTREAM_H