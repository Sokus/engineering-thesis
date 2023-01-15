#include "server.h"

#include <stddef.h>
#include <stdio.h>

#include "system/pi_time.h"
#include "system/network.h"
#include "game/level.h"

#include "parg.h"

unsigned int port = 60000;

enum PargError
{
    PARG_SUCCESS = 0,
    PARG_ERROR_INVALID_PORT = -1,
    PARG_ERROR_MISSING_PORT = -2,
    PARG_ERROR_UNKNOWN_OPTION = -3,
    PARG_ERROR_UNKNOWN_ERROR = -4,
};

int ProcessArguments(int argc, char *argv[])
{
    parg_state parg;
    parg_init(&parg);
    int parg_opt;
    while ((parg_opt = parg_getopt(&parg, argc, argv, "hp:")) != -1)
    {
        switch (parg_opt)
        {
            case 'h': printf("Usage: server [-h] [-p PORT]\n"); break;
            case 'p':
                if (sscanf(parg.optarg, "%u", &port) <= 0) {
                    printf("invalid port (%s)", parg.optarg);
                    return PARG_ERROR_INVALID_PORT;
                }
            case '?':
                switch (parg.optopt)
                {
                    case 'p':
                        printf("option -p required a port number\n");
                        return PARG_ERROR_MISSING_PORT;
                    default:
                        //printf("unknown option %c\n", parg.optopt);
                        //return PARG_ERROR_UNKNOWN_ERROR;
                        return PARG_SUCCESS;
                }
            default:
                printf("unhandled option: -%c\n", parg_opt);
                return PARG_ERROR_UNKNOWN_OPTION;
        }
    }

    return PARG_SUCCESS;
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) printf("%s ", argv[i]); printf("\n");
    InitializeTime();
    InitializeNetwork();

    int pa_rc = ProcessArguments(argc, argv);

    Server server = {};
    printf("server starting on port %u\n", port);
    server.Init(SocketCreate(SOCKET_IPV4, port));

    float hz = 30.0f; // refresh rate
    float dt = 1.0f / hz;
    uint64_t last_time = Time_Now();
    while (true)
    {
        server.ReceivePackets();
        server.world.Update(server.client_input, MAX_CLIENTS, dt);
        server.SendPackets();
        server.SendWorldState();
        server.CheckForTimeOut();

        uint64_t laptime = Time_Laptime(&last_time);
        double laptime_ms = Time_Ms(laptime);
        double sleeptime_ms = (double)dt - laptime_ms;
        if (sleeptime_ms >= 1.0)
            Time_Sleep((unsigned long)sleeptime_ms);
    }

    ShutdownNetwork();

    return 0;
}