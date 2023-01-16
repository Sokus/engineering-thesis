#include <stdint.h>
#include <stdio.h>

#include "system/pi_time.h"
#include "system/network.h"

#include "system.h"
#include "macros.h"

#include <string.h>

int main(int argc, char *argv[])
{
    unsigned short server_port = 60000;

    InitializeNetwork();
    InitializeTime();

    Socket socket = SocketCreate(SOCKET_IPV4, 50000);
    Address address = AddressParseEx("127.0.0.1", 60000);
    int d = 5;
    SocketSend(socket, address, &d, sizeof(d));

    return 0;
}