/* date = April 6th 2022 0:46 am */

#ifndef PI_NETWORKING_H
#define PI_NETWORKING_H

enum PacketType
{
    PacketType_Invalid,
    PacketType_Connect,
    PacketType_Accept,
    PacketType_Disconnect,
    PacketType_Snapshot
};

struct PacketHeader
{
    uint16_t sequence;
    uint8_t reliable;
    uint16_t ack_sequence;
    uint8_t ack_reliable;
};

struct Address
{
    uint32_t ip;
    uint16_t port;
};

struct AddressComponents
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint16_t port;
};

AddressComponents GetAddressComponents(Address address)
{
    AddressComponents result;
    uint32_t ip = address.ip;
    result.a = (uint8_t)(ip >> 24);
    result.b = (ip >> 16 & 0xFF);
    result.c = (ip >> 8  & 0xFF);
    result.d = (ip       & 0xFF);
    result.port = address.port;
    return result;
}

Address AddressFromInt(uint32_t ip, uint16_t port)
{
    Address result;
    result.ip = ip;
    result.port = port;
    return result;
}

Address AddressFromComponents(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port)
{
    Address result;
    result.ip = ((a << 24) | (b << 16) | (c << 8) | (d));
    result.port = port;
    return result;
}

bool AddressCompare(Address a, Address b)
{
    bool result = (a.ip == b.ip && a.port == b.port);
    return result;
}

bool SequenceIsNewer(uint16_t seq_new, uint16_t seq_old)
{
    // max of uint16_t is 65535, if the difference between sequences
    // is greater than half of it (32767.5) we have probably wrapped around
    bool normal = ((seq_new - seq_old <= 32767) && (seq_new > seq_old));
    bool wrapped = ((seq_old - seq_new > 32767) && (seq_new < seq_old));
    bool result = (normal || wrapped);
    return result;
}

bool SocketsInit();
void SocketsShutdown();
bool SocketCreate(int *socket_handle);
bool SocketSetShouldBlock(int socket_handle, bool should_block);
bool SocketBind(int socket_handle, uint16_t port);
bool SocketClose(int socket_handle);
bool SocketSend(int socket_handle, Address *destination, void *data, int size);
int SocketRecieve(int socket_handle, Address *sender, void *data, int size);
#endif //PI_NETWORKING_H
