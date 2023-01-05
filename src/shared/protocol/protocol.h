#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_PACKET_SIZE 1200
#define MAX_CLIENTS 16
#define PROTOCOL_ID 354013947U

#define CONNECTION_REQUEST_SEND_RATE 0.1
#define CONNECTION_REQUEST_TIME_OUT 0.5
#define CONNECTION_KEEP_ALIVE_SEND_RATE 1.0
#define CONNECTION_KEEP_ALIVE_TIME_OUT 10.0
#define CONNECTION_CONFIRM_SEND_RATE 0.1

typedef enum PacketType
{
    PACKET_CONNECTION_REQUEST,
    PACKET_CONNECTION_ACCEPTED,
    PACKET_CONNECTION_DENIED,
    PACKET_CONNECTION_KEEP_ALIVE,
    PACKET_CONNECTION_DISCONNECT,
    PACKET_WORLD_STATE,
    PACKET_TYPE_COUNT,
} PacketType;

int GetPacketType(void *packet);
void SetPacketType(void *packet, PacketType type);
void *CreatePacket(PacketType type);
void DestroyPacket(void *packet);
struct BitStream;
bool SerializePacket(struct BitStream *stream, void *packet);

struct Socket;
struct Address;
void SendPacket(struct Socket socket, struct Address address, void *packet);
void *ReceivePacket(struct Socket socket, struct Address *address);

typedef struct PacketInfo
{
    uint32_t protocol_id;
} PacketInfo;

int WritePacket(PacketInfo info, void *packet, uint8_t *buffer, int buffer_size);
void *ReadPacket(PacketInfo info, uint8_t *buffer, int buffer_size);

typedef struct ConnectionRequestPacket
{
    int32_t type;
    uint8_t data[256];
} ConnectionRequestPacket;

bool SerializeConnectionRequestPacket(struct BitStream *stream, ConnectionRequestPacket *packet);

typedef struct ConnectionAcceptedPacket
{
    int32_t type;
    int client_index;
} ConnectionAcceptedPacket;

bool SerializeConnectionAcceptedPacket(struct BitStream *stream, ConnectionAcceptedPacket *packet);

typedef enum ConnectionDeniedReason
{
    CONNECTION_DENIED_SERVER_FULL,
    CONNECTION_DENIED_ALREADY_CONNECTED,
    CONNECTION_DENIED_REASON_COUNT,
} ConnectionDeniedReason;

typedef struct ConnectionDeniedPacket
{
    int32_t type;
    ConnectionDeniedReason reason;
} ConnectionDeniedPacket;

bool SerializeConnectionDeniedPacket(struct BitStream *stream, ConnectionDeniedPacket *packet);

typedef struct ConnectionKeepAlivePacket
{
    int32_t type;
} ConnectionKeepAlivePacket;

bool SerializeConnectionKeepAlivePacket(struct BitStream *stream, ConnectionKeepAlivePacket *packet);

typedef struct ConnectionDisconnectPacket
{
    int32_t type;
} ConnectionDisconnectPacket;

bool SerializeConnectionDisconnectPacket(struct BitStream *stream, ConnectionDisconnectPacket *packet);

#endif // PROTOCOL_H