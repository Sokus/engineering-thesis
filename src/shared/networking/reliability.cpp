#include "reliability.h"

#include "networking.h"
#include "serialization/serialization.h"
#include "pi_time.h"

#include <stdio.h>
#include <string.h> // memset

namespace Net {

Channel::Channel()
{
    out.standard_messages_stack_used = 0;
    in.read_offset = 0;
    in.write_offset = 0;
}

void Channel::Bind(Socket *socket, Address *address)
{
    this->socket = socket;
    this->address = address;
}

void Channel::NextFrame()
{
    in.write_offset = 0;
    in.read_offset = 0;
}

void Channel::SendPackets()
{
    if(out.standard_messages_stack_used > 0)
    {
        uint8_t payload_data[MAX_PACKET_SIZE];
        int payload_size = 0;
        memcpy(payload_data, out.standard_messages_stack_buf, out.standard_messages_stack_used);
        payload_size += out.standard_messages_stack_used;
        socket->Send(*address, payload_data, payload_size);
        out.standard_messages_stack_used = 0;
    }
}

void Channel::ReceivePacket(void *data, int size)
{
    ASSERT(in.write_offset + size <= ARRAY_SIZE(in.messages_stack_buf));
    memcpy(in.messages_stack_buf + in.write_offset, data, size);
    in.write_offset += size;
}

bool Channel::SendMessageEx(void *data, int size, bool reliable)
{
    uint16_t message_size = size;
    int size_required = message_size + sizeof(message_size);
    ASSERT(out.standard_messages_stack_used + size_required <= ARRAY_SIZE(out.standard_messages_stack_buf));
    memcpy(out.standard_messages_stack_buf + out.standard_messages_stack_used,
           &message_size, sizeof(message_size));
    out.standard_messages_stack_used += sizeof(message_size);
    memcpy(out.standard_messages_stack_buf + out.standard_messages_stack_used,
           data, message_size);
    out.standard_messages_stack_used += message_size;
    return true;
}

bool Channel::ReceiveMessage(void *data, int *size)
{
    if(in.write_offset - in.read_offset > 0)
    {
        uint16_t message_size;
        ASSERT(in.read_offset + sizeof(message_size) < in.write_offset);
        memcpy(&message_size, in.messages_stack_buf + in.read_offset, sizeof(message_size));
        in.read_offset += sizeof(message_size);
        ASSERT(in.read_offset + message_size <= in.write_offset);
        memcpy(data, in.messages_stack_buf + in.read_offset, message_size);
        in.read_offset += message_size;
        return true;
    }
    else
    {
        return false;
    }
}

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2)
{
    return ((s1 > s2) && (s1 - s2 <= 32768)) ||
           ((s1 < s2) && (s2 - s1  > 32768));
}



} // namespace Net