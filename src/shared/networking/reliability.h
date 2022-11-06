#ifndef RELIABILITY_H
#define RELIABILITY_H

#include "networking.h"

#include "data_structures.h"

#include <stdint.h>

namespace Net {

struct PacketHeader
{
    uint16_t standard_sequence;      // incremented on every packet send
    uint16_t reliable_sequence;      // incremented when delievery confirmed
    uint16_t reliable_messages_size; // size of all reliable messages
    uint16_t reliable_ack;           // last received reliable sequence
    uint16_t frame;                  // incremented every frame
};

#define RELIABLE_MESSAGE_SEND_INTERVAL 100 // send interval in ms
#define MAX_PACKET_SIZE 1200

class Channel
{
private:
    struct Out {
        uint16_t frame;

        uint16_t standard_sequence;
        RingBuffer standard_messages;     // queued standard messages

        uint16_t reliable_sequence;       // current outgoing sequence
        RingBuffer reliable_messages;     // queued reliable messages
        uint8_t reliable_stack_buf[1024]; // current outgoing message stack
        int reliable_stack_used;
        uint64_t last_reliable_send_time;
    } out;

    struct In {
        uint16_t frame;             // latest received frame
        uint16_t reliable_sequence; // latest received reliable sequence
        RingBuffer messages;        // messages ready to be received
    } in;

    Socket *socket = nullptr;
    Address *address = nullptr;

public:
    Channel();

    void Bind(Socket *socket, Address *address);
    void NextFrame();
    void SendPackets();
    void ReceivePacket(void *data, int size);

    bool SendMessageEx(void *data, int size, bool reliable);
    bool ReceiveMessage(void *data, int *size);
};

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2);

} // namespace Net

#endif // RELIABILITY_H