#include "protocol.h"
//#include "game/pe_game.h"
#include "serialization/serialize.h"
#include "serialization/bitstream.h"
#include "system/network.h"
#include "macros.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


int GetPacketType(void *packet)
{
    ASSERT(packet);
    return *(int32_t *)packet;
}

void SetPacketType(void *packet, PacketType type)
{
    ASSERT(packet);
    ASSERT(type >= 0);
    ASSERT(type < PACKET_TYPE_COUNT);
    *(int32_t *)packet = (int32_t)type;
}

void *CreatePacket(PacketType type)
{
    ASSERT(type >= 0 && type < PACKET_TYPE_COUNT);

    size_t packet_size = 0;
    switch (type)
    {
    case PACKET_CONNECTION_REQUEST:
        packet_size = sizeof(ConnectionRequestPacket);
        break;
    case PACKET_CONNECTION_ACCEPTED:
        packet_size = sizeof(ConnectionAcceptedPacket);
        break;
    case PACKET_CONNECTION_DENIED:
        packet_size = sizeof(ConnectionDeniedPacket);
        break;
    case PACKET_CONNECTION_KEEP_ALIVE:
        packet_size = sizeof(ConnectionKeepAlivePacket);
        break;
    case PACKET_CONNECTION_DISCONNECT:
        packet_size = sizeof(ConnectionDisconnectPacket);
        break;
    // case PACKET_WORLD_STATE:
    //     packet_size = sizeof(WorldStatePacket);
    //     break;
    default:
        BREAK();
        return NULL;
    }

    void *packet = malloc(packet_size);
    if (!packet)
    {
        BREAK();
        return NULL;
    }

    memset(packet, 0, packet_size);
    SetPacketType(packet, type);

    return packet;
}

void DestroyPacket(void *packet)
{
    free(packet);
}

bool SerializePacket(BitStream *stream, void *packet)
{
    ASSERT(stream);
    ASSERT(packet);
    int packet_type = GetPacketType(packet);
    ASSERT(packet_type >= 0);
    ASSERT(packet_type < PACKET_TYPE_COUNT);

    switch (packet_type)
    {
    case PACKET_CONNECTION_REQUEST:
        return SerializeConnectionRequestPacket(stream, (ConnectionRequestPacket *)packet);
    case PACKET_CONNECTION_ACCEPTED:
        return SerializeConnectionAcceptedPacket(stream, (ConnectionAcceptedPacket *)packet);
    case PACKET_CONNECTION_DENIED:
        return SerializeConnectionDeniedPacket(stream, (ConnectionDeniedPacket *)packet);
    case PACKET_CONNECTION_KEEP_ALIVE:
        return SerializeConnectionKeepAlivePacket(stream, (ConnectionKeepAlivePacket *)packet);
    case PACKET_CONNECTION_DISCONNECT:
        return SerializeConnectionDisconnectPacket(stream, (ConnectionDisconnectPacket *)packet);
    // case PACKET_WORLD_STATE:
    //     return SerializeWorldStatePacket(stream, (WorldStatePacket *)packet);
    default:
        BREAK();
        return false;
    }
}

void SendPacket(Socket socket, Address address, void *packet)
{
    ASSERT(packet);
    ASSERT(AddressIsValid(address));

    uint8_t packet_data[MAX_PACKET_SIZE];

    PacketInfo info = {0};
    info.protocol_id = PROTOCOL_ID;

    int packet_size = WritePacket(info, packet, packet_data, MAX_PACKET_SIZE);

    if (packet_size > 0)
    {
        SocketSend(socket, address, packet_data, packet_size);
    }
}

void *ReceivePacket(Socket socket, Address *address)
{
    uint8_t packet_data[MAX_PACKET_SIZE];
    int packet_bytes = SocketReceive(socket, address, packet_data, MAX_PACKET_SIZE);
    if (packet_bytes == 0)
        return NULL;

    PacketInfo info = {0};
    info.protocol_id = PROTOCOL_ID;

    void *packet = ReadPacket(info, packet_data, packet_bytes);
    return packet;
}

int WritePacket(PacketInfo info, void *packet, uint8_t *buffer, int buffer_size)
{
    ASSERT(packet);
    ASSERT(buffer);
    ASSERT(buffer_size > 0);
    ASSERT(info.protocol_id);
    int32_t packet_type = GetPacketType(packet);
    ASSERT(packet_type >= 0 && packet_type < PACKET_TYPE_COUNT);

    WriteStream stream = WriteStream_Create(buffer, buffer_size);

    uint32_t crc32 = 0;
    BitStream_SerializeBits(&stream, &crc32, 32);
    BitStream_SerializeInteger(&stream, &packet_type, 0, PACKET_TYPE_COUNT - 1);

    if (!SerializePacket(&stream, packet))
        return 0;

    BitStream_SerializeCheck(&stream, "end of packet");
    BitStream_Flush(&stream);

    *(uint32_t *)buffer = info.protocol_id;

    if (BitStream_GetError(&stream))
        return 0;

    return BitStream_GetBytesProcessed(&stream);
}

void *ReadPacket(PacketInfo info, uint8_t *buffer, int buffer_size)
{
    ASSERT(buffer);
    ASSERT(buffer_size > 0);
    ASSERT(info.protocol_id != 0);

    ReadStream stream = ReadStream_Create(buffer, buffer_size);

    uint32_t read_crc32 = 0;
    BitStream_SerializeBits(&stream, &read_crc32, 32);
    uint32_t crc32 = info.protocol_id;

    if (crc32 != read_crc32)
    {
        printf("corrupt packet. expected crc32 %x, got %x\n", crc32, read_crc32);
        return NULL;
    }

    int32_t packet_type = 0;
    if (!BitStream_SerializeInteger(&stream, &packet_type, 0, PACKET_TYPE_COUNT - 1))
    {
        return NULL;
    }

    void *packet = CreatePacket(packet_type);
    if (!packet)
    {
        return NULL;
    }

    if (!SerializePacket(&stream, packet))
    {
        goto cleanup;
    }

    if (!BitStream_SerializeCheck(&stream, "end of packet"))
    {
        goto cleanup;
    }

    return packet;

cleanup:
    DestroyPacket(packet);
    return NULL;
}

bool SerializeConnectionRequestPacket(BitStream *stream, ConnectionRequestPacket *packet)
{
    if (stream->mode == READ_STREAM && BitStream_GetBitsRemaining(stream) < 256 * 8)
        return false;
    SERIALIZE_BYTES(stream, packet->data, sizeof(packet->data));
    return true;
}

bool SerializeConnectionAcceptedPacket(BitStream *stream, ConnectionAcceptedPacket *packet)
{
    SERIALIZE_INT(stream, packet->client_index, 0, MAX_CLIENTS - 1);
    return true;
}

bool SerializeConnectionDeniedPacket(BitStream *stream, ConnectionDeniedPacket *packet)
{
    SERIALIZE_INT(stream, packet->reason, 0, CONNECTION_DENIED_REASON_COUNT - 1);
    return true;
}

bool SerializeConnectionKeepAlivePacket(BitStream *stream, ConnectionKeepAlivePacket *packet)
{
    return true;
}

bool SerializeConnectionDisconnectPacket(BitStream *stream, ConnectionDisconnectPacket *packet)
{
    return true;
}