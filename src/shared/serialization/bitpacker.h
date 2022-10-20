#ifndef PI_BITPACKER_H
#define PI_BITPACKER_H

#include <stdint.h>

typedef enum BitPackerMode
{
    BIT_PACKER_INVALID = 0,
    BIT_WRITER,
    BIT_READER
} BitPackerMode;

typedef struct BitPacker
{
    BitPackerMode mode;
    uint64_t scratch;
    int scratch_bits;
    uint32_t *buffer;
    int word_count;

    // BIT_WRITER
    int word_capacity;

    // BIT_READER
    int bits_read;
} BitPacker;

// BIT_WRITER
BitPacker BitWriter(void *buffer, int bytes);
int BitsWritten(BitPacker *bit_writer);
int BytesWritten(BitPacker *bit_writer);
void WriteBits(BitPacker *bit_writer, uint32_t *value, int bits);
void Flush(BitPacker *bit_writer);

// BIT_READER
BitPacker BitReader(void *buffer, int bytes);
int BitsRead(BitPacker *bit_reader);
int BytesRead(BitPacker *bit_reader);
void ReadBits(BitPacker *bit_reader, uint32_t *value, int bits);
void Rewind(BitPacker *bit_reader, int position);

// General
bool WouldOverflow(BitPacker *bit_packer, int bits);
bool Align(BitPacker *bit_packer);
void Reset(BitPacker *bit_packer);

#endif // PI_BITPACKER_H