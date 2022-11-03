#include "reliability.h"

#include "networking.h"
#include "serialization/serialization.h"

#include <stdio.h>
#include <string.h> // memset

namespace Net {

Channel::Channel()
{

}

void Channel::Bind(Socket *socket, Address *address)
{
    this->socket = socket;
    this->address = address;
}

void Channel::Update(float dt)
{

}

void Channel::SendPackets()
{

}

void Channel::ReceivePacket(void *data, int size)
{

}

void Channel::SendMessageCh(void *data, int size, bool ordered)
{

}

bool Channel::ReceiveMessage(void *data, int *size)
{

    return true;
}

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2)
{
    return ((s1 > s2) && (s1 - s2 <= 32768)) ||
           ((s1 < s2) && (s2 - s1  > 32768));
}



} // namespace Net