#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "pi_time.h"
#include "config.h"

#include <stddef.h>
#include <stdio.h>

#include "nettemp.h"

struct Client
{
    ConnectionState state;
    Net::Address address;
    Net::Channel channel;
};

int main(int argc, char *argv[])
{
    Net::InitializeSockets();
    Time::Setup();

    Client clients[CLIENT_COUNT] = {};
    Net::Socket socket = Net::Socket(48620, false);

    while(true)
    {
        uint8_t buffer[4096];
        int bytes_received;
        Net::Address sender;
        while(bytes_received = socket.Receive(&sender, buffer, 4096))
        {
            ClientId client_id = CLIENT_INVALID;
            ClientId free_id = CLIENT_INVALID;

            for(int client_idx = 0;
                client_idx < CLIENT_COUNT && client_id == CLIENT_INVALID;
                client_idx++)
            {
                Client *existing_client = clients + client_idx;
                switch(existing_client->state)
                {
                    case DISCONNECTED:
                    {
                        if(free_id == CLIENT_INVALID)
                            free_id = (ClientId)client_idx;
                    } break;

                    case CONNECTING:
                    case CONNECTED:
                    {
                        if(Net::AddressCompare(sender, existing_client->address))
                            client_id = (ClientId)client_idx;
                    } break;

                    default: break;
                }
            }

            if(client_id != CLIENT_INVALID)
            {
                if(clients[client_id].state == CONNECTED)
                {
                    clients[client_id].channel.ReceivePacket(buffer, bytes_received);
                }
            }
            else if(free_id != CLIENT_INVALID)
            {
                printf("%s is connecting\n", sender.ToString());
                Client *new_client = clients + free_id;
                new_client->channel.Clear();
                new_client->address = sender;
                new_client->channel.Bind(&socket, &new_client->address);
                new_client->state = CONNECTING;
                new_client->channel.ReceivePacket(buffer, bytes_received);
            }
            else
            {
                Arena msg_arena = Arena(buffer, sizeof(buffer));
                MessageType *msg_type = (MessageType *)msg_arena.Push(sizeof(MessageType));
                // server full
            }
        }

        for(int client_idx = 0;
            client_idx < CLIENT_COUNT;
            client_idx++)
        {
            Client *client = clients + client_idx;

            while(client->channel.ReceiveMessage(buffer, &bytes_received))
            {
                Arena msg_arena = Arena(buffer, bytes_received);
                MessageType *msg_type = (MessageType *)msg_arena.Push(sizeof(MessageType));

                if(client->state == CONNECTING)
                {
                    if(*msg_type == CONNECT)
                    {
                        printf("%s connected\n", client->address.ToString());
                        MessageType response = APPROVE;
                        client->channel.SendMessageEx(&response, sizeof(response), true);
                        client->state = CONNECTED;
                    }
                }
                else if(client->state == CONNECTED)
                {
                    if(*msg_type == DISCONNECT)
                    {
                        printf("%s disconnected\n", client->address.ToString());
                        client->state = DISCONNECTED;
                    }
                    else if(*msg_type == VALUE)
                    {
                        MessageValue *msg_value = (MessageValue *)msg_arena.Push(sizeof(MessageValue));
                        printf("value %u received from %s\n", msg_value->value, client->address.ToString());
                    }
                }
            }

            if(client->state != DISCONNECTED)
            {
                client->channel.SendPackets();
                client->channel.NextFrame();
            }
        }

        Time::SleepMs(500);
    }

    Net::ShutdownSockets();

    return 0;
}