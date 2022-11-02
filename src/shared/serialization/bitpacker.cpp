#include <stdint.h>

#include "bitpacker.h"
#include "utility.h"
#include "log.h"
#include "macros.h"

// BIT_WRITER
BitPacker BitWriter(void *buffer, int bytes)
{
    ASSERT(bytes % 4 == 0);
    BitPacker result = {};
    result.mode = BIT_WRITER;
    result.buffer = (uint32_t *)buffer;
    result.word_capacity = bytes / 4;
    return result;
}

int BitsWritten(BitPacker *bit_writer)
{
    return (bit_writer->word_count * 32) + bit_writer->scratch_bits;
}

int BytesWritten(BitPacker *bit_writer)
{
    int bits = BitsWritten(bit_writer);
    int bytes = bits / 8;
    return (bits % 8 == 0) ? bytes : (bytes + 1);
}

void WriteBits(BitPacker *bit_writer, uint32_t *value, int bits)
{
    ASSERT(bits >= 0);
    ASSERT(bits <= 32);
    ASSERT(bit_writer->mode == BIT_WRITER);

    uint64_t value_masked = (uint64_t)(*value) & (((uint64_t)1 << bits) - 1);
    bit_writer->scratch |= value_masked << bit_writer->scratch_bits;
    bit_writer->scratch_bits += bits;

    if(bit_writer->scratch_bits >= 32)
    {
        uint32_t *current_word_pointer = bit_writer->buffer + bit_writer->word_count;
        *current_word_pointer = (bit_writer->scratch & 0xFFFFFFFF);
        bit_writer->word_count++;
        bit_writer->scratch >>= 32;
        bit_writer->scratch_bits -= 32;
    }
}

void Flush(BitPacker *bit_writer)
{
    if(bit_writer->scratch_bits != 0)
    {
        uint32_t *current_word_pointer = bit_writer->buffer + bit_writer->word_count;
        *current_word_pointer = (bit_writer->scratch & 0xFFFFFFFF);
        bit_writer->word_count++;
        bit_writer->scratch >>= 32;
        bit_writer->scratch_bits -= 32;
    }
}

// BIT_READER
BitPacker BitReader(void *buffer, int bytes)
{
    ASSERT(bytes % 4 == 0);
    BitPacker result = {};
    result.mode = BIT_READER;
    result.buffer = (uint32_t *)buffer;
    result.word_count = bytes / 4;
    return result;
}

int BitsRead(BitPacker *bit_reader)
{
    return bit_reader->bits_read;
}

int BytesRead(BitPacker *bit_reader)
{
    int bytes = bit_reader->bits_read / 8;
    return (bit_reader->bits_read % 8 == 0) ? bytes : (bytes + 1);
}

void ReadBits(BitPacker *bit_reader, uint32_t *value, int bits)
{
    ASSERT(bits >= 0);
    ASSERT(bits <= 32);
    ASSERT(bit_reader->mode == BIT_READER);
    ASSERT(bit_reader->bits_read + bits <= bit_reader->word_count * 32);

    if(bit_reader->scratch_bits < bits)
    {
        int next_word_index = (bit_reader->bits_read + bit_reader->scratch_bits) / 32;
        uint64_t next_word = (uint64_t)(*(bit_reader->buffer + next_word_index));
        bit_reader->scratch |= (next_word << bit_reader->scratch_bits);
        bit_reader->scratch_bits += 32;
    }
    ASSERT(bit_reader->scratch_bits >= bits);

    bit_reader->bits_read += bits;

    *value = (uint32_t)(bit_reader->scratch & ((uint64_t)1 << bits) - 1);

    bit_reader->scratch >>= bits;
    bit_reader->scratch_bits -= bits;
}

void Rewind(BitPacker *bit_reader, int position)
{
    ASSERT(position <= bit_reader->word_count * 32);

    bit_reader->bits_read = position;

    if(bit_reader->bits_read < bit_reader->word_count * 32)
    {
        bit_reader->scratch_bits = 32 - (bit_reader->bits_read % 32);
        int word_index = bit_reader->bits_read / 32;
        uint64_t word_value = (uint64_t)(*(bit_reader->buffer + word_index));
        bit_reader->scratch = word_value >> (bit_reader->bits_read % 32);
    }
    else
    {
        bit_reader->scratch_bits = 0;
        bit_reader->scratch = 0;
    }
}

// General
bool WouldOverflow(BitPacker *bit_packer, int bits)
{
    if(bit_packer->mode == BIT_WRITER)
    {
        int new_bits = BitsWritten(bit_packer) + bits;
        return new_bits > (bit_packer->word_capacity * 32);
    }
    else if(bit_packer->mode == BIT_READER)
    {
        return (bit_packer->bits_read + bits) > (bit_packer->word_count * 32);
    }
    return false;
}

bool Align(BitPacker *bit_packer)
{
    if(bit_packer->mode == BIT_WRITER)
    {
        int remainder_bits = bit_packer->scratch_bits % 8;
        if(remainder_bits != 0)
        {
            uint32_t value = 0;
            WriteBits(bit_packer, &value, 8 - remainder_bits);
            ASSERT((bit_packer->scratch_bits % 8) == 0);
        }
    }
    else if(bit_packer->mode == BIT_READER)
    {
        int remainder_bits = bit_packer->bits_read % 8;
        if(remainder_bits != 0)
        {
            uint32_t value;
            ReadBits(bit_packer, &value, 8 - remainder_bits);
            ASSERT(bit_packer->bits_read % 8 == 0);
            if(value != 0)
            {
                Log(LOG_DEBUG, "Packet alignment: aligning %d bits, expected 0, got 0x%x", 8 - remainder_bits, value);
                return BytesRead(bit_packer) == (bit_packer->word_count * 4);
            }
        }
    }
    return true;
}

void Reset(BitPacker *bit_packer)
{
    if(bit_packer->mode == BIT_WRITER)
    {
        bit_packer->word_count = 0;
        bit_packer->scratch = 0;
        bit_packer->scratch_bits = 0;
    }
    else if(bit_packer->mode == BIT_READER)
    {
        Rewind(bit_packer, 0);
    }
}