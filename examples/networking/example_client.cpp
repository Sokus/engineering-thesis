#include <stdint.h>
#include <stdio.h>

#include "log.h"
#include "system/pi_time.h"

#include "system.h"
#include "macros.h"

#include <string.h>

int main(int argc, char *argv[])
{
    SetLogPrefix("CLIENT: ");

    unsigned short server_port = 25565;

    ProcessHandle *process;
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

        process = CreateChildProcess(launch_arguments, ARRAY_SIZE(launch_arguments));
    }

    //Net::InitializeSockets();
    InitializeTime();
    Time_Sleep(50);

    //Net::Socket socket = Net::Socket(false);
    //Net::Address server_address = Net::Address(127, 0, 0, 1, server_port);

    //Net::ShutdownSockets();

    return 0;
}