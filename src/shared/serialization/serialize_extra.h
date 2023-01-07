#ifndef PE_SERIALIZE_EXTRA_H
#define PE_SERIALIZE_EXTRA_H

#include <stdbool.h>

struct BitStream;

struct Vector2;
bool SerializeVector2(struct BitStream *stream, struct Vector2 *vector2_ptr);

#define SERIALIZE_VECTOR2(stream_ptr, value) \
    do { \
        if (!SerializeVector2((stream_ptr), &value)) \
            return false; \
    } while (0)

struct Vector3;
bool SerializeVector3(struct BitStream *stream, struct Vector3 *vector3_ptr);

#define SERIALIZE_VECTOR3(stream_ptr, value) \
    do { \
        if (!SerializeVector3((stream_ptr), &value)) \
            return false; \
    } while (0)

#endif // PE_SERIALIZE_EXTRA_H