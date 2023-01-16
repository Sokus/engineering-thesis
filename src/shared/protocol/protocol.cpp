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

Packet *CreatePacket(int type)
{
    switch (type)
    {
        case PACKET_CONNECTION_REQUEST: return new ConnectionRequestPacket();
        case PACKET_CONNECTION_ACCEPTED: return new ConnectionAcceptedPacket();
        case PACKET_CONNECTION_DENIED: return new ConnectionDeniedPacket();
        case PACKET_CONNECTION_KEEP_ALIVE: return new ConnectionKeepAlivePacket();
        case PACKET_CONNECTION_DISCONNECT: return new ConnectionDisconnectPacket();
        case PACKET_INPUT: return new InputPacket();
        case PACKET_WORLD_STATE: return new WorldStatePacket();
        default: return nullptr;
    }
}

void DestroyPacket(Packet *packet)
{
    delete packet;
}

void SendPacket(Socket socket, Address address, Packet *packet)
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

Packet *ReceivePacket(Socket socket, Address *address)
{
    uint8_t packet_data[MAX_PACKET_SIZE];
    int packet_bytes = SocketReceive(socket, address, packet_data, MAX_PACKET_SIZE);
    if (packet_bytes == 0)
        return NULL;

    PacketInfo info = {0};
    info.protocol_id = PROTOCOL_ID;

    Packet *packet = ReadPacket(info, packet_data, packet_bytes);
    return packet;
}

int WritePacket(PacketInfo info, Packet *packet, uint8_t *buffer, int buffer_size)
{
    ASSERT(packet);
    ASSERT(buffer);
    ASSERT(buffer_size > 0);
    ASSERT(info.protocol_id);
    ASSERT(packet->type >= 0 && packet->type < PACKET_TYPE_COUNT);

    WriteStream stream = WriteStream_Create(buffer, buffer_size);

    uint32_t crc32 = 0;
    BitStream_SerializeBits(&stream, &crc32, 32);
    BitStream_SerializeInteger(&stream, &packet->type, 0, PACKET_TYPE_COUNT - 1);

    if (!packet->Serialize(&stream))
        return 0;

    BitStream_SerializeCheck(&stream, "end of packet");
    BitStream_Flush(&stream);

    *(uint32_t *)buffer = info.protocol_id;

    if (BitStream_GetError(&stream))
        return 0;

    return BitStream_GetBytesProcessed(&stream);
}

Packet *ReadPacket(PacketInfo info, uint8_t *buffer, int buffer_size)
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
        return nullptr;
    }

    int32_t packet_type = 0;
    if (!BitStream_SerializeInteger(&stream, &packet_type, 0, PACKET_TYPE_COUNT - 1))
    {
        return nullptr;
    }

    Packet *packet = CreatePacket(packet_type);
    if (!packet)
    {
        return nullptr;
    }

    if (!packet->Serialize(&stream))
        goto cleanup;

    if (!BitStream_SerializeCheck(&stream, "end of packet"))
        goto cleanup;

    return packet;

cleanup:
    DestroyPacket(packet);
    return nullptr;
}