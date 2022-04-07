/* date = April 6th 2022 0:46 am */

#ifndef PI_NETWORKING_H
#define PI_NETWORKING_H

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

struct PacketHeader
{
    uint16_t sequence;
    uint8_t reliable;
    uint16_t ack_sequence;
    uint8_t ack_reliable;
};

struct NetSender
{
    uint8_t reliable_buffer[1024];
    uint8_t standard_buffer[1024];
    
};

// platform agnostic
AddressComponents GetAddressComponents(Address address);
Address AddressFromInt(uint32_t ip, uint16_t port);
Address AddressFromComponents(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port);
bool AddressCompare(Address a, Address b);
bool SequenceIsNewer(uint16_t seq_new, uint16_t seq_old);

// platform specific
bool SocketsInit();
void SocketsShutdown();
bool SocketCreate(int *socket_handle);
bool SocketSetShouldBlock(int socket_handle, bool should_block);
bool SocketBind(int socket_handle, uint16_t port);
bool SocketClose(int socket_handle);
bool SocketSend(int socket_handle, Address *destination, void *data, int size);
int SocketRecieve(int socket_handle, Address *sender, void *data, int size);
#endif //PI_NETWORKING_H
