#ifndef RELIABILITY_H
#define RELIABILITY_H

#include "networking.h"

#include "data_structures.h"

#include <stdint.h>


namespace Net {

struct PacketStatus
{
    bool acked;
};

struct PacketHeader
{
    uint16_t sequence;
    uint16_t ack;
    uint32_t ack_bits;
};

class Channel
{
private:
    static const int buffer_size = 1024;

    bool received_packets[buffer_size] = {false};
    uint16_t remote_sequence = 0;

    PacketStatus sent_packets[buffer_size];
    uint16_t local_sequence = 0;

    Socket *socket;
    Address *address;

    RingBuffer data_buffer = RingBuffer(1024);

private:
    PacketStatus *GetPacketStatus(uint16_t sequence);
    uint32_t GetAckBits(uint16_t ack);
    void ReceiveSequence(uint16_t sequence);
    void AckPacket(uint16_t sequence);

public:
    void Bind(Socket *socket, Address *address);
    void Send(void *data, int size);
    void Receive(void *data, int size);
    void Clear();
    void *GetData(int *size);
};

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2);

} // namespace Net

#endif // RELIABILITY_H