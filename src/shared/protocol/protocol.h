#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "serialization/bitstream.h"
#include "serialization/serialize.h"
#include "macros.h"
#include "game/input.h"
#include "game/entity.h"
#include "game/world.h"

#define MAX_PACKET_SIZE 1200
#define MAX_CLIENTS 16
#define PROTOCOL_ID 354013947U

#define CONNECTION_REQUEST_SEND_RATE 0.1
#define CONNECTION_REQUEST_TIME_OUT 0.5
#define CONNECTION_KEEP_ALIVE_SEND_RATE 1.0
#define CONNECTION_KEEP_ALIVE_TIME_OUT 10.0
#define CONNECTION_CONFIRM_SEND_RATE 0.1

enum PacketType
{
    PACKET_CONNECTION_REQUEST,
    PACKET_CONNECTION_ACCEPTED,
    PACKET_CONNECTION_DENIED,
    PACKET_CONNECTION_KEEP_ALIVE,
    PACKET_CONNECTION_DISCONNECT,
    PACKET_INPUT,
    PACKET_WORLD_STATE,
    PACKET_TYPE_COUNT,
};

class Packet
{
public:
    int type;

    Packet(int type) : type(type) {}

    virtual ~Packet() {}
    virtual bool Serialize(BitStream *stream) = 0;

private:
    Packet(const Packet &other);
};

Packet *CreatePacket(int type);
void DestroyPacket(Packet *packet);

struct Socket;
struct Address;
void SendPacket(struct Socket socket, struct Address address, Packet *packet);
Packet *ReceivePacket(struct Socket socket, struct Address *address);

struct PacketInfo
{
    uint32_t protocol_id;
};

int WritePacket(PacketInfo info, Packet *packet, uint8_t *buffer, int buffer_size);
Packet *ReadPacket(PacketInfo info, uint8_t *buffer, int buffer_size);

struct ConnectionRequestPacket : public Packet
{
    uint8_t data[256];

    ConnectionRequestPacket() : Packet(PACKET_CONNECTION_REQUEST)
    {
        memset(data, 0, sizeof(data));
    }

    bool Serialize(BitStream *stream)
    {
        if (stream->mode == READ_STREAM && BitStream_GetBitsRemaining(stream) < 256 * 8)
            return false;
        SERIALIZE_BYTES(stream, data, sizeof(data));
        return true;
    }
};

struct ConnectionAcceptedPacket : public Packet
{
    int client_index;

    ConnectionAcceptedPacket() : Packet(PACKET_CONNECTION_ACCEPTED)
    {
        client_index = 0;
    }

    bool Serialize(BitStream *stream)
    {
        SERIALIZE_INT(stream, client_index, 0, MAX_CLIENTS - 1);
        return true;
    }
};

enum ConnectionDeniedReason
{
    CONNECTION_DENIED_SERVER_FULL,
    CONNECTION_DENIED_ALREADY_CONNECTED,
    CONNECTION_DENIED_REASON_COUNT,
};

struct ConnectionDeniedPacket : public Packet
{
    ConnectionDeniedReason reason;

    ConnectionDeniedPacket() : Packet(PACKET_CONNECTION_DENIED)
    {
        reason = CONNECTION_DENIED_SERVER_FULL;
    }

    bool Serialize(BitStream *stream)
    {
        SERIALIZE_ENUM(stream, reason, ConnectionDeniedReason, CONNECTION_DENIED_REASON_COUNT - 1);
        return true;
    }
};

struct ConnectionKeepAlivePacket : public Packet
{
    ConnectionKeepAlivePacket() : Packet(PACKET_CONNECTION_KEEP_ALIVE) {}

    bool Serialize(BitStream *stream)
    {
        return true;
    }
};

struct ConnectionDisconnectPacket : public Packet
{
    ConnectionDisconnectPacket() : Packet(PACKET_CONNECTION_DISCONNECT) {}

    bool Serialize(BitStream *stream)
    {
        return true;
    }
};

struct InputPacket : public Packet
{
    Game::Input input;

    InputPacket() : Packet(PACKET_INPUT)
    {
        input = Game::Input{};
    }

    bool Serialize(BitStream *stream)
    {
        return input.Serialize(stream);
    }
};

struct WorldStatePacket : public Packet
{
    int start_index;
    int entity_count;
    Game::Entity entities[Game::max_entity_count];

    WorldStatePacket() : Packet(PACKET_WORLD_STATE)
    {
        start_index = 0;
        entity_count = 0;
        memset(entities, 0, sizeof(entities));
    }

    bool Serialize(BitStream *stream)
    {
        SERIALIZE_INT(stream, start_index, 0, Game::max_entity_count);
        SERIALIZE_INT(stream, entity_count, 0, ARRAY_SIZE(entities));
        for (int i = 0; i < entity_count; i++)
        {
            if (!entities[i].Serialize(stream))
                return false;
        }
        return true;
    }
};

#endif // PROTOCOL_H