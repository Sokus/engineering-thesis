#include "serialize.h"
#include "bitstream.h"
#include "macros.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

bool SerializeFloat(BitStream *stream, float *value)
{
    union FloatUint
    {
        float float_value;
        uint32_t uint32_value;
    } tmp = {0.0f};

    if (stream->mode == WRITE_STREAM)
        tmp.float_value = *value;
    bool result = BitStream_SerializeBits(stream, &tmp.uint32_value, 32);
    if (stream->mode == READ_STREAM)
        *value = tmp.float_value;

    return result;
}

bool SerializeString(BitStream *stream, char *string, int buffer_size)
{
    int length;
    if (stream->mode == WRITE_STREAM)
    {
        length = (int)strlen(string);
        ASSERT(length < buffer_size);
    }
    SERIALIZE_INT(stream, length, 0, buffer_size - 1);
    SERIALIZE_BYTES(stream, (uint8_t *)string, length);
    if (stream->mode == READ_STREAM)
        string[length] = '\0';
    return true;
}
