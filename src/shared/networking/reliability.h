#ifndef RELIABILITY_H
#define RELIABILITY_H

#include "networking.h"

#include "data_structures.h"

#include <stdint.h>

namespace Net {

#define PACKET_BUFFER_SIZE 1024
#define MAX_MESSAGES_PER_PACKET 128
#define MESSAGE_BUFFER_SIZE 256
#define MAX_PACKET_PAYLOAD_SIZE 1200

struct PacketStatus
{
    bool acked;
    uint16_t ordered_messages[MAX_MESSAGES_PER_PACKET];
    int ordered_message_count;
};

struct PacketHeader
{
    uint16_t sequence;
    uint16_t ack;
    uint32_t ack_bits;
    int ordered_message_count;
};

struct Message
{
    uint16_t id;
    int size;
    void *data;
};

struct OutMessageStatus
{
    bool acked;
    float send_cooldown;
};

struct MessageBuffer
{
    Message elements[MESSAGE_BUFFER_SIZE];
    uint16_t write_id;
    uint16_t read_id;
    int message_count;
};

class Channel
{
private:
    struct {
        uint16_t sequence = 0;
        uint32_t packet_sequence[PACKET_BUFFER_SIZE];
        PacketStatus packets[PACKET_BUFFER_SIZE] = {};

        OutMessageStatus ordered_message_status[MESSAGE_BUFFER_SIZE] = {};
        MessageBuffer ordered_messages = {};
        MessageBuffer standard_messages = {};
    } local;

    struct {
        uint16_t sequence = 0;
        uint32_t packet_sequence[PACKET_BUFFER_SIZE];

        MessageBuffer ordered_messages = {};
        MessageBuffer standard_messages = {};
    } remote;

    Socket *socket = nullptr;
    Address *address = nullptr;

private:
    PacketStatus *GetPacketStatus(uint16_t sequence);
    PacketStatus *InsertPacketStatus(uint16_t sequence);
    uint32_t GetAckBits(uint16_t ack);
    void ReceiveSequence(uint16_t sequence);
    void AckPacket(uint16_t sequence);

public:
    Channel();

    void Bind(Socket *socket, Address *address);
    void Update(float dt);
    void SendPackets();
    void ReceivePacket(void *data, int size);

    void SendMessageCh(void *data, int size, bool ordered);

    bool ReceiveMessage(void *data, int *size);
};

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2);

} // namespace Net

#endif // RELIABILITY_H