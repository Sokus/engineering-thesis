#include "server.h"

#include <stddef.h>
#include <stdio.h>

#include "system/pi_time.h"
#include "system/network.h"
#include "game/level.h"

#include "parg.h"

unsigned int port = 50000;
Game::LevelType level_type = Game::LEVEL_PLAINS;

enum PargError
{
    PARG_SUCCESS = 0,
    PARG_ERROR_INVALID_PORT = -1,
    PARG_ERROR_MISSING_PORT = -2,
    PARG_ERROR_INVALID_LEVEL = -3,
    PARG_ERROR_MISSING_LEVEL = -4,
    PARG_ERROR_UNKNOWN_OPTION = -5,
    PARG_ERROR_UNKNOWN_ERROR = -6,
};

int ProcessArguments(int argc, char *argv[])
{
    parg_state parg;
    parg_init(&parg);
    int parg_opt;
    while ((parg_opt = parg_getopt(&parg, argc, argv, "hp:l:")) != -1)
    {
        switch (parg_opt)
        {
            case 'h': printf("Usage: server [-h] [-p PORT] [-l LEVEL]\n"); break;
            case 'p':
                if (sscanf(parg.optarg, "%u", &port) <= 0) {
                    printf("invalid port (%s)", parg.optarg);
                    return PARG_ERROR_INVALID_PORT;
                }
                break;
            case 'l':
            {
                int suggested_level = Game::LEVEL_PLAINS;
                if (sscanf(parg.optarg, "%d", &suggested_level) <= 0)
                {
                    printf("invalid level (%s)", parg.optarg);
                    return PARG_ERROR_INVALID_LEVEL;
                }
                if (suggested_level <= 0 || suggested_level >= Game::LEVEL_COUNT)
                {
                    printf("level %d out of range (1-%d)\n", suggested_level, Game::LEVEL_COUNT);
                    return PARG_ERROR_INVALID_LEVEL;
                }
                level_type = (Game::LevelType)suggested_level;
            }   break;
            case '?':
                switch (parg.optopt)
                {
                    case 'p':
                        printf("option -p required a port number\n");
                        return PARG_ERROR_MISSING_PORT;
                    case 'l':
                        printf("option -l required a level number\n");
                        return PARG_ERROR_MISSING_LEVEL;
                    default:
                        //printf("unknown option %c\n", parg.optopt);
                        //return PARG_ERROR_UNKNOWN_ERROR;
                        return PARG_SUCCESS;
                }
                break;
            default:
                printf("unhandled option: -%c\n", parg_opt);
                return PARG_ERROR_UNKNOWN_OPTION;
        }
    }

    return PARG_SUCCESS;
}

int main(int argc, char *argv[])
{
    InitializeTime();
    InitializeNetwork();
    Game::SetLoadTextures(false);

    int pa_rc = ProcessArguments(argc, argv);

    Server server = {};
    Socket socket = SocketCreate(SOCKET_IPV4, port);
    printf("server starting on port %u\n", port);
    server.Init(socket);
    server.level_type = level_type;
    server.world.Clear();
    Game::InitLevel(&server.world,  server.level_type);
    printf("initialised level %d\n",  server.level_type);

    float hz = 60.0f; // refresh rate
    float dt = 1.0f / hz;
    
    while (true)
    {
        const uint64_t t0 = Time_Now();

        server.ReceivePackets();
        server.world.Update(server.client_input, MAX_CLIENTS, dt);
        server.SendPackets();
        server.SendWorldState();
        server.CheckForTimeOut();

        const uint64_t laptime = Time_Now() - t0;
        double laptime_ms = Time_Ms(laptime);
        double sleeptime_ms = (double)dt * 1000.0 - laptime_ms;

        if (sleeptime_ms >= 1.0)
        {
            Time_Sleep((unsigned long)sleeptime_ms);
        }
    }

    ShutdownNetwork();

    return 0;
}