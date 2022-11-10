#include "reliability.h"

#include "networking.h"
#include "serialization/serialization.h"
#include "pi_time.h"

#include <stdio.h>
#include <string.h> // memset

namespace Net {

Channel::Channel()
{
    out.standard_messages.Init(1024);
    in.messages.Init(1024);
}

void Channel::Clear()
{
    socket = nullptr;
    address = nullptr;
    out.standard_messages.Clear();
    in.messages.Clear();
}

void Channel::Bind(Socket *socket, Address *address)
{
    this->socket = socket;
    this->address = address;
}

void Channel::NextFrame()
{
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
    Arena packet_arena = Arena(data, size);

    while(packet_arena.BytesUsed() < size)
    {
        uint16_t *message_size = (uint16_t *)packet_arena.Push(sizeof(uint16_t));
        void *in_data = packet_arena.Push(*message_size);
        in.messages.Write(message_size, sizeof(uint16_t));
        in.messages.Write(in_data, *message_size);
    }
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
    if(in.messages.BytesWritten() > 0)
    {
        uint16_t message_size;
        in.messages.Read(&message_size, sizeof(uint16_t));
        in.messages.Read(data, message_size);
        *size = message_size;
        return true;
    }

    return false;
}

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2)
{
    return ((s1 > s2) && (s1 - s2 <= 32768)) ||
           ((s1 < s2) && (s2 - s1  > 32768));
}



} // namespace Net