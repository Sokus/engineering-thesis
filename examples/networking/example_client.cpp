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
#else
#include <unistd.h>
#endif

#include <string.h>

struct ProcessHandle
{
    #if defined(_WIN32)
        PROCESS_INFORMATION info;
    #else
        pid_t pid;
    #endif
};



bool CreateChildProcess(char **argv, int argc, ProcessHandle *out_process_handle)
{
    bool result = false;
    #if defined(_WIN32)
        char cmd[256];
        size_t cmd_length = 0;
        for(int arg = 0; arg < argc; arg++)
        {
            size_t arg_length = strlen(argv[arg]);
            memcpy(cmd + cmd_length, argv[arg], arg_length);
            cmd_length += arg_length;
            if(arg < argc - 1) cmd[cmd_length++] = ' ';
        }
        cmd[cmd_length + 1] = '\0';

        STARTUPINFOA startup_info;
        GetStartupInfoA(&startup_info);
        result = CreateProcessA(NULL, cmd, NULL, NULL, false, 0, NULL,
                                NULL, &startup_info, &out_process_handle->info);
    #else
        pid_t pid = fork();
        if(pid)
            out_process_handle->pid = pid;
        else
            execve(argv[0], argv, 0);
        result = true;
    #endif
    return result;
}

bool KillChildProcess(ProcessHandle *process_handle)
{
    bool result = false;
    #if defined(_WIN32)
        result = TerminateProcess(process_handle->info.hProcess, 0);
    #else
        result = kill(process_handle->pid, SIGKILL);
    #endif
    return result;
}


unsigned int GetExecutablePath(char *buffer, size_t buffer_length)
{
    unsigned int length = 0;
    #if defined(_WIN32)
        length = (unsigned int)GetModuleFileNameA(0, buffer, (DWORD)buffer_length);
    #else
        length = (unsigned int)readlink("/proc/self/exe", buffer, buffer_length);
    #endif
    return length;
}

int main(int argc, char *argv[])
{
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
            if(executable_path[offset] == '\\')
                offset_to_one_past_last_slash = offset + 1;
        }
        memcpy(server_executable, executable_path, offset_to_one_past_last_slash);
        char server_executable_name[] = "example_server.exe";
        memcpy(server_executable + offset_to_one_past_last_slash,
               server_executable_name, sizeof(server_executable_name));

        char port_stringified[32];
        snprintf(port_stringified, sizeof(port_stringified), "%u", server_port);

        char *launch_arguments[] = {
            server_executable,
            "-p",
            port_stringified
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
        Time::SleepMs(60);
        channel.NextFrame();
    }

    Net::ShutdownSockets();

    return 0;
}