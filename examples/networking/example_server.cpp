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
        uint8_t in_buffer[2048];
        uint8_t out_buffer[1024];
        Net::Address sender;
        int bytes_received;
        while(bytes_received = socket.Receive(&sender, in_buffer, sizeof(in_buffer)))
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
                    clients[client_id].channel.ReceivePacket(in_buffer, bytes_received);
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
                new_client->channel.ReceivePacket(in_buffer, bytes_received);
            }
            else
            {
                printf("connection for %s refused, server full\n", sender.ToString());
                BitPacker bit_packer = BitWriter(out_buffer, sizeof(out_buffer));
                MessageType message_type = JOIN_REFUSE;
                SerializeInteger(&bit_packer, (int32_t *)&message_type, 0, MESSAGE_TYPE_MAX);
                Flush(&bit_packer);
                socket.Send(sender, out_buffer, BytesWritten(&bit_packer));
            }
        }

        for(int client_idx = 0;
            client_idx < CLIENT_COUNT;
            client_idx++)
        {
            Client *client = clients + client_idx;

            while(client->channel.ReceiveMessage(in_buffer, &bytes_received))
            {
                BitPacker bit_reader = BitReader(in_buffer, sizeof(in_buffer));
                MessageType msg_type;
                SerializeInteger(&bit_reader, (int32_t *)&msg_type, 0, MESSAGE_TYPE_MAX);

                if(client->state == CONNECTING)
                {
                    if(msg_type == JOIN_ATTEMPT)
                    {
                        printf("%s connected\n", client->address.ToString());
                        BitPacker bit_writer = BitWriter(out_buffer, sizeof(out_buffer));
                        MessageType response = JOIN_APPROVE;
                        SerializeInteger(&bit_writer, (int32_t *)&response, 0, MESSAGE_TYPE_MAX);
                        Flush(&bit_writer);
                        client->channel.SendMessageEx(out_buffer, BytesWritten(&bit_writer), true);
                        client->state = CONNECTED;
                    }
                }
                else if(client->state == CONNECTED)
                {
                    if(msg_type == DISCONNECT)
                    {
                        printf("%s disconnected\n", client->address.ToString());
                        client->state = DISCONNECTED;
                    }
                    else if(msg_type == VALUE)
                    {
                        MessageValue msg_value;
                        SerializeMessageValue(&bit_reader, &msg_value);
                        printf("value %u received from %s\n", msg_value.value, client->address.ToString());
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