#include "reliability.h"

#include "networking.h"
#include "serialization/serialization.h"

#include <stdio.h>
#include <string.h> // memset

namespace Net {

PacketStatus *Channel::GetPacketStatus(uint16_t sequence)
{
    int index = sequence % buffer_size;
    return sent_packets + index;
}

uint32_t Channel::GetAckBits(uint16_t ack)
{
    uint32_t ack_bits = 0;

    for(int offset = 0; offset < 32; offset++)
    {
        int index = (ack - offset - 1) % buffer_size;
        if(received_packets[index])
            ack_bits |= (1 << offset);
    }

    return ack_bits;
}

void Channel::ReceiveSequence(uint16_t sequence)
{
    if(sequence > remote_sequence)
    {
        for(int missing_sequence = remote_sequence + 1;
            missing_sequence < sequence;
            missing_sequence++)
        {
            int missing_sequence_index = missing_sequence % buffer_size;
            received_packets[missing_sequence_index] = false;
        }

        remote_sequence = sequence;
    }

    const int index = sequence % buffer_size;

    received_packets[index] = true;
}

void Channel::AckPacket(uint16_t sequence)
{
    PacketStatus *ps = GetPacketStatus(sequence);
    if(ps->acked == false)
    {
        ps->acked = true;
    }
}

void Channel::Bind(Socket *socket, Address *address)
{
    this->socket = socket;
    this->address = address;
}

void Channel::Send(void *data, int size)
{
    PacketStatus *ps = GetPacketStatus(local_sequence);

    ps->acked = false;

    PacketHeader ph;
    ph.sequence = local_sequence;
    ph.ack = remote_sequence;
    ph.ack_bits = GetAckBits(remote_sequence);

    uint8_t buffer[2000];
    int buffer_used = 0;
    memcpy(buffer + buffer_used, &ph, sizeof(PacketHeader));
    buffer_used += sizeof(PacketHeader);

    memcpy(buffer + buffer_used, data, size);
    buffer_used += size;

    socket->Send(*address, buffer, buffer_used);

    local_sequence++;
}

void Channel::Receive(void *data, int size)
{
    PacketHeader *ph = (PacketHeader *)data;

    ReceiveSequence(ph->sequence);
    AckPacket(ph->ack);

    for(int offset = 0; offset < 32; offset++)
    {
        if(ph->ack_bits & (1 << offset))
            AckPacket(ph->ack - offset - 1);
    }

    data_buffer.Write(size - sizeof(PacketHeader));
    data_buffer.Write((uint8_t *)data + sizeof(PacketHeader), size - sizeof(PacketHeader));
}

void Channel::Clear()
{
    memset(received_packets, 0x00, sizeof(received_packets));
    remote_sequence = 0;
    local_sequence = 0;
    socket = nullptr;
    address = nullptr;
    data_buffer.Clear();
}

void *Channel::GetData(int *size)
{
    if(data_buffer.BytesWritten())
    {
        static uint8_t buffer[2048];
        data_buffer.Read(*size);
        data_buffer.Read(buffer, *size);
        return buffer;
    }
    else
    {
        *size = 0;
        return nullptr;
    }
}

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2)
{
    return ((s1 > s2) && (s1 - s2 <= 32768)) ||
           ((s1 < s2) && (s2 - s1  > 32768));
}



} // namespace Net