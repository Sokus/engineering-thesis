#include <stdint.h>
#include <stdio.h>

#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "pi_time.h"
#include "config.h"
#include "nettemp.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string.h>

int main(int argc, char *argv[])
{
    unsigned short server_port = 25565;

    #ifdef _WIN32
        STARTUPINFOA startup_info;
        GetStartupInfoA(&startup_info);
        PROCESS_INFORMATION process_info;

        char parent_directory[MAX_PATH];
        DWORD path_length = GetModuleFileNameA(0, parent_directory, sizeof(parent_directory));
        char *one_past_last_slash = parent_directory;
        for(unsigned int offset = 0;
            parent_directory[offset] != '\0' && offset < path_length;
            offset++)
        {
            if(parent_directory[offset] == '\\')
                one_past_last_slash = parent_directory + offset + 1;
        }
        char executable_and_args[64] = {};
        snprintf(executable_and_args, sizeof(executable_and_args), "example_server.exe -p %u", server_port);
        memcpy(one_past_last_slash, executable_and_args, sizeof(executable_and_args));

        bool cpr = CreateProcessA(NULL, executable_and_args, NULL, NULL, false,
                                  0, NULL, NULL, &startup_info, &process_info);
    #endif

    Net::InitializeSockets();
    Time::Setup();

    Time::SleepMs(50);

    Net::Socket socket = Net::Socket(false);
    Net::Address server_address = Net::Address(127, 0, 0, 1, server_port);

    Net::Channel channel = Net::Channel();
    channel.Bind(&socket, &server_address);

    ConnectionState connection_state = DISCONNECTED;
    bool already_tried_connecting = false;

    while(true)
    {
        uint8_t in_buffer[2048];
        uint8_t out_buffer[1024];
        int bytes_received;
        Net::Address sender;
        while(bytes_received = socket.Receive(&sender, in_buffer, sizeof(in_buffer)))
        {
            if(Net::AddressCompare(sender, server_address))
            {
                channel.ReceivePacket(in_buffer, bytes_received);
            }
        }

        while(channel.ReceiveMessage(in_buffer, &bytes_received))
        {
            BitPacker bit_reader = BitReader(in_buffer, sizeof(in_buffer));
            MessageType msg_type;
            SerializeInteger(&bit_reader, (int32_t *)&msg_type, 0, MESSAGE_TYPE_MAX);

            if(connection_state == CONNECTING)
            {
                if(msg_type == JOIN_APPROVE)
                {
                    printf("connected to the server at %s\n", server_address.ToString());
                    connection_state = CONNECTED;
                }
                else if(msg_type == JOIN_REFUSE)
                {
                    printf("connection refused\n");
                    connection_state = DISCONNECTED;
                    already_tried_connecting = true;
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
                    MessageValue msg_value;
                    SerializeMessageValue(&bit_reader, &msg_value);
                    printf("value %u received from %s\n", msg_value.value, sender.ToString());
                }
            }
        }

        if(connection_state == DISCONNECTED && !already_tried_connecting)
        {
            BitPacker bit_writer = BitWriter(out_buffer, sizeof(out_buffer));
            printf("trying to connect to %s\n", server_address.ToString());
            MessageType message = JOIN_ATTEMPT;
            SerializeInteger(&bit_writer, (int32_t *)&message, 0, MESSAGE_TYPE_MAX);
            Flush(&bit_writer);
            channel.SendMessageEx(out_buffer, BytesWritten(&bit_writer), true);
            connection_state = CONNECTING;
        }
        else if(connection_state == CONNECTED)
        {
            BitPacker bit_writer = BitWriter(out_buffer, sizeof(out_buffer));
            MessageType msg_type = VALUE;
            SerializeInteger(&bit_writer, (int32_t *)&msg_type, 0, MESSAGE_TYPE_MAX);
            MessageValue msg_value;
            msg_value.value = 13;
            SerializeMessageValue(&bit_writer, &msg_value);
            Flush(&bit_writer);
            printf("sending value of %d to the server\n", msg_value.value);
            channel.SendMessageEx(out_buffer, BytesWritten(&bit_writer), false);
        }

        channel.SendPackets();
        Time::SleepMs(3000);
        channel.NextFrame();
    }

    Net::ShutdownSockets();

    return 0;
}