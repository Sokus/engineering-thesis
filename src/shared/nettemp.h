#ifndef NETTEMP_H
#define NETTEMP_H

#include "serialization/serialization.h"

enum ClientId
{
    CLIENT_INVALID = -1,
    CLIENT_COUNT   = 1
};

enum ConnectionState
{
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
};

enum MessageType
{
    INVALID,
    JOIN_ATTEMPT,
    JOIN_APPROVE,
    JOIN_REFUSE,
    DISCONNECT,
    VALUE,

    MESSAGE_TYPE_MAX,
};

struct MessageValue
{
    int32_t value;
};


bool SerializeMessageValue(BitPacker *bit_packer, MessageValue *value)
{
    SERIALIZE_INT32(bit_packer, &value->value);
}

#endif // NETTEMP_H