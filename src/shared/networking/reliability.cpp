#include "reliability.h"

#include "networking.h"
#include "serialization/serialization.h"

#include <stdio.h>
#include <string.h> // memset

namespace Net {

PacketStatus *Channel::GetPacketStatus(uint16_t sequence)
{
    const int index = sequence % PACKET_BUFFER_SIZE;
    if(local.packet_sequence[index] == sequence)
        return local.packets + index;
    else
        return nullptr;
}

PacketStatus *Channel::InsertPacketStatus(uint16_t sequence)
{
    const int index = sequence % PACKET_BUFFER_SIZE;
    local.packet_sequence[index] = sequence;
    return local.packets + index;
}

uint32_t Channel::GetAckBits(uint16_t ack)
{
    uint32_t ack_bits = 0;

    for(int offset = 0; offset < 32; offset++)
    {
        uint16_t sequence = ack - offset - 1;
        int index = (sequence + PACKET_BUFFER_SIZE) % PACKET_BUFFER_SIZE;
        if(remote.packet_sequence[index] == sequence)
            ack_bits |= (1 << offset);
    }

    return ack_bits;
}

void Channel::ReceiveSequence(uint16_t sequence)
{
    if(SequenceGreaterThan(sequence, remote.sequence))
    {
        for(uint16_t missing_sequence = remote.sequence + 1;
            missing_sequence != sequence;
            ++missing_sequence)
        {
            const int missing_sequence_index = missing_sequence % PACKET_BUFFER_SIZE;
            remote.packet_sequence[missing_sequence_index] = 0xFFFFFFFF;
        }

        remote.sequence = sequence;
    }

    const int index = sequence % PACKET_BUFFER_SIZE;
    remote.packet_sequence[index] = sequence;
}

void Channel::AckPacket(uint16_t sequence)
{
    PacketStatus *ps = GetPacketStatus(sequence);

    if(ps != nullptr && ps->acked == false)
    {
        for(int ordered_message_idx = 0;
            ordered_message_idx < ps->ordered_message_count;
            ++ordered_message_idx)
        {
            const uint16_t message_id = ps->ordered_messages[ordered_message_idx];
            const int index = message_id % MESSAGE_BUFFER_SIZE;
            if(local.ordered_messages.elements[index].id == message_id)
            {
                local.ordered_message_status->acked = true;
            }
        }

        ps->acked = true;
    }
}

Channel::Channel()
{
    memset(local.packet_sequence, 0xFFFFFFFF, sizeof(local.packet_sequence));
    memset(remote.packet_sequence, 0xFFFFFFFF, sizeof(remote.packet_sequence));
}

void Channel::Bind(Socket *socket, Address *address)
{
    this->socket = socket;
    this->address = address;
}

void Channel::Update(float dt)
{
    if(local.ordered_messages.message_count > 0)
    {
        bool read_id_adjusted = false;
        for(uint16_t id = local.ordered_messages.read_id;
            id != local.ordered_messages.write_id;
            id++)
        {
            const int index = id % MESSAGE_BUFFER_SIZE;
            OutMessageStatus *msg_stat = local.ordered_message_status + index;
            Message *msg = local.ordered_messages.elements + index;
            if(msg_stat->acked)
            {
                if(!read_id_adjusted)
                {
                    local.ordered_messages.read_id = id;
                    local.ordered_messages.message_count--;
                }
            }
            else
            {
                read_id_adjusted = true;
                msg_stat->send_cooldown -= dt;
            }
        }
    }
}

void Channel::SendPackets()
{
    uint8_t buffer[MAX_PACKET_PAYLOAD_SIZE];
    int payload_size = sizeof(PacketHeader);

    PacketHeader *ph = (PacketHeader *)buffer;
    ph->ordered_message_count = 0;

    PacketStatus *ps = InsertPacketStatus(local.sequence);
    ps->acked = false;
    ps->ordered_message_count = 0;

    int message_count = 0;

    for(uint16_t id = local.ordered_messages.read_id;
        id != local.ordered_messages.write_id;
        id++)
    {
        const int index = id % MESSAGE_BUFFER_SIZE;
        OutMessageStatus *msg_stat = local.ordered_message_status + index;
        Message *msg = local.ordered_messages.elements + index;
        if(msg_stat->acked == false && msg_stat->send_cooldown <= 0.0f)
        {
            int msg_size = sizeof(Message) - sizeof(void *) + msg->size;
            if(payload_size + msg_size <= MAX_PACKET_PAYLOAD_SIZE)
            {
                memcpy(buffer + payload_size, &msg->id, sizeof(msg->id));
                payload_size += sizeof(msg->id);
                memcpy(buffer + payload_size, &msg->size, sizeof(msg->size));
                payload_size += sizeof(msg->size);
                memcpy(buffer + payload_size, msg->data, msg->size);
                payload_size += msg->size;

                ps->ordered_messages[ps->ordered_message_count] = msg->id;
                ps->ordered_message_count++;
                ph->ordered_message_count++;
                message_count++;

                msg_stat->send_cooldown = 0.1f; // send every 100ms
            }
        }
    }

    while(local.standard_messages.message_count > 0)
    {
        int read_index = local.standard_messages.read_id % MESSAGE_BUFFER_SIZE;
        Message *msg = local.standard_messages.elements + read_index;
        int msg_size = sizeof(Message) - sizeof(void *) + msg->size;
        if(payload_size + msg_size <= MAX_PACKET_PAYLOAD_SIZE)
        {
            memcpy(buffer + payload_size, &msg->id, sizeof(msg->id));
            payload_size += sizeof(msg->id);
            memcpy(buffer + payload_size, &msg->size, sizeof(msg->size));
            payload_size += sizeof(msg->size);
            memcpy(buffer + payload_size, msg->data, msg->size);
            payload_size += msg->size;

            free(msg->data);
            msg->data = nullptr;

            local.standard_messages.read_id++;
            local.standard_messages.message_count--;
            message_count++;
        }
        else
        {
            break;
        }
    }

    if(message_count > 0)
    {
        ph->sequence = local.sequence;
        ph->ack = remote.sequence;
        ph->ack_bits = GetAckBits(remote.sequence);

        ASSERT(socket != 0 && address != 0);
        socket->Send(*address, buffer, payload_size);

        local.sequence++;
    }
}

void Channel::ReceivePacket(void *data, int size)
{
    int data_used = 0;

    ASSERT(data_used + sizeof(PacketHeader) <= size);
    PacketHeader *ph = (PacketHeader *)data;
    data_used += sizeof(PacketHeader);

    AckPacket(ph->ack);
    for(int offset = 0; offset < 32; offset++)
    {
        if(ph->ack_bits & (1 << offset))
            AckPacket(ph->ack - offset - 1);
    }

    int actual_ordered_messages = 0;
    while(actual_ordered_messages < ph->ordered_message_count)
    {
        if(remote.ordered_messages.message_count >= MESSAGE_BUFFER_SIZE) break;
        if(data_used + sizeof(uint16_t) + sizeof(int) > size) break;

        uint16_t message_id = *(uint16_t *)((uint8_t *)data + data_used);
        data_used += sizeof(uint16_t);
        int message_size = *(int *)((uint8_t *)data + data_used);
        data_used += sizeof(int);

        if(data_used + message_size > size) break;

        if(SequenceGreaterThan(message_id, remote.ordered_messages.read_id))
        {
            const int index = remote.ordered_messages.write_id % MESSAGE_BUFFER_SIZE;
            Message *msg = remote.ordered_messages.elements + index;
            msg->id = message_id;
            msg->size = message_size;
            msg->data = malloc(message_size);
            memcpy(msg->data, (uint8_t *)data + data_used, message_size);

            if(SequenceGreaterThan(message_id, remote.ordered_messages.write_id))
            {
                uint16_t id = remote.ordered_messages.write_id;
                while(id != message_id)
                {
                    remote.ordered_messages.message_count++;
                    id++;
                }
                remote.ordered_messages.write_id = message_id;
            }
        }

        actual_ordered_messages++;
    }

    while(true)
    {
        if(remote.standard_messages.message_count >= MESSAGE_BUFFER_SIZE) break;
        if(data_used + sizeof(uint16_t) + sizeof(int) > size) break;

        uint16_t message_id = *(uint16_t *)((uint8_t *)data + data_used);
        data_used += sizeof(uint16_t);
        int message_size = *(int *)((uint8_t *)data + data_used);
        data_used += sizeof(int);

        if(data_used + message_size > size) break;

        const int index = remote.standard_messages.write_id % MESSAGE_BUFFER_SIZE;
        Message *msg = remote.standard_messages.elements + index;
        msg->id = message_id;
        msg->size = message_size;
        msg->data = malloc(message_size);
        memcpy(msg->data, (uint8_t *)data + data_used, message_size);

        remote.standard_messages.write_id++;
        remote.standard_messages.message_count++;
    }

    if(actual_ordered_messages == ph->ordered_message_count)
        ReceiveSequence(ph->sequence);
}

void Channel::SendMessageCh(void *data, int size, bool ordered)
{
    MessageBuffer *mbuf = ordered ? &local.ordered_messages : &local.standard_messages;
    ASSERT(mbuf->message_count < MESSAGE_BUFFER_SIZE);
    ASSERT(size <= 1024);

    int write_index = mbuf->write_id % MESSAGE_BUFFER_SIZE;
    Message *msg = mbuf->elements + write_index;
    msg->id = mbuf->write_id;
    msg->size = size;

    ASSERT(msg->data == nullptr);
    msg->data = malloc(size);
    memcpy(msg->data, data, size);

    if(ordered)
    {
        OutMessageStatus *msg_status = local.ordered_message_status + write_index;
        msg_status->acked = false;
        msg_status->send_cooldown = 0.0f;
    }

    mbuf->write_id++;
    mbuf->message_count++;
}

bool Channel::ReceiveMessage(void *data, int *size)
{
    Message *msg = nullptr;
    MessageBuffer *mbuf = nullptr;

    if(remote.standard_messages.message_count > 0)
    {
        mbuf = &remote.standard_messages;
        const int index = mbuf->read_id % MESSAGE_BUFFER_SIZE;
        msg = mbuf->elements + index;
    }
    else if(remote.ordered_messages.message_count > 0)
    {
        mbuf = &remote.ordered_messages;
        const int index = mbuf->read_id % MESSAGE_BUFFER_SIZE;
        Message *tmsg = mbuf->elements + index;
        if(tmsg->id == mbuf->read_id)
            msg = tmsg;
    }

    if(msg != nullptr)
    {
        memcpy(data, msg->data, msg->size);
        *size = msg->size;

        free(msg->data);
        msg->data = nullptr;
        mbuf->message_count--;
        mbuf->read_id++;

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