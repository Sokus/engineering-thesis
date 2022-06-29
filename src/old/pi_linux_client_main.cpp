#include <stdio.h>
#include <stdint.h>

// networking
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // inet_addr
#include <fcntl.h>        // set file/socket handle to non-blocking
#include <unistd.h>       // write(), close()
#include <errno.h>        // socket error handling

#include "pi_helpers.h"
#include "pi_networking.h"
#include "pi_networking.cpp"
#include "pi_linux_networking.cpp"

struct Client
{
    int socket_handle;
    Address address;
    
    uint16_t sequence;
    
    uint16_t reliable_sequence;
    uint16_t ack_reliable;
    
    // TODO(sokus): find optimal sizes for those buffers
    uint8_t reliable_backbuffer[3096];
    uint8_t standard_backbuffer[3096];
    RingBuffer reliable_ring_buffer;
    RingBuffer standard_ring_buffer;
    
    uint8_t pending_reliable_backbuffer[1000];
    StreamBuffer pending_reliable_stream_buffer;
};

struct NetChanReliableHeader
{
    uint16_t reliable_data_sequence;
    uint16_t reliable_data_size;
};

struct Command
{
    uint16_t payload_size;
};

void FillStreamWithRingBufferCommands(StreamBuffer *sbuf, RingBuffer *rbuf)
{
    while(RingBufferUsedSpace(rbuf) > 0)
    {
        Command cmd;
        RingBufferRead(rbuf, &cmd, sizeof(Command));
        size_t total_size_to_push = sizeof(Command) + cmd.payload_size;
        if(total_size_to_push <= StreamBufferFreeSpace(sbuf))
        {
            StreamBufferWrite(sbuf, &cmd, sizeof(Command));
            RingBufferArea area = RingBufferPop(rbuf, cmd.payload_size);
            StreamBufferWrite(sbuf, area.first_part, area.first_part_size);
            if(area.second_part_size)
                StreamBufferWrite(sbuf, area.second_part, area.second_part_size);
        }
        else
        {
            RingBufferRewindReadPointer(rbuf, sizeof(Command));
            break;
        }
    }
}

void NetChannelSend(Client *client, bool include_reliable)
{
    RingBuffer *standard_data_rbuf = &client->standard_ring_buffer;
    StreamBuffer *reliable_data_sbuf = &client->pending_reliable_stream_buffer;
    
    size_t standard_data_size = RingBufferUsedSpace(standard_data_rbuf);
    size_t reliable_data_size = StreamBufferUsedSpace(reliable_data_sbuf);
    
    if(standard_data_size > 0 || reliable_data_size > 0)
    {
        uint8_t out_backbuffer[1400];
        StreamBuffer sbuf = CreateStreamBuffer(out_backbuffer, sizeof(out_backbuffer));
        PacketHeader *packet_header = STREAM_PUSH_STRUCT(&sbuf, PacketHeader);
        packet_header->sequence = client->sequence;
        packet_header->ack_reliable = client->ack_reliable;
        packet_header->is_reliable = include_reliable && (reliable_data_size > 0);
        
        if(reliable_data_size > 0)
        {
            NetChanReliableHeader *reliable_header = STREAM_PUSH_STRUCT(&sbuf, NetChanReliableHeader);
            reliable_header->reliable_data_sequence = client->reliable_sequence;
            reliable_header->reliable_data_size = (uint16_t)reliable_data_size;
            StreamBufferWrite(&sbuf, reliable_data_sbuf->base, reliable_data_size);
        }
        
        if(standard_data_size > 0)
        {
            FillStreamWithRingBufferCommands(&sbuf, standard_data_rbuf);
        }
        
        SocketSend(client->socket_handle,
                   &client->address,
                   sbuf.base,
                   (int)StreamBufferUsedSpace(&sbuf));
    }
}

void NetChannelFlush(Client *client)
{
    RingBuffer *reliable_rbuf = &client->reliable_ring_buffer;
    RingBuffer *standard_rbuf = &client->standard_ring_buffer;
    StreamBuffer *reliable_sbuf = &client->pending_reliable_stream_buffer;
    
    if(StreamBufferUsedSpace(reliable_sbuf) == 0)
    {
        FillStreamWithRingBufferCommands(reliable_sbuf, reliable_rbuf);
    }
    
    if(StreamBufferUsedSpace(reliable_sbuf) > 0)
        NetChannelSend(client, true);
    
    while(RingBufferUsedSpace(standard_rbuf) > 0)
        NetChannelSend(client, false);
}

void NetChannelPush(Client *client, void *data, size_t data_size, bool reliable)
{
    Command cmd;
    
    // TODO(sokus): the size shouldn't ever be bigger than MTU
    cmd.payload_size = (uint16_t)data_size;
    size_t total_size_to_push = sizeof(Command) + data_size;
    
    if(reliable)
    {
        RingBuffer *reliable_ring_buffer = &client->reliable_ring_buffer;
        
        if(total_size_to_push <= RingBufferFreeSpace(reliable_ring_buffer))
        {
            RingBufferWrite(reliable_ring_buffer, &cmd, sizeof(Command));
            RingBufferWrite(reliable_ring_buffer, data, data_size);
        }
        else
        {
            // TODO(sokus): reliable ring buffer full, drop the client
        }
    }
    else
    {
        RingBuffer *standard_ring_buffer = &client->standard_ring_buffer;
        
        if(total_size_to_push <= RingBufferFreeSpace(standard_ring_buffer))
        {
            RingBufferWrite(standard_ring_buffer, &cmd, sizeof(Command));
            RingBufferWrite(standard_ring_buffer, data, data_size);
        }
        else
        {
            NetChannelSend(client, false);
        }
    }
}

int main(void)
{
    
    return 0;
}