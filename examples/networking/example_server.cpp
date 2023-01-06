#include "system/pi_time.h"

#include <stddef.h>
#include <stdio.h>

#include "parg.h"

int main(int argc, char *argv[])
{
    InitializeTime();

    unsigned int port = 25565;

    parg_state parg;
    int parg_opt;
    parg_init(&parg);
    while((parg_opt = parg_getopt(&parg, argc, argv, "hp:")) != -1)
    {
        switch(parg_opt)
        {
            case 'h': printf("Usage: example_server [-h] [-p PORT]\n"); return 0; break;
            case 'p': if(sscanf(parg.optarg, "%u", &port) <= 0) return -1; break;
            case '?':
                switch(parg.optopt)
                {
                    case 'p': printf("option -p required a port number\n"); return -1; break;
                    default: return -1; break;
                }
                break;
            default: printf("error: unhandled option: -%c\n", parg_opt); return -1; break;
        }
    }

    printf("Starting on port %u\n", port);

    return 0;
}