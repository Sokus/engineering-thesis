#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdbool.h>
#include <stdint.h>

struct BitStream;

#define SERIALIZE_INT(stream_ptr, value, min, max) \
    do { \
        ASSERT((int32_t)(min) < (int32_t)(max)); \
        int32_t int32_value; \
        if ((stream_ptr)->mode == WRITE_STREAM) \
        { \
            ASSERT((int32_t)(value) >= (int32_t)(min)); \
            ASSERT((int32_t)(value) <= (int32_t)(max)); \
            int32_value = (value); \
        } \
        if (!BitStream_SerializeInteger((stream_ptr), &int32_value, (min), (max))) \
            return false; \
        if ((stream_ptr)->mode == READ_STREAM) \
        { \
            (value) = int32_value; \
            if ((value) < (min) || (value) > (max)) \
                return false; \
        } \
    } while (0)

#define SERIALIZE_BITS(stream_ptr, value, bits) \
    do { \
        uint32_t uint32_value; \
        if ((stream_ptr)->mode == WRITE_STREAM) \
            uint32_value = (uint32_t)value; \
        if (!BitStream_SerializeBits((stream_ptr), &uint32_value, bits)) \
            return false; \
        if ((stream_ptr)->mode == READ_STREAM) \
            value = uint32_value; \
    } while (0)

#define SERIALIZE_BOOL(stream_ptr, value) \
    do { \
        uint32_t uint32_bool_value; \
        if ((stream_ptr)->mode == WRITE_STREAM) \
            uint32_bool_value = (value) ? 1 : 0; \
        if (!BitStream_SerializeBits((stream_ptr), &uint32_bool_value, 1)) \
            return false; \
        if ((stream_ptr)->mode == READ_STREAM) \
            value = (uint32_bool_value != 0) ? true : false; \
    } while (0)

#define SERIALIZE_ENUM(stream_ptr, value, type, num_entries) \
    do { \
        int32_t int32_value; \
        if ((stream_ptr)->mode == WRITE_STREAM) \
            int32_value = (int32_t)value; \
        if (!BitStream_SerializeInteger((stream_ptr), &int32_value, 0, (num_entries) - 1)) \
            return false; \
        if ((stream_ptr)->mode == READ_STREAM) \
            value = (type)int32_value; \
    } while (0)

bool SerializeFloat(struct BitStream *stream, float *value);

#define SERIALIZE_FLOAT(stream_ptr, value) \
    do { \
        if (!SerializeFloat((stream_ptr), &value)) \
            return false; \
    } while (0)

#define SERIALIZE_BYTES(stream_ptr, data_ptr, bytes) \
    do { \
        if (!BitStream_SerializeBytes((stream_ptr), (data_ptr), (bytes))) \
            return false; \
    } while (0)

bool SerializeString(struct BitStream *stream, char *string, int buffer_size);

#define SERIALIZE_STRING(stream_ptr, string, buffer_size) \
    do { \
        if (!SerializeString((stream_ptr), (string), (buffer_size))) \
            return false; \
    } while (0)

#define SERIALIZE_ALIGN(stream_ptr) \
    do { \
        if (!BitStream_SerializeAlign((stream_ptr))) \
            return false; \
    } while (0)

#define SERIALIZE_CHECK(stream_ptr, string) \
    do { \
        if (!BitStream_SerializeCheck((stream_ptr), (string))) \
            return false; \
    } while (0)

#endif // SERIALIZE_H