#include "system/pi_time.h"
#include "system/network.h"

#include <stddef.h>
#include <stdio.h>

#include "parg.h"

int main(int argc, char *argv[])
{
    InitializeNetwork();
    InitializeTime();

    Socket socket = SocketCreate(SOCKET_IPV4, 60000);

    uint8_t data[1400];

    while(true)
    {
        Address addr;
        int d;
        if (SocketReceive(socket, &addr, data, sizeof(data)))
        {
            for (int i = 0; i < 32; i++)
                printf("%u", data[i]);
            printf("\n");
        }
    }

    return 0;
}