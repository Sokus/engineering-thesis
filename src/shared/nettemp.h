#ifndef NETTEMP_H
#define NETTEMP_H

enum ClientId
{
    CLIENT_INVALID = -1,
    CLIENT_COUNT   = 4
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
    CONNECT,
    APPROVE,
    REFUSE,
    DISCONNECT,
    VALUE,
};

struct MessageValue
{
    uint32_t value;
};

#endif // NETTEMP_H