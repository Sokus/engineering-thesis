#include "reliability.h"

#include "networking.h"
#include "serialization/serialization.h"
#include "pi_time.h"

#include <stdio.h>
#include <string.h> // memset

namespace Net {

Channel::Channel()
{
    out.standard_messages = RingBuffer(1024);
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
    uint8_t payload_buf[MAX_PACKET_SIZE];
    Arena payload_arena = Arena(payload_buf, sizeof(payload_buf));

    while(out.standard_messages.BytesWritten() > 0)
    {
        size_t read_offset = out.standard_messages.ReadOffset();
        uint16_t message_size;
        out.standard_messages.Read(&message_size, sizeof(message_size));
        if(!payload_arena.WouldOverflow(sizeof(message_size) + message_size))
        {
            uint16_t *message_size_ptr = (uint16_t *)payload_arena.Push(sizeof(message_size));
            memcpy(message_size_ptr, &message_size, sizeof(message_size));
            void *message_ptr = payload_arena.Push(message_size);
            out.standard_messages.Read(message_ptr, message_size);
        }
        else
        {
            socket->Send(*address, payload_buf, (int)payload_arena.BytesUsed());
            payload_arena.Clear();
            out.standard_messages.RewindRead(read_offset);
        }
    }

    if(payload_arena.BytesUsed() > 0)
    {
        socket->Send(*address, payload_buf, (int)payload_arena.BytesUsed());
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
    ASSERT(!out.standard_messages.WouldOverflow(sizeof(message_size) + message_size));
    out.standard_messages.Write(&message_size, sizeof(message_size));
    out.standard_messages.Write(data, message_size);
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