#include <stdint.h>
#include <stdio.h>

#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "pi_time.h"
#include "config.h"
#include "nettemp.h"

int main(int argc, char *argv[])
{
    Net::InitializeSockets();
    Time::Setup();

    Net::Socket socket = Net::Socket(false);
    Net::Address server_address = Net::Address(127, 0, 0, 1, 48620);

    Net::Channel channel = Net::Channel();
    channel.Bind(&socket, &server_address);

    ConnectionState connection_state = DISCONNECTED;

    while(true)
    {
        uint8_t buffer[4096];
        int bytes_received;
        Net::Address sender;
        while(bytes_received = socket.Receive(&sender, buffer, 4096))
        {
            printf("%d bytes received from %s\n", bytes_received, sender.ToString());
            if(Net::AddressCompare(sender, server_address))
            {
                channel.ReceivePacket(buffer, bytes_received);
            }
        }

        while(channel.ReceiveMessage(buffer, &bytes_received))
        {
            Arena msg_arena = Arena(buffer, bytes_received);
            MessageType msg_type = *(MessageType *)msg_arena.Push(sizeof(MessageType));

            if(connection_state == CONNECTING)
            {
                if(msg_type == APPROVE)
                {
                    printf("connected to the server at%s\n", server_address.ToString());
                    connection_state = CONNECTED;
                }
                else if(msg_type == DISCONNECT)
                {
                    // server full?
                }
            }
            else if(connection_state == CONNECTED)
            {
                if(msg_type == DISCONNECT)
                {
                    // got disconnected
                }
                else if(msg_type == VALUE)
                {
                    MessageValue *msg_value = (MessageValue *)msg_arena.Push(sizeof(MessageValue));
                    printf("value %u received from %s\n", msg_value->value, sender.ToString());
                }
            }
        }

        if(connection_state == DISCONNECTED)
        {
            printf("trying to connect to %s\n", server_address.ToString());
            connection_state = CONNECTING;
            MessageType message = CONNECT;
            channel.SendMessageEx(&message, sizeof(message), true);
        }
        else if(connection_state == CONNECTED)
        {
            Arena msg_arena = Arena(buffer, sizeof(buffer));
            MessageType *msg_type = (MessageType *)msg_arena.Push(sizeof(MessageType));
            *msg_type = VALUE;
            MessageValue *msg_value = (MessageValue *)msg_arena.Push(sizeof(MessageValue));
            msg_value->value = 13;
            printf("sending value of %d to the server\n", msg_value->value);
            channel.SendMessageEx(buffer, (int)msg_arena.BytesUsed(), false);
        }

        channel.SendPackets();
        Time::SleepMs(3000);
        channel.NextFrame();
    }

    Net::ShutdownSockets();

    return 0;
}