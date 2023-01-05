#ifndef PE_BITPACKER_H
#define PE_BITPACKER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum BitPackerMode
{
    BIT_PACKER_INVALID = 0,
    BIT_WRITER,
    BIT_READER
} BitPackerMode;

typedef struct BitPacker
{
    BitPackerMode mode;
    uint32_t *data;
    uint64_t scratch;
    int num_bits;
    int num_words;
    int word_index;
    int scratch_bits;

    int bits_written; // BitWriter only
    int bits_read;    // BitReader only
    int num_bytes;    // BitReader only
} BitPacker;

typedef BitPacker BitWriter;
BitWriter BitWriter_Create(void *data, int bytes);
void BitWriter_WriteBits(BitWriter *bit_writer, uint32_t value, int bits);
void BitWriter_WriteAlign(BitWriter *bit_writer);
void BitWriter_WriteBytes(BitWriter *bit_writer, uint8_t *data, int bytes);
void BitWriter_FlushBits(BitWriter *bit_writer);
int BitWriter_GetAlignBits(BitWriter *bit_packer);
int BitWriter_GetBitsWritten(BitWriter *bit_writer);
int BitWriter_BitsAvailable(BitWriter *bit_writer);
int BitWriter_GetBytesWritten(BitWriter *bit_writer);
int BitWriter_GetTotalBytes(BitWriter *bit_packer);

typedef BitPacker BitReader;
BitReader BitReader_Create(void *data, int bytes);
bool BitReader_WouldOverflow(BitReader *bit_reader, int bits);
uint32_t BitReader_ReadBits(BitReader *bit_reader, int bits);
bool BitReader_Align(BitReader *bit_reader);
void BitReader_ReadBytes(BitReader *bit_reader, uint8_t *data, int bytes);
int BitReader_GetAlignBits(BitReader *bit_packer);
int BitReader_BitsRead(BitReader *bit_reader);
int BitReader_GetBytesRead(BitReader *bit_reader);
int BitReader_GetBitsRemaining(BitReader *bit_reader);
int BitReader_BytesRemaining(BitReader *bit_reader);
int BitReader_GetTotalBits(BitReader *bit_reader);
int BitReader_GetTotalBytes(BitReader *bit_packer);

#endif // PE_BITPACKER_H