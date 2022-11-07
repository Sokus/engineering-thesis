#ifndef RELIABILITY_H
#define RELIABILITY_H

#include "networking.h"

#include "data_structures.h"

#include <stdint.h>

namespace Net {

struct PacketHeader
{
};

#define RELIABLE_MESSAGE_SEND_INTERVAL 100 // send interval in ms
#define MAX_PACKET_SIZE 1200

class Channel
{
private:
    struct Out {
        uint8_t standard_messages_stack_buf[1024];
        int standard_messages_stack_used;
    } out;

    struct In {
        uint8_t messages_stack_buf[1024];
        int write_offset;
        int read_offset;
    } in;

    Socket *socket = nullptr;
    Address *address =  nullptr;

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