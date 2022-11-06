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
    int reliable_message_size_to_send = 0;

    // if there are no reliable messages waiting to be send go through
    // the message queue and fill the reliable_stack_buf,
    // otherwise check if enough time has passed since the last time
    // they were send and re-send them if necessary.
    if(out.reliable_stack_used == 0)
    {
        while(out.reliable_messages.BytesWritten() > 0)
        {
            size_t read_position = out.reliable_messages.ReadOffset();
            uint16_t message_size;
            out.reliable_messages.Read(&message_size, sizeof(uint16_t));

            uint16_t size_required = sizeof(uint16_t) + message_size;
            if(out.reliable_stack_used + size_required <= ARRAY_SIZE(out.reliable_stack_buf))
            {
                memcpy(out.reliable_stack_buf + out.reliable_stack_used, &message_size, sizeof(uint16_t));
                out.reliable_stack_used += sizeof(uint16_t);
                out.reliable_messages.Read(out.reliable_stack_buf + out.reliable_stack_used, message_size);
                out.reliable_stack_used += message_size;
                reliable_message_size_to_send = out.reliable_stack_used;
            }
            else
            {
                out.reliable_messages.RewindRead(read_position);
                break;
            }
        }
    }
    else
    {
        uint64_t time_since_last_send = Time::Since(out.last_reliable_send_time);
        if(Time::Ms(time_since_last_send) >= (double)RELIABLE_MESSAGE_SEND_INTERVAL)
        {
            reliable_message_size_to_send = out.reliable_stack_used;
            out.last_reliable_send_time = Time::Now();
        }
    }

    // start filling the payload and PacketHeader data
    uint8_t payload[MAX_PACKET_SIZE];
    int payload_used = 0;
    PacketHeader *packet_header = (PacketHeader *)payload;
    payload_used += sizeof(PacketHeader);
    packet_header->standard_sequence = out.standard_sequence;
    packet_header->reliable_sequence = out.reliable_sequence;
    packet_header->reliable_ack = in.reliable_sequence;
    packet_header->frame = out.frame;

    // if there are reliable messages waiting
    // to be sent push them onto the payload
    if(reliable_message_size_to_send > 0)
    {
        packet_header->reliable_messages_size = reliable_message_size_to_send;
        memcpy(payload + payload_used, out.reliable_stack_buf, out.reliable_stack_used);
        payload_used += out.reliable_stack_used;
    }
    else
    {
        packet_header->reliable_messages_size = 0;
    }

    // push all standard messages onto the payload until the queue is emptied
    // (if there is not enough space multiple packets might be sent)
    int standard_messages_to_send = 0;
    while(out.standard_messages.BytesWritten() > 0)
    {
        size_t read_position = out.standard_messages.ReadOffset();
        uint16_t message_size;
        out.standard_messages.Read(&message_size, sizeof(uint16_t));

        // push next message onto the payload, send the packet and
        // re-initialize the PacketHeader if there is not enough space
        uint16_t size_required = sizeof(uint16_t) + message_size;
        if(payload_used + size_required <= MAX_PACKET_SIZE)
        {
            memcpy(payload + payload_used, &message_size, sizeof(uint16_t));
            payload_used += sizeof(uint16_t);
            out.standard_messages.Read(payload + payload_used, message_size);
            payload_used += message_size;
            standard_messages_to_send++;
        }
        else
        {
            out.standard_messages.RewindRead(read_position);
            socket->Send(*address, payload, payload_used);
            standard_messages_to_send = 0;
            reliable_message_size_to_send = 0;

            payload_used = sizeof(PacketHeader);
            packet_header->standard_sequence = ++out.standard_sequence;
            packet_header->reliable_messages_size = 0;
        }
    }

    if(standard_messages_to_send || reliable_message_size_to_send)
    {
        socket->Send(*address, payload, payload_used);
        out.standard_sequence++;
    }

}

void Channel::ReceivePacket(void *data, int size)
{
    uint8_t *payload = (uint8_t *)data;
    int payload_used = 0;
    if(payload_used + sizeof(PacketHeader) > size) return;

    PacketHeader *packet_header = (PacketHeader *)(payload + payload_used);
    payload_used += sizeof(PacketHeader);

    printf("Packet received, size: %d\n", size);
    printf("PacketHeader:\n");
    printf(" standard_sequence: %u\n", packet_header->standard_sequence);
    printf(" reliable_sequence: %u\n", packet_header->reliable_sequence);
    printf(" reliable_messages_size: %u\n", packet_header->reliable_messages_size);
    printf(" reliable_ack: %u\n", packet_header->reliable_ack);
    printf(" frame: %u\n", packet_header->frame);

    if(payload_used + packet_header->reliable_messages_size > size) return;

    size_t write_offset = in.messages.WriteOffset();
    bool something_wrong = false;

    uint16_t expected_reliable_sequence = in.reliable_sequence + 1;
    if(packet_header->reliable_messages_size > 0 &&
       packet_header->reliable_sequence == expected_reliable_sequence)
    {
        int reliable_messages_payload_limit = payload_used + packet_header->reliable_messages_size;
        while(payload_used < reliable_messages_payload_limit)
        {
            if(payload_used + sizeof(uint16_t) > reliable_messages_payload_limit)
            {
                something_wrong = true;
                break;
            }

            uint16_t message_size = *(uint16_t *)(payload + payload_used);
            payload_used += sizeof(uint16_t);

            if(payload_used + message_size > reliable_messages_payload_limit ||
               in.messages.WouldOverflow(sizeof(uint16_t) + message_size))
            {
                something_wrong = true;
                break;
            }

            payload_used += message_size;
            in.messages.Write(&message_size, sizeof(uint16_t));
            in.messages.Write(payload + payload_used, message_size);
        }

        if(payload_used != reliable_messages_payload_limit)
            something_wrong = true;
    }

    if(packet_header->frame == in.frame ||
       SequenceGreaterThan(packet_header->frame, in.frame))
    {
        while(payload_used < size)
        {
            if(payload_used + sizeof(uint16_t) > size)
            {
                something_wrong = true;
                break;
            }

            uint16_t message_size = *(uint16_t *)(payload + payload_used);
            payload_used += sizeof(uint16_t);

            if(payload_used + message_size > size ||
               in.messages.WouldOverflow(sizeof(uint16_t) + message_size))
            {
                something_wrong = true;
                break;
            }

            in.messages.Write(&message_size, sizeof(uint16_t));

            in.messages.Write(payload + payload_used, message_size);
            payload_used += message_size;
        }

        ASSERT(payload_used == size);
    }

    if(!something_wrong)
    {
        // delievery of reliable messages confirmed
        if(packet_header->reliable_ack == out.reliable_sequence)
        {
            out.reliable_stack_used = 0;
            out.reliable_sequence++;
        }

        if(packet_header->reliable_messages_size > 0 &&
           packet_header->reliable_sequence == expected_reliable_sequence)
        {
            in.reliable_sequence = packet_header->reliable_sequence;
        }

        if(SequenceGreaterThan(packet_header->frame, in.frame))
            in.frame = packet_header->frame;
    }
    else
    {
        printf("Something was wrong\n");
        in.messages.RewindWrite(write_offset);
    }
}

bool Channel::SendMessageEx(void *data, int size, bool reliable)
{
    RingBuffer *ring_buffer = (reliable ? &out.reliable_messages : &out.standard_messages);
    uint16_t message_size = size;
    if(!ring_buffer->WouldOverflow(sizeof(uint16_t) + message_size))
    {
        ring_buffer->Write(&message_size, sizeof(uint16_t));
        ring_buffer->Write(data, message_size);
        return true;
    }
    else
    {
        return false;
    }
}

bool Channel::ReceiveMessage(void *data, int *size)
{
    if(in.messages.BytesWritten() > 0)
    {
        uint16_t message_size;
        in.messages.Read(&message_size, sizeof(uint16_t));
        ASSERT(message_size < 128);
        in.messages.Read(data, message_size);
        *size = message_size;
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