#include "server.h"

#include <stddef.h>
#include <stdio.h>

#include "system/pi_time.h"
#include "system/network.h"

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
                    default: return PARG_ERROR_UNKNOWN_ERROR;
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
    InitializeTime();
    InitializeNetwork();

    ProcessArguments(argc, argv);

    ShutdownNetwork();

    return 0;
}