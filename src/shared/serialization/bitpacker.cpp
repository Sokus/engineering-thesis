#include "bitpacker.h"
#include "macros.h"

#include <string.h>

BitWriter BitWriter_Create(void *data, int bytes)
{
    ASSERT(data);
    BitWriter bit_writer;
    bit_writer.mode = BIT_WRITER;
    bit_writer.data = (uint32_t *)data;
    bit_writer.num_words = bytes / 4;
    bit_writer.num_bits = bit_writer.num_words * 32;
    return bit_writer;
}

void BitWriter_WriteBits(BitWriter *bit_writer, uint32_t value, int bits)
{
    ASSERT(bits > 0);
    ASSERT(bits <= 32);
    ASSERT(bit_writer->bits_written + bits <= bit_writer->num_bits);

    value &= (1LLU << bits) - 1;
    bit_writer->scratch |= (uint64_t)(value) << bit_writer->scratch_bits;
    bit_writer->scratch_bits += bits;

    if (bit_writer->scratch_bits >= 32)
    {
        ASSERT(bit_writer->word_index < bit_writer->num_words);
        bit_writer->data[bit_writer->word_index] = (uint32_t)(bit_writer->scratch & 0xFFFFFFFF);
        bit_writer->scratch >>= 32;
        bit_writer->scratch_bits -= 32;
        bit_writer->word_index++;
    }

    bit_writer->bits_written += bits;
}

void BitWriter_WriteAlign(BitWriter *bit_writer)
{
    int remainder_bits = bit_writer->bits_written % 8;
    if (remainder_bits != 0)
    {
        uint32_t zero = 0;
        BitWriter_WriteBits(bit_writer, zero, 8 - remainder_bits);
        ASSERT(bit_writer->bits_written % 8 == 0);
    }
}

void BitWriter_WriteBytes(BitWriter *bit_writer, uint8_t *data, int bytes)
{
    ASSERT(BitWriter_GetAlignBits(bit_writer) == 0);
    ASSERT(bit_writer->bits_written + bytes * 8 <= bit_writer->num_bits);
    ASSERT((bit_writer->bits_written % 32) == 0 ||
              (bit_writer->bits_written % 32) == 8 ||
              (bit_writer->bits_written % 32) == 16 ||
              (bit_writer->bits_written % 32) == 24);

    int head_bytes = (4 - (bit_writer->bits_written % 32) / 8) % 4;
    if (head_bytes > bytes)
        head_bytes = bytes;
    for (int i = 0; i < head_bytes; i++)
        BitWriter_WriteBits(bit_writer, data[i], 8);
    if (head_bytes == bytes)
        return;

    ASSERT(BitWriter_GetAlignBits(bit_writer) == 0);

    int num_words = (bytes - head_bytes) / 4;
    if (num_words > 0)
    {
        ASSERT(bit_writer->bits_written % 32 == 0);
        memcpy(&bit_writer->data[bit_writer->word_index], data + head_bytes, num_words * 4);
        bit_writer->bits_written += num_words * 32;
        bit_writer->word_index += num_words;
        bit_writer->scratch = 0;
    }

    ASSERT(BitWriter_GetAlignBits(bit_writer) == 0);

    int tail_start = head_bytes + num_words * 4;
    int tail_bytes = bytes - tail_start;
    ASSERT(tail_bytes >= 0 && tail_bytes < 4);
    for (int i = 0; i < tail_bytes; i++)
        BitWriter_WriteBits(bit_writer, data[tail_start + i], 8);

    ASSERT(BitWriter_GetAlignBits(bit_writer) == 0);
    ASSERT(head_bytes + num_words * 4 + tail_bytes == bytes);
}

void BitWriter_FlushBits(BitWriter *bit_writer)
{
    if (bit_writer->scratch_bits != 0)
    {
        ASSERT(bit_writer->word_index < bit_writer->num_words);
        bit_writer->data[bit_writer->word_index] = (uint32_t)(bit_writer->scratch & 0xFFFFFFFF);
        bit_writer->scratch >>= 32;
        bit_writer->scratch_bits -= 32;
        bit_writer->word_index++;
    }
}

int BitWriter_GetAlignBits(BitWriter *bit_writer)
{
    return (8 - bit_writer->bits_written % 8) % 8;
}

int BitWriter_GetBitsWritten(BitWriter *bit_writer)
{
    return bit_writer->bits_written;
}

int BitWriter_BitsAvailable(BitWriter *bit_writer)
{
    return bit_writer->num_bits - bit_writer->bits_written;
}

int BitWriter_GetBytesWritten(BitWriter *bit_writer)
{
    return (bit_writer->bits_written + 7) / 8;
}

int BitWriter_GetTotalBytes(BitWriter *bit_writer)
{
    return bit_writer->num_words * 4;
}

BitReader BitReader_Create(void *data, int bytes)
{
    ASSERT(data);
    BitReader bit_reader;
    bit_reader.mode = BIT_READER;
    bit_reader.data = (uint32_t *)data;
    bit_reader.num_bytes = bytes;
    bit_reader.num_bits = bytes * 8;
    bit_reader.num_words = (bytes + 3) / 4;
    return bit_reader;
}

bool BitReader_WouldOverflow(BitReader *bit_reader, int bits)
{
    return bit_reader->bits_read + bits > bit_reader->num_bits;
}

uint32_t BitReader_ReadBits(BitReader *bit_reader, int bits)
{
    ASSERT(bits > 0);
    ASSERT(bits <= 32);
    ASSERT(bit_reader->bits_read + bits <= bit_reader->num_bits);

    bit_reader->bits_read += bits;

    ASSERT(bit_reader->scratch_bits >= 0 && bit_reader->scratch_bits <= 64);

    if (bit_reader->scratch_bits < bits)
    {
        ASSERT(bit_reader->word_index < bit_reader->num_words);
        bit_reader->scratch |= (uint64_t)(bit_reader->data[bit_reader->word_index]) << bit_reader->scratch_bits;
        bit_reader->scratch_bits += 32;
        bit_reader->word_index++;
    }

    ASSERT(bit_reader->scratch_bits >= bits);
    uint32_t output = (uint32_t)(bit_reader->scratch & ((1LLU << bits) - 1));
    bit_reader->scratch >>= bits;
    bit_reader->scratch_bits -= bits;

    return output;
}

bool BitReader_Align(BitReader *bit_reader)
{
    int remainder_bits = bit_reader->bits_read % 8;
    if (remainder_bits != 0)
    {
        uint32_t value = BitReader_ReadBits(bit_reader, 8 - remainder_bits);
        ASSERT(bit_reader->bits_read % 8 == 0);
        ASSERT(value == 0);
        if (value != 0)
            return false;
    }
    return true;
}

void BitReader_ReadBytes(BitReader *bit_reader, uint8_t *data, int bytes)
{
    ASSERT(BitReader_GetAlignBits(bit_reader) == 0);
    ASSERT(bit_reader->bits_read + bytes * 8 <= bit_reader->num_bits);
    ASSERT((bit_reader->bits_read % 32) == 0 ||
              (bit_reader->bits_read % 32) == 8 ||
              (bit_reader->bits_read % 32) == 16 ||
              (bit_reader->bits_read % 32) == 24);

    int head_bytes = (4 - (bit_reader->bits_read % 32) / 8) % 4;
    if (head_bytes > bytes)
        head_bytes = bytes;
    for (int i = 0; i < head_bytes; i++)
        data[i] = BitReader_ReadBits(bit_reader, 8);
    if (head_bytes == bytes)
        return;

    ASSERT(BitReader_GetAlignBits(bit_reader) == 0);

    int num_words = (bytes - head_bytes) / 4;
    if (num_words > 0)
    {
        ASSERT((bit_reader->bits_read % 32) == 0);
        memcpy(data + head_bytes, &bit_reader->data[bit_reader->word_index], num_words * 4);
        bit_reader->bits_read += num_words * 32;
        bit_reader->word_index += num_words;
        bit_reader->scratch_bits = 0;
    }

    ASSERT(BitReader_GetAlignBits(bit_reader) == 0);

    int tail_start = head_bytes + num_words * 4;
    int tail_bytes = bytes - tail_start;
    ASSERT(tail_bytes >= 0 && tail_bytes < 4);
    for (int i = 0; i < tail_bytes; i++)
        data[tail_start + i] = (uint8_t)BitReader_ReadBits(bit_reader, 8);

    ASSERT(BitReader_GetAlignBits(bit_reader) == 0);
    ASSERT(head_bytes + num_words * 4 + tail_bytes == bytes);
}

int BitReader_GetAlignBits(BitReader *bit_reader)
{
    return (8 - bit_reader->bits_read % 8) % 8;
}

int BitReader_BitsRead(BitReader *bit_reader)
{
    return bit_reader->bits_read;
}

int BitReader_GetBytesRead(BitReader *bit_reader)
{
    return bit_reader->word_index * 4;
}

int BitReader_GetBitsRemaining(BitReader *bit_reader)
{
    return bit_reader->num_bits - bit_reader->bits_read;
}

int BitReader_BytesRemaining(BitReader *bit_reader)
{
    return BitReader_GetBitsRemaining(bit_reader) / 8;
}

int BitReader_GetTotalBits(BitReader *bit_reader)
{
    return bit_reader->num_bits;
}

int BitReader_GetTotalBytes(BitReader *bit_reader)
{
    return bit_reader->num_bits / 8;
}
