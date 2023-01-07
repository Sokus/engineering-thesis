#include "bitstream.h"
#include "bitpacker.h"
#include "macros.h"

#include <stdio.h>

WriteStream WriteStream_Create(void *buffer, int bytes)
{
    WriteStream write_stream = { 0 };
    write_stream.mode = WRITE_STREAM;
    write_stream.bit_writer = BitWriter_Create(buffer, bytes);
    return write_stream;
}

WriteStream ReadStream_Create(void *buffer, int bytes)
{
    ReadStream read_stream = { 0 };
    read_stream.mode = READ_STREAM;
    read_stream.bit_reader = BitReader_Create(buffer, bytes);
    return read_stream;
}

MeasureStream MeasureStream_Create(void *buffer, int bytes)
{
    MeasureStream measure_stream = { 0 };
    measure_stream.mode = MEASURE_STREAM;
    measure_stream.total_bytes = bytes;
    return measure_stream;
}

bool BitStream_SerializeInteger(BitStream *stream, int32_t *value, int32_t min, int32_t max)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);
    ASSERT(min < max);

    int bits = BitsRequiredForRange(min, max);

    if(stream->mode == WRITE_STREAM)
    {
        ASSERT(*value >= min);
        ASSERT(*value <= max);
        uint32_t value_relative = *value - min;
        BitWriter_WriteBits(&stream->bit_writer, value_relative, bits);
        return true;
    }

    if(stream->mode == READ_STREAM)
    {
        if (BitReader_WouldOverflow(&stream->bit_reader, bits))
        {
            stream->error = 1;
            return false;
        }
        uint32_t value_relative = BitReader_ReadBits(&stream->bit_reader, bits);
        *value = (int32_t)value_relative + min;
        stream->bits_read += bits;
        return true;
    }

    if(stream->mode == MEASURE_STREAM)
    {
        ASSERT(*value >= min);
        ASSERT(*value <= max);
        stream->bits_written += bits;
        return true;
    }

    return false;
}

bool BitStream_SerializeBits(BitStream *stream, uint32_t *value, int bits)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);
    ASSERT(bits > 0);
    ASSERT(bits <= 32);

    if (stream->mode == WRITE_STREAM)
    {
        BitWriter_WriteBits(&stream->bit_writer, *value, bits);
        return true;
    }

    if (stream->mode == READ_STREAM)
    {
        if (BitReader_WouldOverflow(&stream->bit_reader, bits))
        {
            stream->error = 1;
            return false;
        }
        uint32_t read_value = BitReader_ReadBits(&stream->bit_reader, bits);
        *value = read_value;
        stream->bits_read += bits;
        return true;
    }

    if (stream->mode == MEASURE_STREAM)
    {
        stream->bits_written += bits;
        return true;
    }

    return false;
}

bool BitStream_SerializeBytes(BitStream *stream, uint8_t *data, int bytes)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);

    if (stream->mode == WRITE_STREAM)
    {
        ASSERT(data);
        ASSERT(bytes >= 0);
        if (!BitStream_SerializeAlign(stream))
            return false;
        BitWriter_WriteBytes(&stream->bit_writer, data, bytes);
        return true;
    }

    if (stream->mode == READ_STREAM)
    {
        if (!BitStream_SerializeAlign(stream))
            return false;
        if (BitReader_WouldOverflow(&stream->bit_reader, bytes * 8))
        {
            stream->error = 1;
            return false;
        }
        BitReader_ReadBytes(&stream->bit_reader, data, bytes);
        stream->bits_read += bytes * 8;
        return true;
    }

    if (stream->mode == MEASURE_STREAM)
    {
        BitStream_SerializeAlign(stream);
        stream->bits_written += bytes * 8;
        return true;
    }

    return false;
}

bool BitStream_SerializeAlign(BitStream *stream)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);

    if (stream->mode == WRITE_STREAM)
    {
        BitWriter_WriteAlign(&stream->bit_writer);
        return true;
    }

    if (stream->mode == READ_STREAM)
    {
        int align_bits = BitReader_GetAlignBits(&stream->bit_reader);
        if (BitReader_WouldOverflow(&stream->bit_reader, align_bits))
        {
            stream->error = 1;
            return false;
        }
        if (!BitReader_Align(&stream->bit_reader))
            return false;
        stream->bits_read += align_bits;
        return true;
    }

    if (stream->mode == MEASURE_STREAM)
    {
        int align_bits = BitStream_GetAlignBits(stream);
        stream->bits_written += align_bits;
        return true;
    }

    return false;
}

int BitStream_GetAlignBits(BitStream *stream)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);

    if (stream->mode == WRITE_STREAM)
        return BitWriter_GetAlignBits(&stream->bit_writer);

    if (stream->mode == READ_STREAM)
        return BitReader_GetAlignBits(&stream->bit_reader);

    if (stream->mode == MEASURE_STREAM)
        return 7;

    return 0;
}

bool BitStream_SerializeCheck(BitStream *stream, char *string)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);

    if (stream->mode == WRITE_STREAM)
    {
        BitStream_SerializeAlign(stream);
        uint32_t magic = HashString(string, 0);
        BitStream_SerializeBits(stream, &magic, 32);
        return true;
    }

    if (stream->mode == READ_STREAM)
    {
        BitStream_SerializeAlign(stream);
        uint32_t value = 0;
        BitStream_SerializeBits(stream, &value, 32);
        uint32_t magic = HashString(string, 0);
        if (value != magic)
        {
            printf("serialize check failed: '%s', "
                   "exptected %x, got %x\n",
                   string, magic, value);
        }
        return value == magic;
    }

    if (stream->mode == MEASURE_STREAM)
    {
        BitStream_SerializeAlign(stream);
        stream->bits_written += 32;
        return true;
    }

    return false;
}

void BitStream_Flush(BitStream *stream)
{
    ASSERT(stream->mode == WRITE_STREAM);
    if (stream->mode == WRITE_STREAM)
    {
        BitWriter_FlushBits(&stream->bit_writer);
    }
}

int BitStream_GetBitsProcessed(BitStream *stream)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);

    if (stream->mode == WRITE_STREAM)
        return BitWriter_GetBitsWritten(&stream->bit_writer);

    if (stream->mode == READ_STREAM)
        return stream->bits_read;

    if (stream->mode == MEASURE_STREAM)
        return stream->bits_written;

    return 0;
}

int BitStream_GetBytesProcessed(BitStream *stream)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);

    if (stream->mode == WRITE_STREAM)
        return BitWriter_GetBytesWritten(&stream->bit_writer);

    if (stream->mode == READ_STREAM)
        return (stream->bits_read + 7) / 8;

    if (stream->mode == MEASURE_STREAM)
        return (stream->bits_written + 7) / 8;

    return 0;
}

int BitStream_GetBitsRemaining(BitStream *stream)
{
    ASSERT(stream->mode != BIT_STREAM_INVALID);

    if (stream->mode == WRITE_STREAM)
    {
        int total_bits = BitStream_GetTotalBits(stream);
        int bits_processed = BitStream_GetBitsProcessed(stream);
        return total_bits - bits_processed;
    }

    if (stream->mode == READ_STREAM)
    {
        return BitReader_GetBitsRemaining(&stream->bit_reader);
    }

    if (stream->mode == MEASURE_STREAM)
    {
        int total_bits = stream->total_bytes * 8;
        int bits_processed = BitStream_GetBitsProcessed(stream);
        return total_bits - bits_processed;
    }

    return 0;
}

int BitStream_GetTotalBits(BitStream *stream)
{
    ASSERT(stream->mode == WRITE_STREAM || stream->mode == MEASURE_STREAM);

    if (stream->mode == WRITE_STREAM)
    {
        return BitWriter_GetTotalBytes(&stream->bit_writer) * 8;
    }

    if (stream->mode == MEASURE_STREAM)
    {
        return stream->total_bytes * 8;
    }

    return 0;
}

int BitStream_GetTotalBytes(BitStream *stream)
{
    ASSERT(stream->mode == WRITE_STREAM || stream->mode == MEASURE_STREAM);

    if (stream->mode == WRITE_STREAM)
    {
        return BitWriter_GetTotalBytes(&stream->bit_writer);
    }

    if (stream->mode == MEASURE_STREAM)
    {
        return stream->total_bytes;
    }

    return 0;
}

int BitStream_GetBytesRead(BitStream *stream)
{
    ASSERT(stream->mode == READ_STREAM);
    if (stream->mode == READ_STREAM)
    {
        return BitReader_GetBytesRead(&stream->bit_reader);
    }

    return 0;
}

int BitStream_GetError(BitStream *stream)
{
    return stream->error;
}

int BitsRequired(unsigned int value)
{
    int pow = 0;
    if(value > (1U << 31)) return 32;
    while(value > (1U << pow)) ++pow;
    return pow;
}

int BitsRequiredForRange(unsigned int min, unsigned int max)
{
    return (min == max) ? 0 : BitsRequired(max - min);
}

uint32_t HashString(char *string, uint32_t hash)
{
    ASSERT(string);
    while (*string != '\0')
    {
        char c = *string++;
        if ((c >= 'a') && (c <= 'z'))
            c = (c - 'a') + 'A';
        hash += c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    return hash;
}