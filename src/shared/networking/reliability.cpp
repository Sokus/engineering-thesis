#include "reliability.h"

#include "networking.h"
#include "serialization/serialization.h"
#include "pi_time.h"

#include <stdio.h>
#include <string.h> // memset

namespace Net {

Channel::Channel()
{
    // TODO: Check what are the optimal sizes for
    //       each of the buffers.

    out.frame = 0;
    out.standard_sequence = 0;
    out.standard_messages = RingBuffer(4096);

    out.reliable_sequence = 0;
    out.reliable_messages = 0;
    out.reliable_messages = RingBuffer(1024);
    out.reliable_stack_used = 0;

    in.frame = 0;
    in.reliable_sequence = 0xFFFF;
    in.messages = RingBuffer(4096);
}

void Channel::Bind(Socket *socket, Address *address)
{
    this->socket = socket;
    this->address = address;
}

void Channel::NextFrame()
{
    out.frame++;
}

void Channel::SendPackets()
{

}

void Channel::ReceivePacket(void *data, int size)
{

}

bool Channel::SendMessageEx(void *data, int size, bool reliable)
{

}

bool Channel::ReceiveMessage(void *data, int *size)
{

}

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2)
{
    return ((s1 > s2) && (s1 - s2 <= 32768)) ||
           ((s1 < s2) && (s2 - s1  > 32768));
}



} // namespace Net