#include <stdint.h>
#include <stdio.h>

#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "pi_time.h"
#include "nettemp.h"

#include "system.h"

#include <string.h>

int main(int argc, char *argv[])
{
    SetLogPrefix("CLIENT: ");

    unsigned short server_port = 25565;

    ProcessHandle process;
    {
        char server_executable[255];
        char executable_path[255];
        unsigned int executable_path_length = GetExecutablePath(executable_path, sizeof(executable_path));
        unsigned int offset_to_one_past_last_slash = 0;
        for(unsigned int offset = 0; offset < executable_path_length; offset++)
        {
            if(executable_path[offset] == '\0') break;
            if(executable_path[offset] == '\\' || executable_path[offset] == '/')
                offset_to_one_past_last_slash = offset + 1;
        }
        memcpy(server_executable, executable_path, offset_to_one_past_last_slash);
        #if defined(_WIN32)
            char server_executable_name[] = "example_server.exe";
        #else
            char server_executable_name[] = "example_server";
        #endif
        memcpy(server_executable + offset_to_one_past_last_slash,
               server_executable_name, sizeof(server_executable_name));

        char port_stringified[32];
        snprintf(port_stringified, sizeof(port_stringified), "%u", server_port);

        char *launch_arguments[] = {
            server_executable,
            (char *)"-p",
            port_stringified,
            NULL
        };

        CreateChildProcess(launch_arguments, ARRAY_SIZE(launch_arguments), &process);
    }

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
                    Log(LOG_INFO, "Connected to the server");
                    connection_state = CONNECTED;
                }
                else if(msg_type == JOIN_REFUSE)
                {
                    Log(LOG_INFO, "Connection refused", server_address.ToString());
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
                    Log(LOG_INFO, "Value %u received", msg_value.value);
                }
            }
        }

        if(connection_state == DISCONNECTED && !already_tried_connecting)
        {
            BitPacker bit_writer = BitWriter(out_buffer, sizeof(out_buffer));
            Log(LOG_INFO, "Connecting to %s...", server_address.ToString());
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
            Log(LOG_INFO, "Sending a value of %d", msg_value.value);
            channel.SendMessageEx(out_buffer, BytesWritten(&bit_writer), false);
        }

        channel.SendPackets();
        Time::SleepMs(60);
        channel.NextFrame();
    }

    Net::ShutdownSockets();

    return 0;
}