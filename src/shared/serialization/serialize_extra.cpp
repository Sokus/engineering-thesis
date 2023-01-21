#include "serialize_extra.h"

#include "bitstream.h"
#include "serialize.h"

//#include "protocol/pe_protocol.h"
#include "macros.h"

#include "raylib.h"

#include <stdbool.h>

bool SerializeVector2(BitStream *stream, Vector2 *value)
{
    ASSERT(stream);
    ASSERT(value);
    SERIALIZE_FLOAT(stream, value->x);
    SERIALIZE_FLOAT(stream, value->y);
    return true;
}

bool SerializeVector3(BitStream *stream, Vector3 *value)
{
    ASSERT(stream);
    ASSERT(value);
    SERIALIZE_FLOAT(stream, value->x);
    SERIALIZE_FLOAT(stream, value->y);
    SERIALIZE_FLOAT(stream, value->z);
    return true;
}

bool SerializeRectangle(BitStream *stream, Rectangle *value)
{
    ASSERT(stream);
    ASSERT(value);
    SERIALIZE_FLOAT(stream, value->x);
    SERIALIZE_FLOAT(stream, value->y);
    SERIALIZE_FLOAT(stream, value->width);
    SERIALIZE_FLOAT(stream, value->height);
    return true;
}